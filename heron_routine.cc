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
        log_event("~routine:create_time=%lld,fd=%d,writable=%d,"
                        "time_to_close=%lld,touch_time=%ld,",
                        "to_send_len=%u,to_proc_len=%u,",
                        "send_times=%llU,recv_times=%llU,"
                        "send_bytes=%llU,recv_bytes=%llU",
                        m_attr.m_create_time, m_fd, (int)m_writable,
                        m_time_to_close, 0,
                        m_send.data_len(), m_recv.data_len(),
                        m_stat.send_times, m_stat.recv_times,
                        m_stat.send_bytes, m_stat.recv_bytes);
}


heron_tcp_routine*    heron_tcp_routine::create(uint label, int fd)
{
        if(-1 == fcntl(fd, F_SETFL, O_NONBLOCK))
        {
                ::close(fd);
                return  nullptr;
        }

        return  new heron_tcp_routine();
}






int heron_tcp_routine::on_event(heron_event ev)
{

    if(heron_socket_readable == ev)
    {

    }
    else    if(heron_socket_read_hup)
    {
        /**
         * process epollrdhup event, which is offten triggered by
         * remote peer shut down writing half of connection and so on.
         */

        log_trace( "on_read_hup was triggered");
    }
    else    if(heron_socket_peer_hup)
    {
        /*epollhup event,which is offten triggered 
         * by crushing of remote peer and so on.*/
        log_trace( "on_peer_hup was triggered");
    }
}


uint        heron_tcp_routine::get_changed_events()
{
	uint        events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	if(heron_engine::get_instance()->get_state() == heron_engine::state_starting){
		if(!m_writable || m_send.data_len() > 0){
			return  (tcp_events & ~m_managed_events) | EPOLLOUT;
		}
        } else {
        }

        return  events;
}



int     heron_tcp_routine::on_writable()
{
	unsigned bytes_sent = 0;
        int     result = do_nonblock_write(m_send.data_ptr(), m_send.data_len(), bytes_sent);
        if(result >= 0)
        {
                m_send.consume(bytes_sent);
                log_trace("do_nonblock_write %u bytes,%u not sent",
                                bytes_sent, m_send.data_len());
        }

        return  result;

        m_writable = true;
        return  do_nonblock_write();
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
                        log_error( "on_error,%d occurred", errno);
                }
                else
                {
                        log_error( "on_error,%d occurred", errno);
                }
        }
        else if(ENETUNREACH == errno || ENETDOWN == errno || EADDRNOTAVAIL == errno)
        {
                /**
                 * EADDRNOTAVAIL:       The specified address is not available from the local machine.
                 * ENETUNREACH:         No route to the network is present.
                 * ENETDOWN:                    The local network interface used to reach the destination is down.
                 */
                log_error( "in event_process occurred,rpc_state=rpc_in_isolated");
        }
        else
        {
                log_error( "occurred,rpc_state=rpc_recv_failed");
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
                log_debug("heron_tcp_routine.check_conn_state, errno=%d", err);
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
                log_error( "heron_tcp_routine.check_conn_state, errno=%d", err);
                return  0;
        }
        else if(ENETUNREACH == err || ENETDOWN == err || EADDRNOTAVAIL == err)
        {
                /**
                 * EADDRNOTAVAIL:       The specified address is not available from the local machine.
                 * ENETUNREACH:         No route to the network is present.
                 * ENETDOWN:            The local network interface used to reach the destination is down.
                 */
                log_error( "heron_tcp_routine.check_conn_state, errno=%d", err);
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
                log_debug("heron_tcp_routine,EINTR");
        }
        else
        {
                log_error( "heron_tcp_routine.check_conn_state, errno=%d", err);
                return  0;
        }
	return  0;
}




sint    heron_tcp_routine::do_connect(ulong label, const char *ipaddr, ushort port)
{
        m_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(m_fd < 0 || -1 == fcntl(m_fd, F_SETFL, O_NONBLOCK))
        {
                log_error( "connect %d occurred", errno);
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

        log_trace( "active_session_routine.create,m_writable=%d",
                        (int)m_writable);
        return 0;
}

int     heron_tcp_routine::on_readable()
{
        int result = do_nonblock_recv(m_recv);
	//forward to main——thread
        return  0;
}

int     heron_routine::do_nonblock_write(const void *buf, unsigned len, unsigned &sent)
{
        while(sent < len)
        {
                int ret = write(m_fd, (const char *)buf + sent, len - sent);

                ++m_stat.send_times;
                if(ret >= 0)
                {
                        sent += ret;
                        m_stat.send_bytes += ret;
                        continue;
                }

                const int err = errno;
                if(err == EWOULDBLOCK || err == EAGAIN)
                {
                        m_writable = false;
                        return  0;
                }
                else if(err == EINTR)
                {
                        continue;
                }
                else
                {
                        return  -1;
                }
        }

        return  sent;
}

int     heron_routine::do_nonblock_recv(heron_buffer &cb)
{
        char            buff[64 * 1024];
        unsigned        max_recv = cb.unused_len();

        if(max_recv > sizeof(buff))
        {
                max_recv = sizeof(buff);
        }

        int len = read(m_fd, buff, max_recv);
        if(len > 0)
        {
                if(cb.append(buff, len))
                {
                        log_trace("do_nonblock_recv,%d bytes", len);
                }
                else
                {
                        log_error( "do_nonblock_recv,%d bytes save failed,buf.data_len=%u",
                                        len, cb.data_len());
                }
                m_stat.recv_times += 1;
                m_stat.recv_bytes += len;
                if(len == (int)max_recv)
                {
                        return  0;
                }
                else
                {
                        return  len;
                }
        }
        else if(len == 0)
        {
                log_event("do_nonblock_recv,peer closed");
                return  -1;
        }
        else if(errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
        {
                log_alert("nothing was read");
                return  1;
        }
        else
        {
                log_error( "read.errno=%d,strerror=%s",
                                errno, strerror(errno));
                return  -1;
        }
}

sint    heron_routine::append_send_data(const void *data, uint len)
{
        if(m_writable && m_send.data_len() == 0)
        {
                unsigned sent = 0;
                if(do_nonblock_write(data, len, sent) >= 0)
                {
                        log_trace( "append_send_data.do_nonblock_write %u bytes", sent);

                        if(sent < len)
                        {
                                m_writable = false;
                                return  m_send.append((const char *)data + sent, len - sent);
                        }
                        return  true;
                }
                else
                {
                        log_error( "append_send_data.do_nonblock_write failed");
                        return  false;
                }
        }

        if((m_send.append(data, len)))
        {
                log_trace( "append_send_data %u bytes success.", len);
                if(do_nonblock_write() >= 0)
                {
                        if(m_send.data_len() > 0) m_writable = false;
                        return  true;
                }
                else
                {
                        return  false;
                }
        }
        else
        {
                log_error( "append_send_data %u bytes failed!", len);
                return  false;
        }


        if(!m_writable)
        {
        }
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
                log_error( "set fd=%d linger errno=%d", fd, errno);
        }
        int nr = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
        if(nr != 0)
        {       
                log_error( "set fd=%d nodelay errno=%d", fd, errno);
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
                        log_error( "modify_events,errno=%d", errno);
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
                        log_trace("unregister_events done");
                }
                else
                {
                        log_error( "unregister_events,errno=%d,strerror(errno)=%s",
                                        errno, strerror(errno));
                }
        }
}
}}//namespace heron::tati
