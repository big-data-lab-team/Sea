#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include "logger.h"
#include "config.h"
#include "passthrough.h"

/**
 * Directories which do not exist in all source mounts are created in all the mounts.
 *
 * @param basePath the root directory to start adding paths from
 * @param sea_lvl the index of the basePath's parent source mount
 * @param sea_config the sea configuration struct
 *
 */
void mirrorSourceDirs(char *basePath, int sea_lvl, struct config sea_config)
{
	log_msg(DEBUG, "mirrorSourceDirs: Attempting to mirror directories at %s", basePath);
	char path[PATH_MAX];
	struct dirent *dp;
	DIR *dir = opendir(basePath);

	// Unable to open directory stream
	if (!dir)
		return;

	while ((dp = readdir(dir)) != NULL)
	{
		// Construct new path from our base path
		strcpy(path, basePath);

		strcat(path, "/");
		strcat(path, dp->d_name);

		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			if (dp->d_type == DT_DIR)
			{
				struct stat buf;
				if (stat(path, &buf) == 0)
				{

					for (int i = 0; i < sea_config.n_sources; ++i)
					{
						if (i != sea_lvl)
						{
							char dir_to_create[PATH_MAX];

							strcpy(dir_to_create, sea_config.source_mounts[i]);

							char *match;
							int len = strlen(sea_config.source_mounts[sea_lvl]);
							char *tmp = strdup(basePath);

							if ((match = strstr(tmp, sea_config.source_mounts[sea_lvl])))
							{
								if (match != NULL && match[0] != '\0')
								{
									*match = '\0';
									strcat(dir_to_create, match + len);
								}
							}

							strcat(dir_to_create, "/");
							strcat(dir_to_create, dp->d_name);

							//TODO: add error handling here
							log_msg(INFO, "mirrorSourceDirs: Creating dir %s %d", dir_to_create, buf.st_mode);
							mkdir(dir_to_create, buf.st_mode);
							// don't care if directory already exists
							if (errno == EEXIST)
								errno = 0;
						}
					}
				}
			}
			mirrorSourceDirs(path, sea_lvl, sea_config);
		}
	}
	log_msg(DEBUG, "mirrorSourceDirs: Completed");
	closedir(dir);
}

int main(void)
{
	initialize_passthrough_if_necessary();
	struct config sea_config = get_sea_config();

	for (int i = 0; i < sea_config.n_sources; i++)
	{
		mirrorSourceDirs(sea_config.source_mounts[i], i, sea_config);
	}
}