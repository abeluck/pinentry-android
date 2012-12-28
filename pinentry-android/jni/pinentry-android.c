/* pinentry-curses.c - A secure curses dialog for PIN entry.
   Copyright (C) 2012 Abel Luck <abel@guardianproject.info>

   This file is part of PINENTRY.

   PINENTRY is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   PINENTRY is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/un.h>


// #include "pinentry.h"
// #include "pinentry-curses.h"
// 
#define SOCKET_NAME "info.guardianproject.gpg.pinentry"
// 
// int
// android_cmd_handler (pinentry_t pe)
// {
//   /*if (pe->pin)
//   {
//     if (pe->error)
// 	pinentry.setError (from_utf8 (pe->error));
//       if (pe->quality_bar)
// 	pinentry.setQualityBar (from_utf8 (pe->quality_bar));
//       if (pe->quality_bar_tt)
// 	pinentry.setQualityBarTT (from_utf8 (pe->quality_bar_tt));
//       int len = strlen (pin);
//       if (len >= 0)
// 	{
// 	  pinentry_setbufferlen (pe, len + 1);
// 	  if (pe->pin)
// 	    {
// 	      strcpy (pe->pin, pin);
// 	      return len;
// 	    }
// 	}
//   }*/
//   return 0;
// }
// 
// pinentry_cmd_handler_t pinentry_cmd_handler = android_cmd_handler;

int send_intent() {
 
  char command[PATH_MAX];

  sprintf(command, "/system/bin/am start -n info.guardianproject.gpg.pinentry/info.guardianproject.gpg.pinentry.PINEntry > /dev/null"); 
  
      static const char* const unsec_vars[] = {
        "GCONV_PATH",
        "GETCONF_DIR",
        "HOSTALIASES",
        "LD_AUDIT",
        "LD_DEBUG",
        "LD_DEBUG_OUTPUT",
        "LD_DYNAMIC_WEAK",
        "LD_LIBRARY_PATH",
        "LD_ORIGIN_PATH",
        "LD_PRELOAD",
        "LD_PROFILE",
        "LD_SHOW_AUXV",
        "LD_USE_LOAD_BIAS",
        "LOCALDOMAIN",
        "LOCPATH",
        "MALLOC_TRACE",
        "MALLOC_CHECK_",
        "NIS_PATH",
        "NLSPATH",
        "RESOLV_HOST_CONF",
        "RES_OPTIONS",
        "TMPDIR",
        "TZDIR",
        "LD_AOUT_LIBRARY_PATH",
        "LD_AOUT_PRELOAD",
        // not listed in linker, used due to system() call
        "IFS",
    };
    const char* const* cp = unsec_vars;
    const char* const* endp = cp + sizeof(unsec_vars)/sizeof(unsec_vars[0]);
    while (cp < endp) {
        unsetenv(*cp);
        cp++;
    }
    // sane value so "am" works
    setenv("LD_LIBRARY_PATH", "/vendor/lib:/system/lib", 1);
    setegid(getgid());
    seteuid(getuid());
    return system(command);
}

int main (int argc, char *argv[])
{
  printf("Welcome to pinentry-android\n");
  printf("starting java activity...\n");
  send_intent();
  sleep(3);
  pid_t child_pid;
  int child_status;
//   pinentry_init ("pinentry-android");

  /* Consumes all arguments.  */
//   if (pinentry_parse_opts (argc, argv))
//     {
//       printf ("pinentry-android (pinentry) " VERSION "\n");
//       exit (EXIT_SUCCESS);
//     }

  struct sockaddr_un addr;
  int fd;

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  addr.sun_path[0] = '\0';
  strncpy( &addr.sun_path[1], SOCKET_NAME, sizeof(addr.sun_path)-1 );
  /* calculate the length of our addrlen, for some reason this isn't simply
   * sizeof(addr), TODO: learn why, i suspect it has something to do with sun_path
   * being a char[108]
   */
  int len = offsetof(struct sockaddr_un, sun_path) + 1 + strlen(&addr.sun_path[1]);

  printf("connecting to Java socket server...\n");
  if (connect(fd, (struct sockaddr*)&addr, len) < 0) {
    perror("connect error");
    exit(-1);
  }

  printf("connection succeeded\n");
  /* remember Java doesn't understand \0 terminated strings*/
  int bytes_written = write(fd, "hello!", 6);
  printf("wrote %d bytes \n", bytes_written);
  close(fd);

  child_pid = fork();
  if( child_pid < 0 ) {
    /* unsuccessful fork */
    perror("fork:");
    exit(-1);
  }
  if( child_pid == 0 ) { /* child process */
    printf("child! bye\n");
    /* TODO: we actually don't want to pass the socket here..*/
//     if (pinentry_loop2 (fd, fd))
//       exit(1);
    exit(0);
  } else { /* parent process */
    wait(&child_status);
    exit(0);
  }

  return 0;
}
