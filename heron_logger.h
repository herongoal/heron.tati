#ifndef _HERON_LOGGER_H_
#define _HERON_LOGGER_H_


#include "heron_define.h"
#include "heron_routine.h"


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

class	heron_synch_buffer;
class   heron_log_reader:public heron_routine{
public:
        heron_log_reader(heron_synch_buffer *buff, int fd);
        static  heron_log_reader*  create(heron_synch_buffer *buff, int fd);
	virtual bool    vital() const
        {
               return true;
        }
protected:
        heron_synch_buffer*     m_buff;
};

class   heron_log_writer:public heron_routine{
public:
        heron_log_writer(heron_synch_buffer *buff, int fd);
        static  heron_log_writer*  create(heron_synch_buffer *buff, int fd);
	virtual bool    vital() const{
               return true;
        }
	sint	log_event(const char *fmt, ...);
	sint	log_alert(const char *fmt, ...);
	sint	log_vital(const char *fmt, ...);
	sint	log_fatal(const char *fmt, ...);
protected:
        heron_synch_buffer*     m_buff;
};
}}//namespace heron::tati


#endif//_HERON_LOGGER_H_
