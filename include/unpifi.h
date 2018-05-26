#ifndef _unp_ifi_h
#define _unp_ifi_h

#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <stdlib.h>
#include <net/if.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define	MAXLINE		4096	/* max text line length */

#define IFI_NAME 16		/* IFNAMSIZ */
#define IFI_HADDR 6

struct ifi_info {
	char	ifi_name[IFI_NAME];
	short	ifi_index;
	short	ifi_mtu;
	u_char	ifi_haddr[IFI_HADDR];
	u_short	ifi_hlen;
	short ifi_flags;
	short ifi_myflags;
	struct sockaddr *ifi_addr;
	struct sockaddr *ifi_brdaddr;
	struct sockaddr *ifi_dstaddr;
	struct ifi_info *ifi_next;
};

#define IFI_ALIAS 1


#ifndef HAVE_INET_PTON_PROTO
int                      inet_pton(int, const char *, void *);
const char      *inet_ntop(int, const void *, char *, size_t);
#endif

struct ifi_info *get_ifi_info(int ,char *);
void free_ifi_info(struct ifi_info *);

#endif