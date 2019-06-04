#ifndef	_HERON_NETWORK_H_
#define	_HERON_NETWORK_H_


#include "heron_define.h"
#include "heron_thread.h"
#include "heron_routine.h"
#include "heron_pool.h"
#include "heron_channel.h"
#include "heron_logger.h"
#include <sys/epoll.h>
#include <pthread.h>


using namespace std;
namespace   heron{namespace tati{
class	heron_log_writer;
class	heron_routine;
class	heron_engine;
class   heron_network_thread:public heron_thread{
public:
        heron_network_thread(){
		m_network_channel = nullptr;
		m_control_channel = nullptr;
	}
	void	process_events(sint fd, heron_event events);
	void	set_routine_timeout(sint fd, int timeout_ms);
	void	process_timers(){}
	slong   gen_monotonic_ms();
	void	inspect();
	sint	send_message(sint fd, const void *data, unsigned len);
	sint	close_routine(sint fd);
        void    dispose_events(sint timeout_in_ms);
	static void*	start(void* arg);
	uint	m_proxy_id;

protected:
	heron_synch_channel*      m_network_channel;
	heron_synch_channel*      m_control_channel;

        heron_log_writer*         m_logger;
        heron_channel_routine*    m_channel_routine;

	void	half_exit();
	void*	run();
	friend  class   heron_factory;
	friend  class   heron_engine;
	pthread_t	m_thread;
};

class   heron_listen_routine:public heron_routine{
public:
	virtual ~heron_listen_routine();

	virtual bool    vital() const{
		return	true;
	}

	sint    append_send_data(const void *data, unsigned len){
		return  0;
	}
	sint    on_events(heron_event ev);

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
	friend	class		heron_factory;
	friend	class		heron_engine;
	heron_thread*		m_proxy;
	heron_log_writer*	m_logger;
	static const int        s_reuse_port;
};

class   heron_tcp_routine: public heron_routine{
public:
        heron_tcp_routine(uint label, int fd):heron_routine(label, fd){}
        virtual ~heron_tcp_routine();
        sint on_events(heron_event ev);
        sint do_connect(ulong, const char *ipaddr, uint16_t port);
        sint check_conn_state(int err);
	sint	do_nonblock_recv(heron::tati::heron_buffer&);
	sint	do_nonblock_write(const void *buf, unsigned len, unsigned &sent);
        virtual sint    append_send_data(const void *data, unsigned len);
        uint    get_changed_events();
        sint on_writable();
        sint on_readable();
        sint on_error();
private:
        heron_log_writer*       m_logger;
        static const uint tcp_events = EPOLLIN | EPOLLOUT;
};
}}//namespace heron::tati
#endif//_HERON_NETWORK_H_
