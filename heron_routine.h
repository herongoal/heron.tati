#ifndef _HERON_ROUTINE_H_
#define _HERON_ROUTINE_H_


#include "heron_define.h"
#include "heron_buffer.h"


#include <netinet/in.h>
#include <sys/epoll.h>


namespace heron{namespace tati{
class	heron_routine;
typedef  void    (*conn_handle_routine)(heron_routine *rt, heron_event ev);
typedef  void    (*data_handle_routine)(heron_routine *rt, heron_event ev);
typedef  void    (*time_handle_routine)(heron_routine *rt, heron_event ev);

void    unregister_events(sint epoll_fd, heron_routine *rt);


class        heron_routine{
public:
        heron_routine(ulong label, sint fd):m_managed_events(0), m_fd(fd){}
        virtual         ~heron_routine(){}

        virtual	sint    on_events(heron_event ev) = 0;
        virtual sint	append_send_data(const void*, uint) = 0;

        virtual bool    vital() const {
               return false;
        }

        virtual sint    inspect(){
		return 0;
	}

        virtual sint    get_type(){
		return 0;
	}

	void	add_routine(heron_routine *rt);

        uint    get_managed_events() const{
		return  m_managed_events;
	} 
        virtual uint    get_changed_events() = 0;

protected:
        int             do_nonblock_write(const void *buf, unsigned len, unsigned &bytes_sent);
        int             do_nonblock_write();

        /**
         * return value:
         * > 0  received length
         * = 0  not all received
         * < 0  error occurred
         */
        int                do_nonblock_recv(heron_buffer &hb);

        void               close_fd(){
	}

public:
	uint	m_managed_events;
	bool	m_close_mark;
        heron_buffer    *m_send;
        heron_buffer    *m_recv;
        ulong           m_routine_id;
        ulong           m_timeout;
        uint            m_proxy_id;
        heron_routine_attr            m_attr;
        heron_routine_stat            m_stat;
        struct  sockaddr_in     m_peer_addr;

        bool                    m_writable;
        int                     m_fd;

        ulong                   m_time_to_close;
        ulong                   m_last_sync_time;
        ulong                   m_last_inspect_time;
};
}}//namespace heron::tati


#endif //_HERON_ROUTINE_H_
