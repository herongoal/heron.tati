#include "heron_thread.h"
#include "heron_engine.h"

#include <signal.h>
#include <unistd.h>


namespace heron{namespace tati{
void *heron_thread::start(void *arg)
{
	heron_thread *thread = static_cast<heron_thread *>(arg);
	if(thread == nullptr){
		thread->m_logger->log_fatal("bad params to start heron_work_thread");
		return nullptr;
	}

	struct sigaction        sa;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = nullptr;

        sigaction(SIGHUP, &sa, nullptr);
        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGQUIT, &sa, nullptr);
        sigaction(SIGPIPE, &sa, nullptr);
	return	thread->run();
}

void*   heron_thread::run()
{       
        while(heron_engine::get_instance()->get_state() == heron_engine::state_running){
		usleep(1*1000);
        }
        while(heron_engine::get_instance()->get_state() == heron_engine::state_exiting){
		usleep(1*1000);
        }
	return	nullptr;
}

sint   heron_thread::register_routine(heron_routine *rt)
{
        bool ret = m_pool.insert_element(rt->m_fd, rt);
        if (ret){
                cout << "ret=1" << endl;
        }else{
                cout << "fatal" << endl;
                exit(0);
        }

        if(rt->get_changed_events() != 0)
        {
                const int events = rt->get_changed_events();
                struct  epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.u64 = rt->m_fd;

                if(epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, rt->m_fd, &ev) == 0)
                {
                        m_logger->log_event("done add_routine.epoll_ctl,events=%d, errmsg=%s",
                                        events, strerror(errno));
                }
                else
                {
                        m_logger->log_event( "failed to add_routine.epoll_ctl,epoll_fd=%d,fd=%d,events=%d, errno=%d,msg=%s",
                                        m_epoll_fd,rt->m_fd,events, errno, strerror(errno));
                }
        }
        else
        {
                m_logger->log_event("add_routine no_events");
        }

        return  heron_result_state::success;
}
}}
