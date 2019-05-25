#include "heron_worker.h"
#include "heron_engine.h"

#include <signal.h>


namespace heron{namespace tati{
void *heron_worker_thread::start(void *arg)
{
	heron_worker_thread *thread = static_cast<heron_worker_thread *>(arg);
	if(thread == nullptr){
		log_fatal("bad params to start heron_work_thread");
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

void*   heron_worker_thread::run()
{       
        while(heron_engine::get_instance()->get_state() == heron_engine::state_running){
        }
        while(heron_engine::get_instance()->get_state() == heron_engine::state_exiting){
        }
	return	nullptr;
}
}}
