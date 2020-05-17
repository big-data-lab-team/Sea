#ifndef CONFIG_H
#define CONFIG_H

typedef struct config
{
    bool parsed = false;
    int n_sources;
    char ** source_mounts;
    char * mount_dir;
    int log_level;
} config;

char * get_sea_home();
char * get_config_file();
struct config get_sea_config();

#endif