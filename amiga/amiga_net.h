/* Netacka for GFOS: the BSD socket calls of libnet's internet driver
 * (libnet/lib/drivers/internet.c, inetaddr.c) on Roadshow's
 * bsdsocket.library. The makefile includes this ahead of those two only.
 * - SocketBase is opened at start (amiga_net.c) and stays NULL when no
 *   TCP/IP stack runs: then every call fails like a closed network and
 *   libnet's internet driver does not start.
 * - close() on a socket is CloseSocket(); fcntl(O_NONBLOCK) is FIONBIO;
 *   select() is WaitSelect(); socket errors land in errno (the library is
 *   told where errno lives when it opens).
 * Copyright (c) 2026 AmiCradle Team. MIT, as Netacka. */
#ifndef NETACKA_AMIGA_NET_H
#define NETACKA_AMIGA_NET_H
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <proto/bsdsocket.h>

extern struct Library *SocketBase;
int amiga_net_fcntl (int s, int cmd, ...);

#define AMIGA_NET_UP (SocketBase != NULL)

static inline int amiga_net_close (int s)
{ return AMIGA_NET_UP ? (int) CloseSocket (s) : -1; }
static inline int amiga_net_select (int n, fd_set *r, fd_set *w, fd_set *e,
                                    struct timeval *t)
{ return AMIGA_NET_UP ? (int) WaitSelect (n, r, w, e, t, NULL) : -1; }
static inline int amiga_net_socket (int d, int t, int p)
{ return AMIGA_NET_UP ? (int) socket (d, t, p) : -1; }
static inline struct hostent *amiga_net_gethostbyname (const char *n)
{ return AMIGA_NET_UP ? gethostbyname ((STRPTR) n) : (struct hostent *) 0; }
static inline int amiga_net_inet_aton (const char *c, struct in_addr *a)
{ return AMIGA_NET_UP ? (int) inet_aton ((STRPTR) c, a) : 0; }
static inline char *amiga_net_inet_ntoa (struct in_addr a)
{ return AMIGA_NET_UP ? (char *) Inet_NtoA (a.s_addr) : (char *) "0.0.0.0"; }

#undef close
#define close amiga_net_close
#undef fcntl
#define fcntl amiga_net_fcntl
#undef select
#define select amiga_net_select
#undef socket
#define socket amiga_net_socket
#undef gethostbyname
#define gethostbyname amiga_net_gethostbyname
#undef inet_aton
#define inet_aton amiga_net_inet_aton
#undef inet_ntoa
#define inet_ntoa amiga_net_inet_ntoa
#endif
