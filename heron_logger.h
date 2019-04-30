#ifndef _HERON_LOGGER_H_
#define _HERON_LOGGER_H_


#include "heron_define.h"


namespace heron{namespace tati{
enum    log_level{
        log_level_trace = 1,
        log_level_debug = 2,
        log_level_event = 2,
        log_level_alert = 3,
        log_level_error = 4,
        log_level_vital = 5,
        log_level_fatal = 6,
};

void log_trace(const char *fmt, ...);
void log_debug(const char *fmt, ...);
void log_event(const char *fmt, ...);
void log_alert(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_vital(const char *fmt, ...);
void log_fatal(const char *fmt, ...);

}}//namespace heron::tati


#endif//_HERON_LOGGER_H_
