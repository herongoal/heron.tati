#include "heron_factory.h"
#include "heron_process.h"
#include "heron_network.h"
#include "heron_worker.h"
#include "heron_engine.h"
#include <unistd.h>
#include <fcntl.h>


namespace heron{namespace tati{
heron_process_thread*   heron_factory::create_process_thread(heron_engine *engine, uint id){
        heron_process_thread*   thread = new heron_process_thread();

        thread->m_epoll_fd = epoll_create(32768);
        if (thread->m_epoll_fd < 0){
                engine->log_fatal("create epoll error");
                exit(0);
                delete  thread;
                return  nullptr;
        }
        return  thread;
}

heron_worker_thread*   heron_factory::create_worker_thread(heron_engine *engine, uint id){
        heron_worker_thread*   thread = new heron_worker_thread();

        thread->m_epoll_fd = epoll_create(32768);
        if (thread->m_epoll_fd < 0){
                engine->log_fatal("create epoll error");
                exit(0);
                delete  thread;
                return  nullptr;
        }
        return  thread;
}

heron_network_thread*   heron_factory::create_network_thread(heron_engine *engine, uint id){
        heron_network_thread*   thread = new heron_network_thread();

        thread->m_epoll_fd = epoll_create(32768);
        if (thread->m_epoll_fd < 0){
                engine->log_fatal("create epoll error");
                exit(0);
                delete  thread;
                return  nullptr;
        }
        return  thread;
}

heron_tcp_routine*    heron_factory::create_tcp_routine(uint label, int fd)
{
        if(-1 == fcntl(fd, F_SETFL, O_NONBLOCK))
        {
                ::close(fd);
                return  nullptr;
        }

        heron_tcp_routine *rtn = new heron_tcp_routine(label, fd);

        rtn->m_send = new heron_buffer(32 * 1024);
        rtn->m_recv = new heron_buffer(32 * 1024);

        if (nullptr == rtn->m_send || nullptr == rtn->m_recv){
                delete  rtn;
                return  nullptr;
        }

        return  rtn;
}

heron_listen_routine* heron_factory::create_listen_routine(heron_engine *engine, sint fd)
{
        heron_listen_routine *sr = new heron_listen_routine(0, fd);
        if(nullptr == sr)
        {
                engine->log_fatal( "heron_listen_routine.create bad_alloc,close fd=%d", fd);
                ::close(fd);
        }
        return  sr;
}
}}//namespace heron::tati
