#include <iostream>
#include <ctime>

//封装一个日志宏，通过日志宏进行日志的打印，在打印的信息前带有系统时间以及文件名和行号
//     [17:26:24] [log.cpp:12] 打开文件失败！ 

#define DBG_LEVEL 0
#define INF_LEVEL 1
#define ERR_LEVEL 2
#define DEFAULT_LEVEL DBG_LEVEL
#define LOG(lev_str, level, format, ...) {\
    if (level >= DEFAULT_LEVEL) {\
        time_t t = time(nullptr);\
        struct tm* ptm = localtime(&t);\
        char time_str[32];\
        strftime(time_str, 31, "%H:%M:%S", ptm);\
        printf("[%s][%s][%s:%d]\t" format "\n", lev_str, time_str, __FILE__, __LINE__, DBG_LEVEL);\
    }\
}

#define DLOG(format, ...) LOG("DBG", DBG_LEVEL, format, DBG_LEVEL)
#define ILOG(format, ...) LOG("INF", INF_LEVEL, format, DBG_LEVEL)
#define ELOG(format, ...) LOG("ERR", ERR_LEVEL, format, DBG_LEVEL)

int main() 
{
    DLOG("Hello World");
    ILOG("Hello World");
    ELOG("Hello World");
    return 0;
}