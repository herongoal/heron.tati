#ifndef _HERON_LOGGER_H_
#define _HERON_LOGGER_H_


#include "heron_define.h"
#include "heron_channel.h"


namespace heron{namespace tati{
void log_trace(const char *fmt, ...);
void log_debug(const char *fmt, ...);
void log_event(const char *fmt, ...);
void log_alert(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_vital(const char *fmt, ...);
void log_fatal(const char *fmt, ...);

class   heron_log_channel{
public:
        heron_log_channel();
        static  heron_log_channel*    create();
private:
        heron_synch_buffer      m_synch_buff;
        int                     m_socketpair[2];
        heron_channel_routine*  m_routines[2];
};

class   heron_log_reader:public heron_routine{
public:
        heron_log_reader(heron_synch_buffer *buff, int fd);
        static  heron_log_reader*  create(heron_synch_buffer *buff, int fd);
	virtual bool    vital() const
        {
               return false;
        }
protected:
        heron_synch_buffer*     m_buff;
};

class   heron_log_writer:public heron_routine{
public:
        heron_log_writer(heron_synch_buffer *buff, int fd);
        static  heron_log_writer*  create(heron_synch_buffer *buff, int fd);
	virtual bool    vital() const
        {
               return false;
        }
protected:
        heron_synch_buffer*     m_buff;
};
}}//namespace heron::tati


#endif//_HERON_LOGGER_H_
