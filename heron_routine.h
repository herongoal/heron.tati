#ifndef _HERON_ROUTINE_H_
#define _HERON_ROUTINE_H_


#include "heron_define.h"
#include "heron_buffer.h"


#include <netinet/in.h>


namespace heron{namespace tati{
class	heron_routine;
typedef  void    (*conn_handle_routine)(heron_routine *rt, heron_event ev);
typedef  void    (*data_handle_routine)(heron_routine *rt, heron_event ev);
typedef  void    (*time_handle_routine)(heron_routine *rt, heron_event ev);

void    unregister_events(sint epoll_fd, heron_routine *rt);


class        heron_routine{
public:
        heron_routine(ulong label, sint fd);
        virtual         ~heron_routine();

        virtual     int     on_event(heron_event ev);
        sint append_send_data(const void*, uint);

        virtual bool    vital() const
        {
               return false;
        }
        virtual sint    inspect()
	{
		return 0;
	}

	void	add_routine(heron_routine *rt);

	bool	m_close_mark;
        /**
         * Used to return epoll events for epoll registering.
         */
        virtual        uint        get_events() = 0;


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

        void               close_fd();

public:
        heron_buffer    m_send;
        heron_buffer    m_recv;
        ulong           m_routine_id;
	bool		m_del_flag;
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

class   heron_tcp_routine: public heron_routine{
public:
	static  heron_tcp_routine*      create(uint label, int fd);
        heron_tcp_routine();
        virtual ~heron_tcp_routine();
        sint on_event(heron_event ev);
        sint do_connect(ulong, const char *ipaddr, uint16_t port);
	sint check_conn_state(int err);
	virtual bool       append_send_data(const void *data, unsigned len);
        uint get_events();
	sint on_writable();
	sint on_readable();
	sint on_error();
private:
};
}}//namespace heron::tati


#endif //_HERON_ROUTINE_H_
