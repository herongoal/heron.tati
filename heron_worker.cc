#include "heron_worker.h"

#include <signal.h>


namespace heron{namespace tati{
void *heron_heavy_work_thread::start(void *arg)
{
	struct sigaction        sa_interupted;

        sa_interupted.sa_flags = SA_SIGINFO;
        sigemptyset(&sa_interupted.sa_mask);
        sa_interupted.sa_sigaction = nullptr;

        sigaction(SIGHUP, &sa_interupted, nullptr);
        sigaction(SIGINT, &sa_interupted, nullptr);
        sigaction(SIGQUIT, &sa_interupted, nullptr);
        sigaction(SIGPIPE, &sa_interupted, nullptr);
}
}}
