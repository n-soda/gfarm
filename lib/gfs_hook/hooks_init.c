/*
 * $Id$
 */

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <gfarm/gfarm.h>
#include <gfarm/gfs_hook.h>
#include "hooks_subr.h"

extern int gf_hook_default_global;

char *
gfs_hook_initialize(void)
{
	char *e;

	_gfs_hook_debug(fprintf(stderr, "GFS: gfs_hook_initialize\n"));

	/*
	 * allocate file descriptor greater than MIN_FD defined in
	 * hooks_subr.c for connection to metadata server.
	 */
	gfs_hook_reserve_fd();
	e = gfarm_initialize(NULL, NULL);
	gfs_hook_release_fd();
	if (e != NULL) {
		_gfs_hook_debug(
			fprintf(stderr, "GFS: gfs_hook_initialize: %s\n", e));
		return (e);
	}
	if (gf_hook_default_global)
		gfs_hook_set_default_view_global();

	if (gfs_pio_set_local_check() != NULL)
		gfs_pio_set_local(0, 1);

	/* exexute close_all() and gfarm_terminate() at program termination */
	atexit((void (*)(void))gfarm_terminate);
	atexit((void (*)(void))gfs_hook_close_all);

	return (NULL);
}
