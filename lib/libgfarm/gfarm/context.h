extern struct gfarm_context {
	/* global variables in config.c */
	char *metadb_server_name;
	int metadb_server_port;
	char *metadb_admin_user;
	char *metadb_admin_user_gsi_dn;

	int log_level;
	int no_file_system_node_timeout;
	int gfmd_reconnection_timeout;
	int attr_cache_limit;
	int attr_cache_timeout;
	int schedule_cache_timeout;
	float schedule_idle_load;
	float schedule_busy_load;
	float schedule_virtual_load;
	int gfmd_connection_cache;
	int gfsd_connection_cache;
	int record_atime;
	int client_file_bufsize;
	int on_demand_replication;
	int network_receive_timeout;
	int file_trace;

	/* platform dependent constant */
	int getpw_r_bufsz;

	/* gfs_profile.c */
	int profile;

	/* static variables */
	struct gfarm_config_static *config_static;
	struct gfarm_sockopt_static *sockopt_static;
	struct gfm_client_static *gfm_client_static;
	struct gfs_client_static *gfs_client_static;
	struct gfarm_host_static *host_static;
	struct gfarm_auth_config_static *auth_config_static;
	struct gfarm_auth_common_static *auth_common_static;
	struct gfarm_auth_common_gsi_static *auth_common_gsi_static;
	struct gfarm_auth_client_static *auth_client_static;
	struct gfarm_liberror_static *liberror_static;
	struct gfarm_schedule_static *schedule_static;
	struct gfarm_gfs_pio_static *gfs_pio_static;
	struct gfarm_gfs_pio_section_static *gfs_pio_section_static;
	struct gfarm_gfs_stat_static *gfs_stat_static;
	struct gfarm_gfs_unlink_static *gfs_unlink_static;
	struct gfarm_gfs_xattr_static *gfs_xattr_static;
	struct gfarm_filesystem_static *filesystem_static;
} *gfarm_ctxp;

gfarm_error_t gfarm_context_init(void);

gfarm_error_t gfarm_config_static_init(struct gfarm_context *);
gfarm_error_t gfarm_sockopt_static_init(struct gfarm_context *);
gfarm_error_t gfm_client_static_init(struct gfarm_context *);
gfarm_error_t gfs_client_static_init(struct gfarm_context *);
gfarm_error_t gfarm_host_static_init(struct gfarm_context *);
gfarm_error_t gfarm_auth_config_static_init(struct gfarm_context *);
gfarm_error_t gfarm_auth_common_static_init(struct gfarm_context *);
gfarm_error_t gfarm_auth_common_gsi_static_init(struct gfarm_context *);
gfarm_error_t gfarm_auth_client_static_init(struct gfarm_context *);
gfarm_error_t gfarm_liberror_static_init(struct gfarm_context *);
gfarm_error_t gfarm_schedule_static_init(struct gfarm_context *);
gfarm_error_t gfarm_gfs_pio_static_init(struct gfarm_context *);
gfarm_error_t gfarm_gfs_pio_section_static_init(struct gfarm_context *);
gfarm_error_t gfarm_gfs_stat_static_init(struct gfarm_context *);
gfarm_error_t gfarm_gfs_unlink_static_init(struct gfarm_context *);
gfarm_error_t gfarm_gfs_xattr_static_init(struct gfarm_context *);
gfarm_error_t gfarm_filesystem_static_init(struct gfarm_context *);
