#include "heron_routine.h"
#include "heron_define.h"
#include "heron_engine.h"
#include "heron_logger.h"


#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>


namespace heron{namespace tati{
heron_tcp_routine::~heron_tcp_routine()
{
        m_logger->log_event("~routine:create_time=%lld,fd=%d,writable=%d,"
                        "time_to_close=%lld,touch_time=%ld,",
                        "to_send_len=%u,to_proc_len=%u,",
                        "send_times=%llU,recv_times=%llU,"
                        "send_bytes=%llU,recv_bytes=%llU",
                        m_attr.m_create_time, m_fd, (int)m_writable,
                        m_time_to_close, 0,
                        m_send->data_len(), m_recv->data_len(),
                        m_stat.send_times, m_stat.recv_times,
                        m_stat.send_bytes, m_stat.recv_bytes);
}


int heron_tcp_routine::on_events(heron_event events)
{
	cout << "tcp routine on events" << endl;
    if((heron_socket_readable & events)!=0)
    {
	return	on_readable();
    }

    if((heron_socket_read_hup&events)!=0)
    {
        /**
         * process epollrdhup event, which is offten triggered by
         * remote peer shut down writing half of connection and so on.
         */

        m_logger->log_event( "on_read_hup was triggered");
    }

    if(heron_socket_peer_hup)
    {
        /*epollhup event,which is offten triggered 
         * by crushing of remote peer and so on.*/
        m_logger->log_event( "on_peer_hup was triggered");
    }
    return	heron_result_state::success;
}


uint        heron_tcp_routine::get_changed_events()
{
	uint        events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP;
	if(heron_engine::get_instance()->get_state() == heron_engine::state_starting){
		if(!m_writable || m_send->data_len() > 0){
			return  (tcp_events & ~m_managed_events) | EPOLLOUT;
		}
        } else {
        }

        return  events;
}



int     heron_tcp_routine::on_writable()
{
	unsigned bytes_sent = 0;
        int     result = do_nonblock_write(m_send->data_ptr(), m_send->data_len(), bytes_sent);
        if(result >= 0)
        {
                m_send->consume(bytes_sent);
                m_logger->log_event("do_nonblock_write %u bytes,%u not sent",
                                bytes_sent, m_send->data_len());
        }

        return  result;

        m_writable = true;
	return	0;
}

sint    heron_tcp_routine::on_error()
{
        if(!m_writable)
        {
                /**
                 *      ECONNRESET:             Remote host reset the connection request.
                 *
                 *      EHOSTUNREACH:   The destination host cannot be reached (probably 
                 *      because the host is down or a remote router cannot reach it).
                 *
                 *      ECONNREFUSED:   The target address was not listening for
                 *      connections or refused the connection request.
                 */

                if(ECONNRESET == errno || EHOSTUNREACH == errno || ECONNREFUSED == errno)
                {
                        m_logger->log_error( "on_error,%d occurred", errno);
                }
                else
                {
                        m_logger->log_error( "on_error,%d occurred", errno);
                }
        }
        else if(ENETUNREACH == errno || ENETDOWN == errno || EADDRNOTAVAIL == errno)
        {
                /**
                 * EADDRNOTAVAIL:       The specified address is not available from the local machine.
                 * ENETUNREACH:         No route to the network is present.
                 * ENETDOWN:                    The local network interface used to reach the destination is down.
                 */
                m_logger->log_error( "in event_process occurred,rpc_state=rpc_in_isolated");
        }
        else
        {
                m_logger->log_error( "occurred,rpc_state=rpc_recv_failed");
        }
        return errno;
}

sint    heron_tcp_routine::check_conn_state(int err)
{
        /**
         * Although POSIX 
         */
        if(EISCONN == err || EALREADY == err || EINPROGRESS == err)
        {
                /**
                 *      EALREADY:       A connection request is already in progress for the specified socket.
                 *      EISCONN:        The specified socket is connection-mode and is already connected.
                 */
                //log_debug("heron_tcp_routine.check_conn_state, errno=%d", err);
        }
        else if(ECONNRESET == err || EHOSTUNREACH == err || ECONNREFUSED == err)
        {
                /**
                 *      ECONNRESET:             Remote host reset the connection request.
                 *
                 *      EHOSTUNREACH:   The destination host cannot be reached (probably 
                 *      because the host is down or a remote router cannot reach it).
                 *
                 *      ECONNREFUSED:   The target address was not listening for
                 *      connections or refused the connection request.
                 */
                m_logger->log_error( "heron_tcp_routine.check_conn_state, errno=%d", err);
                return  0;
        }
        else if(ENETUNREACH == err || ENETDOWN == err || EADDRNOTAVAIL == err)
        {
                /**
                 * EADDRNOTAVAIL:       The specified address is not available from the local machine.
                 * ENETUNREACH:         No route to the network is present.
                 * ENETDOWN:            The local network interface used to reach the destination is down.
                 */
                m_logger->log_error( "heron_tcp_routine.check_conn_state, errno=%d", err);
                return  0;
        }
        else if(EINTR == errno)
        {
                /**
                 * The attempt to establish a connection was interrupted by delivery of a 
                 * signal that was caught; the connection shall be established asynchronously.
                 * refer to http://www.madore.org/~david/computers/connect-intr.html
                 * for some useful information about the EINTR after connect
                 */
                //log_debug("heron_tcp_routine,EINTR");
        }
        else
        {
                m_logger->log_error( "heron_tcp_routine.check_conn_state, errno=%d", err);
                return  0;
        }
	return  0;
}




sint    heron_tcp_routine::do_connect(ulong label, const char *ipaddr, ushort port)
{
        m_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(m_fd < 0 || -1 == fcntl(m_fd, F_SETFL, O_NONBLOCK))
        {
                m_logger->log_error( "connect %d occurred", errno);
                return  errno;
        }

        bzero(&m_peer_addr, sizeof(m_peer_addr));
        m_peer_addr.sin_family = AF_INET;
        m_peer_addr.sin_port = htons(port);
        inet_aton(ipaddr, &m_peer_addr.sin_addr);

        m_writable = false;
        /**
         * circumstance that connection is established immediately after calling connect non-blockly
         * especially when client and server are on the same machine do exist.
         */
        if(connect(m_fd, (struct sockaddr *)&m_peer_addr, sizeof(m_peer_addr)) == 0)
        {
                m_writable = true;
        }

        m_logger->log_event( "active_session_routine.create,m_writable=%d",
                        (int)m_writable);
        return 0;
}

int     heron_tcp_routine::on_readable()
{
        sint result = do_nonblock_recv(*m_recv);
	cout << "tcp routine:readable result=" << result << endl;
        return  result;
}

void    set_flags(int fd)
{       
        struct linger so_linger;
        so_linger.l_onoff = 1;
        so_linger.l_linger = 0;
        int enable = 1;
        int lr = setsockopt(fd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
        if(lr != 0)
        {       
        }
        int nr = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
        if(nr != 0)
        {       
        }
}

void    modify_events(sint epoll_fd, heron_routine *rt)
{
        struct  epoll_event ev;
        ev.data.u64 = rt->m_routine_id;
        ev.events = rt->get_changed_events();

        if(ev.events != 0 && rt->m_fd >= 0)
        {
                int ret = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, rt->m_fd, &ev);
                if(0 != ret)
                {
                }
        }
}

void    unregister_events(sint epoll_fd, heron_routine *rt)
{
        struct  epoll_event ev;

        if((ev.events = rt->get_changed_events()) != 0 && rt->m_fd >= 0)
        {
                int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, rt->m_fd, &ev);
                if(0 == ret)
                {
                }
                else
                {
                }
        }
}
}}//namespace heron::tati
