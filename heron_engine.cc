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

heron_engine*   heron_engine::create(const string &log_file, log_level level, uint slice_kb, uint proxy_num, uint worker_num)
{
	if(heron_engine::get_instance()!=nullptr){
		return	nullptr;
	}

	m_engine_instance = new heron_engine();
	m_engine_instance->m_proxy_num = proxy_num;
	m_engine_instance->m_worker_num = worker_num;

	if(proxy_num > sizeof(m_engine_instance->m_network_threads)/sizeof(m_engine_instance->m_network_threads[0])){
		return	nullptr;
	}
	if(worker_num > sizeof(m_engine_instance->m_worker_threads)/sizeof(m_engine_instance->m_worker_threads[0])){
		return	nullptr;
	}

	m_engine_instance->m_process_thread = new heron_process_thread();
        for(int n = proxy_num; n > 0; --n)
        {
                m_engine_instance->m_network_threads[n] = new heron_network_thread();
        }

        for(int n = worker_num; n > 0; --n)
        {
                m_engine_instance->m_worker_threads[n-1] = new heron_worker_thread();
        }

	for(int n = 0; n < proxy_num + worker_num; ++n){
		m_engine_instance->m_synch_channels[n] = heron_synch_channel::create();
		m_engine_instance->m_log_channels[n] = heron_log_channel::create();
	}

	return	m_engine_instance
}

int     heron_engine::init()
{
	//passive
        //start logger
        //start listen
        //start active workers
        //register channels

        m_process_thread->init();
        int ret = pthread_create(&m_process_thread->m_thread, nullptr, m_process_thread->start, m_process_thread);
        if (ret != 0){
                log_fatal("failed to create process thread");
        }
        log_event("create process thread finished");
        return  heron_result_state::success;


	//create threads

        return  0;
}

void	heron_engine::stop_threads()
{
	//phase stop independence jobs
	//timers
	//parent id=0(udp passive sessions(stop reading),passive tcp sessions（stop reading）, listening,parent =0 periodic timers) stop
	//if no parent, then close(then set all it's created parent=0)
	
	//shuangxiang say good by
	
	//files
	//active sessions: ref count() after ref count=0, then close; after sending via a channel, increase 1, recving resp, decrease 1
	
	/*
	 * stop network thread(set no accepting, no reading request for passive conns, close after all data sent)
	 *
	 **long connections
	 * periodic tasks close
	 * periodic timers stop
	 * timers close
	 * heron_process stop reading
	 * heron_process stop register timers
	 * heron_process stop creating channels
	 * heron_process stop active sending
	 * pthread_join heron_process
	 *
	 * phase 2 stop dependented 
	 *
	 *
	 *
	 *
	 ///////////////////////when to start the phase 2
	 * worker thread set flag(exit after empty)
	 * stop network thread()
	 * stop utilssssssss
	 *
	 *
	 *
	 ///////////////////////when to start the phase 3
	 //resource clear
	 *
	 *
	 */

	//stop reading from connections
	//process all the requests
	//stop writing
	//never accept more active querying
	//call before_routine_closed()
	//write last log
	//stop logger
	//jieguan logger
	//exit
	//stop channels
}

sint    heron_engine::start_worker_threads()
{
	for(size_t idx=sizeof(m_worker_threads)/sizeof(m_worker_threads[0]);
		idx>0; ++idx)
        {
                heron_worker_thread *thread=m_worker_threads[idx];
                sint ret = thread->init();
		if (ret != heron_result_state::success){
			return ret;
		}
                ret = pthread_create(&thread->m_thread, nullptr, thread->start, &thread);
                if (ret != heron_result_state::success){
                        log_fatal("failed to create heavy work thread");
			return ret;
                }
                log_event("create heavy work thread finished");
        }
	return	heron_result_state::success;
}

sint    heron_engine::start_network_threads()
{
	for(size_t idx=sizeof(m_network_threads)/sizeof(m_network_threads[0]);
		idx>0; ++idx)
        {
                heron_network_thread *thread=m_network_threads[idx];
                sint ret = thread->init();
		if (ret != heron_result_state::success){
			return ret;
		}
                ret = pthread_create(&thread->m_thread, nullptr, thread->start, &thread);
                if (ret != heron_result_state::success){
                        log_fatal("failed to create network thread");
			return ret;
                }
                log_event("create network thread finished");
        }
	return	heron_result_state::success;
}

sint    heron_engine::start_threads()
{
	//passive
	//start logger
	start_worker_threads();
	start_process_threads();
	start_network_threads();
	//start listen
	//start active workers

	m_process_thread->init();
	int ret = pthread_create(&m_process_thread->m_thread, nullptr, m_process_thread->start, &m_process_thread);
	if (ret != 0){
		log_fatal("failed to create process thread");
	}
	log_event("create process thread finished");
	return	heron_result_state::success;
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
