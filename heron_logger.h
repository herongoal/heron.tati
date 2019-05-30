#ifndef _HERON_LOGGER_H_
#define _HERON_LOGGER_H_


#include "heron_define.h"
#include "heron_routine.h"
#include <iostream>
#include <stdarg.h>


using namespace std;


namespace heron{namespace tati{
enum    log_level{
        log_level_trace = 1,
        log_level_debug = 2,
        log_level_event = 3,
        log_level_alert = 4,
        log_level_error = 5,
        log_level_vital = 6,
	log_level_panic = 7,
        log_level_fatal = 8,
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
	virtual sint    append_send_data(const void *data, unsigned len);
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
	//inherited from heron_routine
	sint    append_send_data(const void *data, unsigned len){
		return	heron_result_state::success;
	}
	sint	log_event(const char *fmt, ...){
		va_list ap;
                va_start(ap, fmt);
		sint	result = log_append(log_level_event, fmt, ap);
                va_end(ap);
		return	result;
	}
	sint	log_vital(const char *fmt, ...){
		va_list ap;
                va_start(ap, fmt);
		sint	result = log_append(log_level_vital, fmt, ap);
                va_end(ap);
		return	result;
	}
	sint	log_alert(const char *fmt, ...){
		va_list ap;
                va_start(ap, fmt);
		sint	result = log_append(log_level_alert, fmt, ap);
                va_end(ap);
		return	result;
	}
	sint	log_panic(const char *fmt, ...){
		va_list ap;
                va_start(ap, fmt);
		sint	result = log_append(log_level_panic, fmt, ap);
                va_end(ap);
		return	result;
	}
	sint	log_fatal(const char *fmt, ...){
		va_list ap;
                va_start(ap, fmt);
		sint	result = log_append(log_level_fatal, fmt, ap);
                va_end(ap);
		return	result;
	}
protected:
	sint	log_append(log_level level, const char *fmt, va_list ap){
		char*	levels[] = {"trace","debug","event","alert",
			"error","vital","panic","fatal"};
		sint shift=5;
		char	buf[1024] = { 0 };
		memcpy(buf,levels[level-1],shift);
		sint len = vsnprintf(buf+shift, sizeof(buf)-shift, fmt, ap);
		buf[len+shift]='\n';
		cout << buf << endl;
		return	heron_result_state::success;
	}
        heron_synch_buffer*     m_buff;
};
}}//namespace heron::tati


#endif//_HERON_LOGGER_H_
