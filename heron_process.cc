#include "heron_process.h"
#include "heron_logger.h"


namespace   heron{namespace tati{
heron_process_thread::heron_process_thread()
{
}

sint	heron_process_thread::init()
{
        return  0;
}

sint    heron_process_thread::react()
{
        process_timers();

        {
                struct timespec timeout, remain;
                timeout.tv_sec = 0;
                timeout.tv_nsec = 1000 * 1000;
                nanosleep(&timeout, &remain);
        }
}

sint    heron_process_thread::process_timers()
{       
}
}}//namespace heron::tati
