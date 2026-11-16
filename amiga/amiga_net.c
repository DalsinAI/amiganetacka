/* bsdsocket.library for Netacka's network game: opened when the game
 * starts if a TCP/IP stack (Roadshow) runs, closed when it ends. Without
 * one SocketBase stays NULL and amiga_net.h fails every call.
 * Copyright (c) 2026 AmiCradle Team. MIT, as Netacka. */
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <proto/exec.h>
#include <proto/bsdsocket.h>
#include <amitcp/socketbasetags.h>

struct Library *SocketBase;

/* The only fcntl() calls are F_SETFL with O_NONBLOCK on new sockets. */
int amiga_net_fcntl (int s, int cmd, ...)
{
  va_list ap;
  long arg, on;

  if (SocketBase == NULL || cmd != F_SETFL)
    return -1;
  va_start (ap, cmd);
  arg = va_arg (ap, long);
  va_end (ap);
  on = (arg & O_NONBLOCK) != 0;
  return IoctlSocket (s, FIONBIO, (char *) &on) < 0 ? -1 : 0;
}

static void __attribute__((constructor)) amiga_net_open (void)
{
  SocketBase = OpenLibrary ("bsdsocket.library", 4);
  if (SocketBase != NULL)
    SocketBaseTags (SBTM_SETVAL (SBTC_ERRNOLONGPTR), (ULONG) &errno,
                    TAG_DONE);
}

static void __attribute__((destructor)) amiga_net_end (void)
{
  if (SocketBase != NULL)
    {
      CloseLibrary (SocketBase);
      SocketBase = NULL;
    }
}
