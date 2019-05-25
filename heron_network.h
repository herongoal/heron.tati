#ifndef	_HERON_NETWORK_H_
#define	_HERON_NETWORK_H_


#include "heron_define.h"
#include "heron_routine.h"
#include "heron_pool.h"
#include <sys/epoll.h>
#include <pthread.h>


using namespace std;
namespace   heron{namespace tati{
class	heron_routine;
class   heron_network_thread{
public:
        heron_network_thread();
	void	process_events(ulong routine_id, uint events);
	void	set_routine_timeout(ulong routine_id, int timeout_ms);
	void	process_timers();
	slong   gen_monotonic_ms();
	void	inspect();
	sint	send_message(ulong dest_routine_id, const void *data, unsigned len);
	sint	close_routine(ulong routine_id);
	sint	add_routine(heron_routine *rt);
	uint    get_changed_events() const{
                return  EPOLLIN & ~m_managed_events;
        }
        void    react();
	sint	init();
	static void*	start(void* arg);
	heron_pool	m_routine_pool;
	int	m_epoll_fd;
	uint	m_proxy_id;
	int	m_log_fds[2];
protected:
        heron_log_writer*       m_log_writer;
	void	half_exit();
	sint	m_managed_events;
	void	run();
	friend  class   heron_engine;
	pthread_t	m_thread;
};
class   tcp_listen_routine:public heron_routine{
public:
        static  tcp_listen_routine*     create(sint fd);
	virtual ~tcp_listen_routine();

	virtual bool    vital() const{
		return	true;
	}

	sint    on_event(heron_event ev);

	uint	get_changed_events();

	virtual int     inspect(){
		return  0;
	}
	int                 on_readable();
	void                on_error();

private:
	tcp_listen_routine(uint label, int fd);
	static const int        s_reuse_port;
};
}}//namespace heron::tati
#endif//_HERON_NETWORK_H_
