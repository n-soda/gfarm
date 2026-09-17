#include <gfarm/gfarm_config.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "gfutil.h"

#include <gfarm/gflog.h>

#ifndef HAVE_DAEMON
int
gfarm_daemon(int not_chdir, int not_close)
{
	int save_errno;
long child_pid;
	switch (child_pid = fork()) {
	case -1:
		save_errno = errno;
		gflog_debug(GFARM_MSG_1000766, "fork() failed: %s",
			strerror(save_errno));
gflog_info(GFARM_MSG_UNFIXED, "fork() failed: %s", strerror(save_errno));

		errno = save_errno;
		return (-1);
	case 0:
gflog_info(GFARM_MSG_UNFIXED, "fork() child ppid %d pid %d", (int)getppid(), (int)getpid());
		break;
	default:
gflog_info(GFARM_MSG_UNFIXED, "fork() parent pid %d child pid %d", (int)getpid(), (int)child_pid);
		exit(0);
	}
	setsid();
	if (!not_chdir) {
		if (chdir("/") == -1)
			gflog_debug_errno(GFARM_MSG_1005109, "chdir(/)");
	}
	    
	if (!not_close) {
		int fd = open("/dev/null", O_RDWR, 0);

		if (fd != -1) {
			dup2(fd, 0);
			dup2(fd, 1);
			dup2(fd, 2);
			if (fd > 2)
				close(fd);
		}
	}
	return (0);
}
#endif /* !HAVE_DAEMON */
