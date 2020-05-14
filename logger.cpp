#include "logger.h"
#include "passthrough.h"

#include <time.h>
#include <string.h>

// maybe use tmpnam?
static const char* log_fn = "pass.log";

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

    if (LOG_FOREGROUND)
        fprintf(stderr, "%s: %s: %s\n", strtok(asctime(timeinfo), "\n"), get_lvlname(lvl), fmsg);

    else {
        FILE* logs = ((funcptr_fopen)libc_fopen)(log_fn, "a+");
        // write complete log string to file
        if (logs == NULL)
        {
            xprintf("WARNING: Cannot write to log file %s: %s (%s)\n", log_fn, msg, get_lvlname(lvl));
            return 1;
        }
        fprintf(logs, "%s: %s: %s\n", strtok(asctime(timeinfo), "\n"), get_lvlname(lvl), fmsg);
        fclose(logs);
    }
    return 0;
}


//FILE* log_fopen(const char *path, const char *mode){
//    if(libc_fopen)
//        return ((funcptr_fopen)libc_fopen)(path, mode);
//    return NULL;
//}
