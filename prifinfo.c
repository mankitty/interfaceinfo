#include "unpifi.h"
int main(int argc,char **argv)
{
	int i;
	u_char *ptr;
	struct sockaddr *sa;
	struct ifi_info *ifi,*ifihead;
	
	if (argc > 2)
		err_quit("usage: prifinfo <device>");
	
	ifi = get_ifi_info(AF_INET,argv[1]);
	if (ifi == NULL) {
		err_quit("Can't find interface or interface state is down !!!");
	}

	for (ifihead = ifi;ifi != NULL;ifi = ifi->ifi_next) {
		printf("%s: flags=%d<", ifi->ifi_name,ifi->ifi_flags);
/* *INDENT-OFF* */
		if (ifi->ifi_flags & IFF_UP)			printf("UP ");
		if (ifi->ifi_flags & IFF_BROADCAST)		printf("BCAST ");
		if (ifi->ifi_flags & IFF_MULTICAST)		printf("MCAST ");
		if (ifi->ifi_flags & IFF_LOOPBACK)		printf("LOOP ");
		if (ifi->ifi_flags & IFF_POINTOPOINT)	printf("P2P ");
		printf(">\n");
/* *INDENT-ON* */

		if ( (i = ifi->ifi_hlen) > 0) {
			ptr = ifi->ifi_haddr;
			do {
				printf("%s%x", (i == ifi->ifi_hlen) ? "  " : ":", *ptr++);
			} while (--i > 0);
			printf("\n");
		}

		if ((sa = ifi->ifi_addr) != NULL)
			printf("  IP addr: %s\n",Sock_ntop_host(sa, sizeof(*sa)));
		if ((sa = ifi->ifi_brdaddr) != NULL)
			printf("  broadcast addr: %s\n",Sock_ntop_host(sa, sizeof(*sa)));
		if ((sa = ifi->ifi_dstaddr) != NULL)
			printf("  destination addr: %s\n",Sock_ntop_host(sa, sizeof(*sa)));
		if (ifi->ifi_haddr != NULL)
			printf("  ether %02x:%02x:%02x:%02x:%02x:%02x\n",ifi->ifi_haddr[0],ifi->ifi_haddr[1],
				ifi->ifi_haddr[2],ifi->ifi_haddr[3],ifi->ifi_haddr[4],ifi->ifi_haddr[5]);
	}
	free_ifi_info(ifihead);
	return 0;
}
