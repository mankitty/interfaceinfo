#include "unpifi.h"

#define defaultDevNum 3

struct ifi_info *
get_ifi_info(int family,char *device)
{
	struct ifi_info *ifi,*ifihead,**ifipnext;
	int sockfd,len,lastlen,flags,myflags,idx = 0,hlen = 0;
	char *ptr,*buf,lastname[IFNAMSIZ],*cptr,*haddr,*sdlname;

	struct ifconf ifc;
	struct ifreq *ifr,ifrcopy;
	struct sockaddr_in *sinptr;
	struct sockaddr_in6 *sin6ptr;

	sockfd = socket(family,SOCK_DGRAM,0);
	if (sockfd == -1)
		err_sys("create scoket failed !!!\n");

	lastlen = 0;
	len = defaultDevNum * sizeof(struct ifreq);

	for ( ; ; ) {
		buf = malloc(len);
		if (buf == NULL)
			err_sys("malloc failed !!!\n");
		ifc.ifc_len = len;
		ifc.ifc_buf = buf;
		if (ioctl(sockfd,SIOCGIFCONF,&ifc) < 0) {
			if (errno != EINVAL || lastlen != 0)
				err_sys("ioctl call SIOCGIFCONF failed !!!");
		} else {
			if (ifc.ifc_len == lastlen)
				break;
			lastlen = ifc.ifc_len;
		}
		len += 10 * sizeof(struct ifreq);
		free(buf);
	}

	ifihead = NULL;
	ifipnext = &ifihead;
	lastname[0] = 0;
	sdlname = NULL;

	for (ptr = buf ; ptr < buf + ifc.ifc_len; ptr++) {
		ifr = (struct ifreq *)ptr;
		//ptr += sizeof(ifr->ifr_name) + len;

		if (ifr->ifr_addr.sa_family != family) {
			continue;
		}

		myflags = 0;
		if ((cptr = strchr(ifr->ifr_name,':')) != NULL) {
			*cptr = 0;
		}
		if ((device != NULL) && (strncmp(ifr->ifr_name,device,IFNAMSIZ) != 0)) {
			continue;
		}
		if (strncmp(lastname,ifr->ifr_name,IFNAMSIZ) == 0) {
			continue;
		}
		memcpy(lastname,ifr->ifr_name,IFNAMSIZ);
		ifrcopy = *ifr;
		
		if (-1 == ioctl(sockfd,SIOCGIFFLAGS,&ifrcopy))
			err_sys("ioctl SIOCGIFFLAGS failed");
		flags = ifrcopy.ifr_flags;
		
		if ((flags & IFF_UP) == 0) {
			//continue;
		}
		
		ifi = calloc(1,sizeof(struct ifi_info));
		if (ifi == NULL)
			err_sys("calloc for ifi failed");
		*ifipnext = ifi;
		ifipnext = &ifi->ifi_next;

		ifi->ifi_flags = flags;

		if (-1 == ioctl(sockfd,SIOCGIFMTU,&ifrcopy))
			err_sys("ioctl call SIOCGIFMTU failed !!!");
		ifi->ifi_mtu = ifrcopy.ifr_mtu;

		memcpy(ifi->ifi_name, ifr->ifr_name, IFI_NAME);
		ifi->ifi_name[IFI_NAME-1] = '\0';
		if (sdlname == NULL || strcmp(sdlname,ifr->ifr_name) != 0)
			idx = hlen = 0;
		ifi->ifi_index = idx;
		ifi->ifi_hlen = hlen;
		if (ifi->ifi_hlen > IFI_HADDR)
			ifi->ifi_hlen = IFI_HADDR;
		if (hlen)
			memcpy(ifi->ifi_haddr,haddr,ifi->ifi_hlen);
		
		if (ifr->ifr_addr.sa_family != AF_INET)
			continue;
		sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
		ifi->ifi_addr = calloc(1, sizeof(struct sockaddr_in));
		if (!ifi->ifi_addr)
			err_sys("calloc for ifi->ifi_addr failed !!!");
		
		memcpy(ifi->ifi_addr, sinptr, sizeof(struct sockaddr_in));

		if (flags & IFF_BROADCAST) {
			if (ioctl(sockfd, SIOCGIFBRDADDR, &ifrcopy) == -1)
				err_sys("ioctl call SIOCGIFBRDADDR failed !!!");
			
			sinptr = (struct sockaddr_in *) &ifrcopy.ifr_broadaddr;
			ifi->ifi_brdaddr = calloc(1, sizeof(struct sockaddr_in));
			if (!ifi->ifi_brdaddr)
				err_sys("calloc for ifi->ifi_brdaddr failed !!!");
			
			memcpy(ifi->ifi_brdaddr, sinptr, sizeof(struct sockaddr_in));
		}

		if (flags & IFF_POINTOPOINT) {
			if (ioctl(sockfd, SIOCGIFDSTADDR, &ifrcopy) == -1)
				err_sys("ioctl call SIOCGIFDSTADDR failed !!!");
			
			sinptr = (struct sockaddr_in *) &ifrcopy.ifr_dstaddr;
			ifi->ifi_dstaddr = calloc(1, sizeof(struct sockaddr_in));
			if (!ifi->ifi_dstaddr)
				err_sys("calloc for ifi->ifi_dstaddr failed !!!");
			
			memcpy(ifi->ifi_dstaddr, sinptr, sizeof(struct sockaddr_in));
		}
		if (-1 == ioctl(sockfd,SIOCGIFHWADDR,&ifrcopy))
			err_sys("ioctl call SIOCGIFHWADDR failed !!!");
		memcpy(ifi->ifi_haddr,ifrcopy.ifr_hwaddr.sa_data,IFI_HADDR);
		//unsigned char* hw=ifrcopy.ifr_hwaddr.sa_data;  
        //printf("SIOCGIFHWADDR:%02x:%02x:%02x:%02x:%02x:%02x\n",hw[0],hw[1],hw[2],hw[3],hw[4],hw[5]);  
	}
	free(buf);
	return (ifihead);
}

/* include free_ifi_info */
void
free_ifi_info(struct ifi_info *ifihead)
{
	struct ifi_info	*ifi, *ifinext;

	for (ifi = ifihead; ifi != NULL; ifi = ifinext) {
		if (ifi->ifi_addr != NULL)
			free(ifi->ifi_addr);
		if (ifi->ifi_brdaddr != NULL)
			free(ifi->ifi_brdaddr);
		if (ifi->ifi_dstaddr != NULL)
			free(ifi->ifi_dstaddr);
		ifinext = ifi->ifi_next;	/* can't fetch ifi_next after free() */
		free(ifi);					/* the ifi_info{} itself */
	}
}
/* end free_ifi_info */
