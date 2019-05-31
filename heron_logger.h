#ifndef _HERON_LOGGER_H_
#define _HERON_LOGGER_H_


#include "heron_define.h"
#include "heron_routine.h"
#include <iostream>
#include <stdarg.h>
#include <cstdio>
#include <sys/time.h>


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

void    log_error(const char *format, ...);
void    log_trace(const char *format, ...);
void    log_alert(const char *format, ...);
void    log_event(const char *format, ...);
sint	log_append(log_level level, const char *fmt, va_list ap);

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
	sint    log_error(const char *fmt, ...){
                va_list ap;
                va_start(ap, fmt);
                sint    result = log_append(log_level_error, fmt, ap);
                va_end(ap);
                return  result;
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
		const   char*	levels[] = {"trace","debug","event","alert",
			"error","vital","panic","fatal"};
		char	buf[1024] = { 0 };

		struct	timeval		m_time;
		struct  tm  tt;

		gettimeofday(&m_time, nullptr);
		localtime_r(&m_time.tv_sec, &tt);

		sint shift = snprintf(buf, sizeof(buf), "[%04d%02d%02d %02d:%02d:%02d-%s]",
				tt.tm_year + 1900, tt.tm_mon, tt.tm_mday,
				tt.tm_hour, tt.tm_min, tt.tm_sec, levels[level-1]);


		sint len = vsnprintf(buf+shift, sizeof(buf)-shift, fmt, ap);
		buf[len+shift]='\n';
		cout << buf << endl;
		return	heron_result_state::success;
	}
        heron_synch_buffer*     m_buff;
};
}}//namespace heron::tati


#endif//_HERON_LOGGER_H_
