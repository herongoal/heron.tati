#include "heron_process.h"
#include "heron_engine.h"
#include "heron_logger.h"


namespace   heron{namespace tati{
sint    heron_process_thread::react()
{
        const sint result = process_timers();
	if(result != heron_result_state::success){
		return	result;
	}

	{
                struct timespec timeout, remain;
                timeout.tv_sec = 0;
                timeout.tv_nsec = 1000 * 1000;
                nanosleep(&timeout, &remain);
        }
	return	heron_result_state::success;
}

void* heron_process_thread::start(void* arg)
{
	heron_process_thread *thread = static_cast<heron_process_thread*>(arg);
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
	return	thread->run();
}

sint    heron_process_thread::process_timers()
{       
	return	heron_result_state::success;
}

void    heron_process_thread::half_exit()
{
	for(uint n = 0; n < m_pool.entity_num(); ++n){
		heron_routine *rtn = static_cast<heron_routine *>(m_pool.current_element());
		if (rtn->get_type() == 0){
			//network_thread closing mode(no listen)
			//networker 退出条件
		}
		else if(rtn->get_type() == 1){
			//unregister readable events
			//process all left messages
		}
		else if(rtn->get_type() == 2){
			//process thread broadcast closing events for each routine
		}
		else {
			//process thread broadcast closing events for each routine
		}
	}

	//for passive sessions
	//process all left messages(已经读到就处理，已经决定发送就发送完)
	//什么时候推出？没有数据要发送

	//do not accept new channels
}

void*   heron_process_thread::run()
{
	while(heron_engine::get_instance()->get_state() == heron_engine::state_running){
		react();
	}
	while(heron_engine::get_instance()->get_state() == heron_engine::state_exiting){
		react();
	}
	return	nullptr;
}
}}//namespace heron::tati
