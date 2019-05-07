#include "heron_engine.h"


#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>


using namespace std;
using ::sigaction;

namespace heron{namespace tati{
void    signal_handle(int sigid, siginfo_t *si, void *unused)
{
        if(SIGTERM == sigid || SIGINT == sigid)
        {
        }
        else if(SIGPIPE == sigid || SIGHUP == sigid)
        {
        }
        else if(SIGUSR1 == sigid)
        {
                //heron_engine::get_instance()->reload_config();
        }
        else if(SIGUSR2 == sigid)
        {
                //heron_engine::get_instance()->gen_report();
        }
}

ulong    heron_engine::create_listen_routine(ulong label, const char *ipaddr, uint16_t port)
{
        tcp_listen_routine *rtn = tcp_listen_routine::create(ipaddr,port);

        rtn->add_routine(rtn);
        log_event( "create_service_routine,origin=%d,routine_id=%lu,"
                        "ipaddr=%s,port=%u",
                        rtn->m_routine_id, ipaddr, port);

        return  rtn->m_routine_id;
}

heron_engine::heron_engine(const string &log_file, log_level level, uint slice_kb)
{
        if(socketpair(AF_UNIX, SOCK_DGRAM, 0, m_log_fds) < 0)
        {
                log_fatal("init.socketpair error,errno=%d,errmsg=%s",
                                errno, strerror(errno));
        }

                dup2(m_log_fds[0], STDOUT_FILENO);
                dup2(m_log_fds[0], STDERR_FILENO);

        if(-1 == fcntl(m_log_fds[0], F_SETFL, O_NONBLOCK)
                        || -1 == fcntl(m_log_fds[1], F_SETFL, O_NONBLOCK))
        {
                log_fatal("set nonblock failed,errno=%d,errmsg=%s",
                                errno, strerror(errno));
        }

        const socklen_t buf_len = 4 * 1024 * 1024;
        setsockopt(m_log_fds[0], SOL_SOCKET, SO_SNDBUF, &buf_len, sizeof(buf_len));
        setsockopt(m_log_fds[0], SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(buf_len));
        setsockopt(m_log_fds[1], SOL_SOCKET, SO_SNDBUF, &buf_len, sizeof(buf_len));
        setsockopt(m_log_fds[1], SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(buf_len));
}

void func()
{
        //pthread_t thread_id = pthread_self();
	// check if called by master
                {
                //                void    *exit_ret = nullptr;
                 //               pthread_join(rtp->m_threadid, &exit_ret);
                }
}

int     heron_engine::init()
{
        if(socketpair(AF_UNIX, SOCK_DGRAM, 0, m_log_fds) < 0)
        {
                log_event("init.socketpair error,errno=%d,errmsg=%s",
                                errno, strerror(errno));
                return  -1;
        }

        dup2(m_log_fds[0], STDOUT_FILENO);
        dup2(m_log_fds[0], STDERR_FILENO);

        if(-1 == fcntl(m_log_fds[0], F_SETFL, O_NONBLOCK)
                        || -1 == fcntl(m_log_fds[1], F_SETFL, O_NONBLOCK))
        {
                log_event("set nonblock failed,errno=%d,errmsg=%s",
                                errno, strerror(errno));
                return  -1;
        }

        return  0;
}

void	heron_engine::stop_threads()
{
	while(true){
		join_threads
	}

	//stop all new connections
	for(size_t n = 0; n < sizeof(m_network_threads)/sizeof(m_network_threads); ++n)
	{
		//stop new connection creation
	}

	//stop reading from connections
	//process all the requests
	//stop writing
	//never accept more active querying
	//call before_routine_closed()
	//write last log
	//stop logger
	//jieguan logger
	//exit
}

sint    heron_engine::start_heavy_work_threads()
{
	for(size_t idx=sizeof(m_heavy_work_threads)/sizeof(m_heavy_work_threads[0]);
		idx>0; ++idx)
        {
                heron_heavy_work_thread &thread=m_heavy_work_threads[idx];
                thread.init();
                int ret = pthread_create(&thread.m_thread, nullptr, thread.start, &thread);
                if (ret != 0){
                        log_fatal("failed to create heavy work thread");
                }
                log_event("create heavy work thread finished");
        }
}

sint    heron_engine::start_network_threads()
{
	for(size_t idx=sizeof(m_network_threads)/sizeof(m_network_threads[0]);
		idx>0; ++idx)
        {
                heron_network_thread &thread=m_network_threads[idx];
                thread.init();
                int ret = pthread_create(&thread.m_thread, nullptr, thread.start, &thread);
                if (ret != 0){
                        log_fatal("failed to create network thread");
                }
                log_event("create network thread finished");
        }
}
sint    heron_engine::start_threads()
{
	//passive
	//start logger
	//start heavy work thread, all the heavy work will be processed here.
	//start process thread, //all the logic process will be procesed here.
	//start network thread, //all the network process will be processed here.
	//start listen
	//start active workers

	m_process_thread.init();
	int ret = pthread_create(&m_process_thread.m_thread, nullptr, m_process_thread.start, &m_process_thread);
	if (ret != 0){
		log_fatal("failed to create process thread");
	}
	log_event("create process thread finished");
}

void    mask_signals()
{
        struct sigaction        sa_interupted;
        
        sa_interupted.sa_flags = SA_SIGINFO;
        sigemptyset(&sa_interupted.sa_mask);
        sa_interupted.sa_sigaction = signal_handle;
        
        sigaction(SIGHUP, &sa_interupted, nullptr);
        sigaction(SIGINT, &sa_interupted, nullptr);
        sigaction(SIGQUIT, &sa_interupted, nullptr);
        sigaction(SIGPIPE, &sa_interupted, nullptr);
        
}
}}//namespace heron::tati
