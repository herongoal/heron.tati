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
#include <netinet/in.h>
#include <arpa/inet.h>


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

heron_engine*	heron_engine::m_instance = nullptr;
ulong    heron_engine::register_listen_port(ulong label, const char *ipaddr, uint16_t port)
{
	int fd = 0;
        heron_listen_routine *rtn = heron_listen_routine::create(this, fd);

        //rtn->add_routine(rtn);
        log_vital( "create_service_routine,origin=%d,routine_id=%lu,"
                        "ipaddr=%s,port=%u",
                        rtn->m_routine_id, ipaddr, port);

        return  rtn->m_routine_id;
}

heron_engine*   heron_engine::create(const string &log_file, log_level level, uint slice_kb, uint proxy_num, uint worker_num)
{
	if(heron_engine::get_instance()!=nullptr){
		m_instance->log_fatal("one only");
		return	nullptr;
	}

	if(proxy_num > sizeof(m_instance->m_proxies)/sizeof(m_instance->m_proxies[0])){
		m_instance->log_fatal("proxy num");
		return	nullptr;
	}

	if(worker_num > sizeof(m_instance->m_workers)/sizeof(m_instance->m_workers[0])){
		m_instance->log_fatal("proxy num");
		return	nullptr;
	}

	m_instance = new heron_engine();
	m_instance->m_worker_num = worker_num;
	m_instance->m_proxy_num = proxy_num;

	bzero(m_instance->m_proxies, sizeof(m_instance->m_proxies));
	bzero(m_instance->m_workers, sizeof(m_instance->m_workers));
	bzero(m_instance->m_listen_addrs, sizeof(m_instance->m_listen_addrs));

	m_instance->m_process_thread = new heron_process_thread();
	if (nullptr == m_instance->m_process_thread){
		m_instance->log_fatal("failed to create worker thread");
		delete	m_instance;
		m_instance = nullptr;
		return	nullptr;
	}

        for(uint n = 0; n < m_instance->m_proxy_num; ++n){
                m_instance->m_proxies[n] = heron_network_thread::create(m_instance);
		if (nullptr == m_instance->m_proxies[n]){
			m_instance->log_fatal("failed to create network thread");
			delete	m_instance;
			m_instance = nullptr;
			return	nullptr;
		}
		m_instance->log_fatal("create network thread=%d", n);
        }

        for(uint n = 0; n < m_instance->m_worker_num; ++n){
                m_instance->m_workers[n] = new heron_worker_thread();
		if (nullptr == m_instance->m_workers[n]){
			m_instance->log_fatal("");
			delete	m_instance;
			m_instance = nullptr;
			return	nullptr;
		}
        }

	return	m_instance;
}

int     heron_engine::init()
{
	//passive
        //start logger
        //start listen
        //start active workers
        //register channels

        sint result = m_process_thread->init();
	if (heron_result_state::success!=result){
		log_fatal("init process thread result=%d", result);
		return  result;
	}
	log_vital("init process thread success");
        int ret = pthread_create(&m_process_thread->m_thread, nullptr, m_process_thread->start, m_process_thread);
        if (ret != heron_result_state::success){
                m_instance->log_fatal("failed to create process thread");
		return	result;
        }
        log_vital("create process thread finished");
        return  heron_result_state::success;
}

void	heron_engine::stop_threads()
{
	//没有消息待处理
	//没有数据要发送
	//
	//phase stop independence jobs
	//timers
	//if no parent, then close(then set all it's created parent=0)
	
	//shuangxiang say good by
	
	//files
	//active sessions: ref count() after ref count=0, then close; after sending via a channel, increase 1, recving resp, decrease 1
	
	/*
	 **long connections
	 * timers close
	 * heron_process stop active sending
	 * pthread_join heron_process
	 * phase 2 stop dependented 
	 *
	 *
	 *
	 *
	 * worker thread set flag(exit after empty)
	 * stop network thread()
	 * stop utilssssssss
	 */
	//call before_routine_closed()
}

void    heron_engine::clear_resource()
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
         */
        //stop writing
        //write last log
        //stop logger
        //jieguan logger
        //exit
        //stop channels
}

sint    heron_engine::start_worker_threads()
{
	for(uint n = 0; n < m_worker_num; ++n){
                heron_worker_thread *thread=m_workers[n];
                sint ret = pthread_create(&thread->m_thread, nullptr, thread->start, thread);
                if (ret != heron_result_state::success){
			log_fatal("create worker-thread=%d failed", thread->m_proxy_id);
			return ret;
                }
                log_vital("create worker-thread=%d finished", thread->m_proxy_id);
		cout << "start worker done" << endl;
        }
	return	heron_result_state::success;
}

sint    heron_engine::start_process_thread()
{
	sint ret = m_process_thread->init();
	if (ret != heron_result_state::success){
		return ret;
	}
	ret = pthread_create(&m_process_thread->m_thread, nullptr, m_process_thread->start, m_process_thread);
	if (ret != heron_result_state::success){
		m_instance->log_fatal("failed to create process thread");
		return ret;
	}
	log_vital("create process thread finished");
	return  heron_result_state::success;
}

sint    heron_engine::start_network_threads()
{
	for(uint n = 0; n < m_proxy_num; ++n){
		heron_network_thread *thread=m_proxies[n];
                sint ret = pthread_create(&thread->m_thread, nullptr, thread->start, thread);
                if (ret != heron_result_state::success){
                        m_instance->log_fatal("failed to create network thread");
			return ret;
                }
                log_vital("create network thread finished");
        }
	return	heron_result_state::success;
}

sint    heron_engine::start_service(){
	m_state = state_running;
	sint	result = start_threads();
	if (heron_result_state::success!=result){
		log_vital("start threads result=%d", result);
		return	result;
	}
	log_vital("start threads done.");

	result = run();

	if (heron_result_state::success!=result){
		log_fatal("exit abnormally");
		return	result;
	}

	return	0;
}

sint    heron_engine::run()
{
	while(m_state == state_running){
		cout << "clock" << endl;
		sleep(1);
		//read logs
		//check states
	}
	while(m_state == state_running){
		//read logs
		//check states
		//check if state has channged
	}
	return	heron_result_state::success;
}

sint    heron_engine::start_threads()
{
	//passive
	//start logger
	start_worker_threads();
	start_process_thread();
	start_network_threads();
	//start listen
	//start active workers

	m_process_thread->init();
	int ret = pthread_create(&m_process_thread->m_thread, nullptr, m_process_thread->start, &m_process_thread);
	if (ret != 0){
		m_instance->log_fatal("failed to create process thread");
	}
	log_vital("create process thread finished");
	return	heron_result_state::success;
}

sint    heron_engine::listen_at_port(const char *ipaddr, ushort port)
{
        struct sockaddr_in  addr = {AF_INET, htons(port), {0u},
                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

        if(0 == inet_aton(ipaddr, &addr.sin_addr)){
                log_fatal( "heron_listen_routine.inet_aton error,ipaddr=%s",
                                errno, strerror(errno));
                return  -1;
        }

        int     fd = socket(AF_INET, SOCK_STREAM, 0);
        if(fd < 0){
                log_fatal( "heron_listen_routine.socket errno=%d,errmsg=%s",
                                errno, strerror(errno));
                return  -1;
        }

        if(-1 == fcntl(fd, F_SETFL, O_NONBLOCK)){
                log_fatal( "heron_listen_routine.fcntl errno=%d,errmsg=%s",
                                errno, strerror(errno));
                ::close(fd);
                return  -1;
        }

        const sint reuse_port = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(reuse_port));
        if(0 != bind(fd, (struct sockaddr *)&addr, sizeof(addr))){
                log_fatal( "heron_listen_routine.bind errno=%d,errmsg=%s",
                                errno, strerror(errno));
                ::close(fd);
                return  -1;
        }

        if(0 != listen(fd, 2000))
        {
                log_fatal( "heron_listen_routine.listen errno=%d,errmsg=%s",
                                errno, strerror(errno));
                ::close(fd);
                return  -1;
        }

	log_fatal("%d listen",fd);

	for(uint n = 0; n < m_proxy_num; ++n){
		heron_listen_routine *rtn = heron_listen_routine::create(this, fd);
		if (nullptr == rtn){
			log_fatal("failed to create listen routine: fd=%d", fd);
			return	-1;
		}
		log_vital("Create-Listen-Routine: fd=%d", fd);
		sint result = m_proxies[n]->register_routine(rtn);
		if (heron_result_state::success!=result){
			log_fatal("failed to register listen routine to proxy=%d", n+1);
			return	result;
		}
		log_vital("register listen routine to proxy=%d", n+1);
	}
        return  heron_result_state::success;
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
