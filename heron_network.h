#ifndef	_HERON_NETWORK_H_
#define	_HERON_NETWORK_H_


#include "heron_define.h"
#include "heron_routine.h"
#include "heron_pool.h"
#include "heron_channel.h"
#include <sys/epoll.h>
#include <pthread.h>


using namespace std;
namespace   heron{namespace tati{
class	heron_log_writer;
class	heron_routine;
class	heron_engine;
class   heron_network_thread{
public:
        heron_network_thread():m_pool(32*1024){}
	static  heron_network_thread* create(heron_engine *engine);
	void	process_events(ulong routine_id, uint events);
	void	set_routine_timeout(ulong routine_id, int timeout_ms);
	void	process_timers(){}
	slong   gen_monotonic_ms();
	void	inspect();
	sint	send_message(ulong dest_routine_id, const void *data, unsigned len);
	sint	close_routine(ulong routine_id);
	sint	register_routine(heron_routine *rt);
	uint    get_changed_events() const{
                return  EPOLLIN & ~m_managed_events;
        }
        void    react();
	sint	init();
	static void*	start(void* arg);
	int	m_epoll_fd;
	uint	m_proxy_id;
protected:
	heron_synch_channel*      m_network_channel;
	heron_synch_channel*      m_control_channel;

        heron_log_writer*         m_log_writer;
        heron_channel_routine*    m_channel_routine;

	void	half_exit();
	sint	m_managed_events;
	void	run();
	heron_pool      m_pool;
	friend  class   heron_engine;
	pthread_t	m_thread;
};
class   heron_listen_routine:public heron_routine{
public:
        static  heron_listen_routine*     create(heron_engine* engine, sint fd);
	virtual ~heron_listen_routine();

	virtual bool    vital() const{
		return	true;
	}

	sint    append_send_data(const void *data, unsigned len){
		return  0;
	}
	sint    on_event(heron_event ev){
		return	0;
	}

	uint	get_changed_events(){
		return  EPOLLIN & ~m_managed_events;
	}

	virtual int     inspect(){
		return  0;
	}
	int                 on_readable();
	void                on_error();

private:
	heron_listen_routine(uint label, int fd):heron_routine(label,fd){}
	heron_log_writer*	m_log_writer;
	static const int        s_reuse_port;
};
}}//namespace heron::tati
#endif//_HERON_NETWORK_H_
