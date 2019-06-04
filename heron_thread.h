#ifndef	_HERON_THREAD_H_
#define	_HERON_THREAD_H_


#include "heron_define.h"
#include "heron_logger.h"
#include "heron_pool.h"
#include "heron_channel.h"
#include <pthread.h>


namespace heron{namespace tati{
class	heron_thread{
public:
	heron_thread():m_pool(32 * 1024){
	}
	static  void    *start(void *arg);
	virtual sint    init(){
		return	heron_result_state::success;
	}
        sint	register_routine(heron_routine *rtn);

	virtual ~heron_thread(){
	}
	void*	run();

protected:
	friend class	heron_factory;
	friend class	heron_engine;

	heron_log_writer*	m_logger;
	sint	m_epoll_fd;
	uint	m_proxy_id;
	heron_channel_routine*  m_channel_routine;
	heron_pool	m_pool;
	pthread_t	m_thread;
};//end of class heron_thread
}}//namespace heron::tati

#endif//_HERON_THREAD_H_
