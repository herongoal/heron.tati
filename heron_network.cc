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
tcp_listen_routine* tcp_listen_routine::create(heron_engine *engine, sint fd)
{
        tcp_listen_routine *sr = new tcp_listen_routine(0, fd);
        if(nullptr == sr) 
        {   
                engine->log_fatal( "tcp_listen_routine.create bad_alloc,close fd=%d", fd);
                ::close(fd);
        }   
        return  sr; 
}

tcp_listen_routine::~tcp_listen_routine()
{
}

void    heron_network_thread::react()
{
        inspect();
        process_timers();

	struct timespec timeout, remain;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 1000 * 1000;
	nanosleep(&timeout, &remain);

        struct  epoll_event     arr_events[512];
        uint    fetch_num = sizeof(arr_events) / sizeof(arr_events[0]);

        if(m_pool.entity_num() < fetch_num) fetch_num = m_pool.entity_num();
        if(fetch_num > 0)
        {
                int result = epoll_wait(m_epoll_fd, arr_events, fetch_num, 0);
                if(result < 0 && errno != EINTR)
                {
                        m_log_writer->log_event("process_events.epoll_wait,epoll_fd=%d,fetch_num=%u,%d occurred,errmsg=%s",
                                        m_epoll_fd, fetch_num,errno, strerror(errno));
                }

                for(int n = 0; n < result; ++n)
                {
                        struct  epoll_event *pe = arr_events + n;
                        process_events(pe->data.u64, pe->events);
                }
	}
}


int64_t        heron_network_thread::gen_monotonic_ms()
{
        struct  timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return        ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int     tcp_listen_routine::on_readable()
{
        while(true){
                socklen_t       len = sizeof(struct     sockaddr_in);
                struct  sockaddr_in     addr;

                int conn_fd = accept(m_fd, (struct sockaddr *)&addr, &len);
                if(conn_fd >= 0){
                }
                else if(EAGAIN == errno){
                        break;
                }
		else{
                        m_log_writer->log_event( "on_readable.accept, %d occurred", errno);
                }
        }
        return  0;
}

void    tcp_listen_routine::on_error()
{
        m_log_writer->log_event( "on_error was triggered");
        close_fd();
}


sint    heron_network_thread::send_message(ulong dest_routine_id, const void *data, uint len)
{
        heron_routine *rt = (heron_routine *)m_pool.search_element(dest_routine_id);

        if(nullptr == rt)
        {
                m_log_writer->log_event( "send_message to routine=%ld, not exist",
                                dest_routine_id);
                return        false;
        }

        if(!rt->append_send_data(data, len))
        {
                m_log_writer->log_event( "send_message to routine=%ld, failed",
                                dest_routine_id );
                return        false;
        }
        return        true;
}

void* heron_network_thread::start(void* arg)
{
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

        return  (void *)0;
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
                m_log_writer->log_event("close_routine routine_id=%lu", routine_id);
                //unregister_events(m_epoll_fd, rt);
                delete  rt;
        }
        else
        {
                m_log_writer->log_event( "close_routine routine_id=%ld not found",
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

sint   heron_network_thread::add_routine(heron_routine *rt)
{
        m_pool.insert_element(rt->m_routine_id, rt);

        if(rt->get_changed_events() != 0)
        {
                const int events = rt->get_changed_events();
                struct  epoll_event ev;
                ev.events = events;
                ev.data.u64 = rt->m_routine_id;

                if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, rt->m_fd, &ev) == 0)
                {
                        m_log_writer->log_event("add_routine.epoll_ctl,events=%d",
                                        events);
                }
                else
                {
                        m_log_writer->log_event( "add_routine.epoll_ctl,events=%d, errno=%d",
                                        events, errno);
                }
        }
        else
        {
                m_log_writer->log_event("add_routine no_events");
        }

        if(rt->m_proxy_id == m_proxy_id && rt->m_recv->data_len() > 0)
        {
        }
	return	heron_result_state::success;
}

void    heron_network_thread::run()
{
        while(heron_engine::get_instance()->get_state() == heron_engine::state_running){
        }
        while(heron_engine::get_instance()->get_state() == heron_engine::state_exiting){
        }
}

void    heron_network_thread::process_events(ulong routine_id, const unsigned events)
{
        heron_routine *rt = (heron_routine *)m_pool.search_element(routine_id);
        if(nullptr == rt)
        {
                m_log_writer->log_event("process_events,routine_id=%lu,events=%u",
                                routine_id, events);
                return        ;
        }

        if(events & EPOLLOUT)
        {
                rt->m_writable = true;
                if(rt->m_writable)
                {
                        //delete writable event
                        struct  epoll_event ev;
                        ev.events = rt->get_managed_events() & (~EPOLLOUT);
                        ev.data.u64 = rt->m_routine_id;
                        if(epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, rt->m_fd, &ev) != 0)
                        {
                                m_log_writer->log_event( "failed to depress writable=%u",
                                                ev.events);
                        }
                }
        }
        if(events & EPOLLIN)
        {
        }

        if(events & EPOLLERR)
        {
                rt->m_close_mark = true;
        }

        if(events & EPOLLRDHUP)
        {
                //this is not even an error
        }

        if(events & EPOLLHUP)
        {
                rt->m_close_mark = true;
        }
}
}}//namespace heron::tati
