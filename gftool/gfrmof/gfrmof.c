#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <gfarm/gfarm.h>

#include "gfm_client.h"
#include "lookup.h"
#include "gfarm_path.h"

static const char *program_name = "gfrmof";

#define GFARM_EXIT_USAGE 2

static void
usage(void)
{
	fprintf(stderr,
	    "Usage: %s [-P <path>] [-p <pid]> [-d <fd>] [-h <gfsd>]\n",
	    program_name);
	exit(GFARM_EXIT_USAGE);
}

long
parse_opt_long(char *option, int option_char, char *argument_name)
{
	long value;
	char *s;

	errno = 0;
	value = strtol(option, &s, 0);
	if (s == option) {
		fprintf(stderr, "%s: missing %s after -%c\n",
		    program_name, argument_name, option_char);
		usage();
	} else if (*s != '\0') {
		fprintf(stderr, "%s: garbage in -%c %s\n",
		    program_name, option_char, option);
		usage();
	} else if (errno != 0 && (value == LONG_MIN || value == LONG_MAX)) {
		fprintf(stderr, "%s: %s with -%c %s\n",
		    program_name, strerror(errno), option_char, option);
		usage();
	}
	return (value);
}

int
main(int argc, char **argv)
{
	gfarm_error_t e;
	int exit_code = EXIT_FAILURE;
	struct gfm_connection *gfm_server = NULL;
	gfarm_pid_t pid = -1;
	int c, n, fd = -1;
	char *gfsd_hostname = NULL;
	static const char opt_path_default[] = ".";
	const char *opt_path = opt_path_default;
	char *realpath = NULL;

	if (argc > 0)
		program_name = basename(argv[0]);

	e = gfarm_initialize(&argc, &argv);
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", program_name,
		    gfarm_error_string(e));
		exit(EXIT_FAILURE);
	}

	while ((c = getopt(argc, argv, "P:d:h:p:?")) != -1) {
		switch (c) {
		case 'P':
			opt_path = optarg;
			break;
		case 'd':
			fd = parse_opt_long(optarg, c, "<fd>");
			break;
		case 'h':
			gfsd_hostname = optarg;
			break;
		case 'p':
			pid = parse_opt_long(optarg, c, "<pid>");
			break;
		case '?':
		default:
			usage();
			/*NOTREACHED*/
		}
	}
	argc -= optind;
	argv += optind;
	if (argc > 0) {
		fprintf(stderr, "%s: extra operand `%s'\n",
		    program_name, argv[0]);
		usage();
		/*NOTREACHED*/
	}

	if (pid == -1 && fd == -1 && gfsd_hostname == NULL) {
		fprintf(stderr, "%s: at least one of the -p, -d, or -h options"
		    " must be specified\n",
		    program_name);
		exit(GFARM_EXIT_USAGE);
	}
	if (gfsd_hostname == NULL)
		gfsd_hostname = "";

	if (gfarm_realpath_by_gfarm2fs(opt_path, &realpath)
	    == GFARM_ERR_NO_ERROR)
		opt_path = realpath;
	if ((e = gfm_client_connection_and_process_acquire_by_path(
	    opt_path, &gfm_server)) != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: metadata server for \"%s\": %s\n",
		    program_name, opt_path, gfarm_error_string(e));
		exit(EXIT_FAILURE);
	}
	free(realpath);

	e = gfm_client_process_fd_remove(gfm_server, pid, fd, gfsd_hostname,
	    &n);
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", program_name,
		    gfarm_error_string(e));
	} else if (n == 0) {
		fprintf(stderr, "%s: %s\n", program_name,
		    gfarm_error_string(GFARM_ERR_NO_SUCH_PROCESS));
	} else {
		printf("%d %s removed\n", n, n == 1 ? "entry" : "entries");
		exit_code = EXIT_SUCCESS;
	}

	e = gfarm_terminate();
	if (e != GFARM_ERR_NO_ERROR) {
		fprintf(stderr, "%s: %s\n", program_name,
		    gfarm_error_string(e));
		exit(EXIT_FAILURE);
	}
	return (exit_code);
}

