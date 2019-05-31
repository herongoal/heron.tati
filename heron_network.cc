#include "heron_define.h"
#include "heron_logger.h"
#include "heron_engine.h"
#include "heron_network.h"
#include "heron_routine.h"
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


namespace   heron{namespace tati{
heron_listen_routine* heron_listen_routine::create(heron_engine *engine, sint fd)
{
        heron_listen_routine *sr = new heron_listen_routine(0, fd);
        if(nullptr == sr) 
        {   
                engine->log_fatal( "heron_listen_routine.create bad_alloc,close fd=%d", fd);
                ::close(fd);
        }   
	return  sr; 
}

heron_listen_routine::~heron_listen_routine()
{
}

sint	heron_listen_routine::on_events(heron_event events)
{
        if(events & heron_socket_readable){
		return	on_readable();
        }

	if (events &(~heron_socket_readable)){
		return	-1;
	}
}

heron_network_thread*   heron_network_thread::create(heron_engine *engine)
{
	heron_network_thread*   thread = new heron_network_thread();
	thread->m_network_channel = heron_synch_channel::create(engine, 1);
        if (nullptr == thread->m_network_channel){
                engine->log_fatal("");
                delete  thread;
                return  nullptr;
        }
        thread->m_control_channel = heron_synch_channel::create(engine, 1);
        if (nullptr == thread->m_control_channel){
                engine->log_fatal("");
		delete	thread;
                return  nullptr;
        }

	thread->m_epoll_fd = epoll_create(32768);
	if (thread->m_epoll_fd < 0){
                engine->log_fatal("create epoll error");
		exit(0);
		delete	thread;
                return  nullptr;
	}
        return  thread;
}

void    heron_network_thread::dispose_events(sint timeout_in_ms)
{
        signed  long            max_events = 32;
        struct  epoll_event     arr_events[max_events];

        if(max_events > m_pool.entity_num()){
		 max_events = m_pool.entity_num();
	}

        if(max_events > 0){
                int result = epoll_wait(m_epoll_fd, arr_events, max_events, timeout_in_ms);
                if(result < 0 && errno != EINTR)
                {
                        m_logger->log_event("epoll_wait,epoll_fd=%d,max_events=%u,%d occurred,errmsg=%s",
                                        m_epoll_fd, max_events, errno, strerror(errno));
                }

                for(int n = 0; n < result; ++n)
                {
                        struct  epoll_event *pe = arr_events + n;
			heron_event events = heron_socket_event_none;

			//heron_socket_read_hup = 1 << 2,
			//heron_socket_peer_hup = 1 << 3,
			//heron_socket_error = 1 << 4,

			if (pe->events & EPOLLIN){
				events |= heron_socket_readable;
			}
			if (pe->events & EPOLLOUT){
				events |= heron_socket_writable;
			}
                        process_events(pe->data.u64, events);
                }
	}
}


int64_t        heron_network_thread::gen_monotonic_ms()
{
        struct  timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return        ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int     heron_listen_routine::on_readable()
{
        while(true){
                socklen_t       len = sizeof(struct     sockaddr_in);
                struct  sockaddr_in     addr;

                int conn_fd = accept(m_fd, (struct sockaddr *)&addr, &len);
                if(conn_fd >= 0){
			heron_tcp_routine *rtn = heron_tcp_routine::create(0, conn_fd);
			
                        m_logger->log_event("new con");
                }
                else if(EAGAIN == errno){
                        break;
                }
		else{
                        m_logger->log_event( "on_readable.accept, %d occurred", errno);
                }
        }
        return  0;
}

void    heron_listen_routine::on_error()
{
        m_logger->log_event( "on_error was triggered");
        close_fd();
}


sint    heron_network_thread::send_message(ulong dest_routine_id, const void *data, uint len)
{
        heron_routine *rt = (heron_routine *)m_pool.search_element(dest_routine_id);

        if(nullptr == rt)
        {
                m_logger->log_event( "send_message to routine=%ld, not exist",
                                dest_routine_id);
                return        false;
        }

        if(!rt->append_send_data(data, len))
        {
                m_logger->log_event( "send_message to routine=%ld, failed",
                                dest_routine_id );
                return        false;
        }
        return        true;
}

void* heron_network_thread::start(void* arg)
{
	heron_network_thread *thread = static_cast<heron_network_thread *>(arg);
	sigset_t        sig_set;
	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGTERM);
	sigaddset(&sig_set, SIGQUIT);
	sigaddset(&sig_set, SIGINT);
	sigaddset(&sig_set, SIGHUP);
	sigaddset(&sig_set, SIGPIPE);
	sigaddset(&sig_set, SIGUSR1);
	sigaddset(&sig_set, SIGUSR2);
	sigaddset(&sig_set, SIGXFSZ);
	sigaddset(&sig_set, SIGTRAP);
	pthread_sigmask(SIG_BLOCK, &sig_set, nullptr);
	thread->run();
	return	nullptr;
}

void        heron_network_thread::inspect()
{
	ulong monotonic_ms = gen_monotonic_ms();

        for(uint n = 0; n < m_pool.entity_num(); ++n)
        {
                heron_routine *rt = (heron_routine *)m_pool.current_element();

                if(rt->m_last_inspect_time + 20 < monotonic_ms)
                {
                        if(rt->m_close_mark || rt->inspect() < 0)
                        {
                                m_pool.remove_element(rt->m_routine_id);
                                delete        rt;
                        }
                        else
                        {
                                rt->m_last_inspect_time = gen_monotonic_ms();
                        }
                }
                else
                {
                        break;
                }
                m_pool.forward_element();
        }
}

void    heron_network_thread::set_routine_timeout(ulong routine_id, int timeout_ms)
{
        heron_routine *rt = (heron_routine *)m_pool.search_element(routine_id);
        if(nullptr != rt)
        {
                rt->m_timeout = timeout_ms;
        }
}

sint    heron_network_thread::close_routine(ulong routine_id)
{
        heron_routine *rt = (heron_routine *)m_pool.remove_element(routine_id);

        if(nullptr != rt)
        {
                m_logger->log_event("close_routine routine_id=%lu", routine_id);
                //unregister_events(m_epoll_fd, rt);
                delete  rt;
        }
        else
        {
                m_logger->log_event( "close_routine routine_id=%ld not found",
                                routine_id);
        }
	return 0;
}

void    heron_network_thread::half_exit()
{
        for(uint n = 0; n < m_pool.entity_num(); ++n){
                heron_routine *rtn = static_cast<heron_routine *>(m_pool.current_element());
                if (rtn->get_type() == 0){
                        //network_thread closing mode(no listen)
                        //networker 退出条件
                }
                else if(rtn->get_type() == 2){
                        //unregister readable events
                        //process all left messages
                }
                else if(3 == 0){
                        //process thread broadcast closing events for each routine
                }
                else{
                        //所有的响应都发出去了,所有的查询都返回了
                }
        }

        //for passive sessions
        //process all left messages(已经读到就处理，已经决定发送就发送完)
        //什么时候推出？没有数据要发送

        //do not accept new channels
}

sint   heron_network_thread::register_routine(heron_routine *rt)
{
        bool ret = m_pool.insert_element(rt->m_routine_id, rt);
	if (ret){
		cout << "ret=1" << endl;
	}

        if(rt->get_changed_events() != 0)
        {
                const int events = rt->get_changed_events();
                struct  epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.u64 = rt->m_routine_id;

                if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, rt->m_fd, &ev) == 0)
                {
                        m_logger->log_event("done add_routine.epoll_ctl,events=%d, errmsg=%s",
                                        events, strerror(errno));
                }
                else
                {
                        m_logger->log_event( "add_routine.epoll_ctl,epoll_fd=%d,fd=%d,events=%d, errno=%d,msg=%s",
                                        m_epoll_fd,rt->m_fd,events, errno, strerror(errno));
                }
        }
        else
        {
                m_logger->log_event("add_routine no_events");
        }

	return	heron_result_state::success;
}

void    heron_network_thread::run()
{
        while(heron_engine::get_instance()->get_state() == heron_engine::state_running){
		dispose_events(5);
		process_timers();
		struct timespec timeout, remain;
		timeout.tv_sec = 0;
		timeout.tv_nsec = 1000 * 1000;
		nanosleep(&timeout, &remain);
        }
}

int     heron_tcp_routine::do_nonblock_recv(heron_buffer &cb)
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
                }
                else    
                {       
                        m_logger->log_error( "do_nonblock_recv,%d bytes save failed,buf.data_len=%u",
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
                m_logger->log_event("do_nonblock_recv,peer closed");
                return  -1;
        }
        else if(errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
        {
                m_logger->log_alert("nothing was read");
                return  1;
        }
        else
        {
                m_logger->log_error( "read.errno=%d,strerror=%s",
                                errno, strerror(errno));
                return  -1;
        }
}

sint    heron_tcp_routine::append_send_data(const void *data, uint len)
{
        if(m_writable && m_send->data_len() == 0)
        {
                unsigned sent = 0;
                if(do_nonblock_write(data, len, sent) >= 0)
                {
                        if(sent < len)
                        {
                                m_writable = false;
                                return  m_send->append((const char *)data + sent, len - sent);
                        }
                        return  true;
                }
                else
                {
                        m_logger->log_error( "append_send_data.do_nonblock_write failed");
                        return  false;
                }
        }

        if((m_send->append(data, len)))
        {
        }
        else
        {
                m_logger->log_error( "append_send_data %u bytes failed!", len);
                return  false;
        }


        if(!m_writable)
        {
        }
}

int     heron_tcp_routine::do_nonblock_write(const void *buf, unsigned len, unsigned &sent)
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

void    heron_network_thread::process_events(ulong routine_id, heron_event events)
{
        heron_routine *rt = (heron_routine *)m_pool.search_element(routine_id);
        if(nullptr == rt)
        {
                m_logger->log_event("process_events,routine_id=%lu,events=%u",
                                routine_id, events);
                return        ;
        }
	/*
	//delete writable event
                        struct  epoll_event ev;
                        ev.events = rt->get_managed_events() & (~EPOLLOUT);
                        ev.data.u64 = rt->m_routine_id;
                        if(epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, rt->m_fd, &ev) != 0)
                        {       
                                m_logger->log_event( "failed to depress writable=%u",
                                                ev.events);
                        }
	*/
	rt->on_events(events);
}
}}//namespace heron::tati
