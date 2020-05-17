#include "logger.h"
#include "passthrough.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>


const char* get_lvlname(int lvl){
    switch(lvl){
        case 1:
            return "ERROR";
        case 2:
            return "WARNING";
        case 3:
            return "INFO";
        default:
            return "DEBUG";
    }
}

int log_msg(int lvl, const char* msg, ...){
    char log_fn[PATH_MAX];
    *log_fn = '\0';
    char * sea_home = get_sea_home();
    strcat(log_fn, sea_home);
    strcat(log_fn, "/sea.log");
    if (lvl > DEBUG_LVL)
        return 0;

    //get current time
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    //format input string
    char fmsg[MAX_LOG];
    va_list arglist;
    va_start( arglist, msg );
    vsprintf(fmsg, msg ,arglist);
    va_end( arglist );

    char* currtime;
    char* formattime = (char *)malloc(sizeof(char*) * 7);
    if (( currtime = asctime(timeinfo)) != NULL)
       formattime = strtok(currtime, "\n"); 
    else
        strcpy(formattime, "UNKNOWN");

    if (LOG_FOREGROUND)
        fprintf(stderr, "%s: %s: %s\n", formattime, get_lvlname(lvl), fmsg);

    else {
        FILE* logs = ((funcptr_fopen)libc_fopen)(log_fn, "a+");
        // write complete log string to file
        if (logs == NULL)
        {
            xprintf("WARNING: Cannot write to log file %s: %s (%s)\n", log_fn, msg, get_lvlname(lvl));
            return 1;
        }
        fprintf(logs, "%s: %s: %s\n", formattime, get_lvlname(lvl), fmsg);
        fclose(logs);
    }
    return 0;
}


//FILE* log_fopen(const char *path, const char *mode){
//    if(libc_fopen)
//        return ((funcptr_fopen)libc_fopen)(path, mode);
//    return NULL;
//}
