#ifndef	_HERON_WORKER_H_
#define	_HERON_WORKER_H_


#include "heron_define.h"
#include "heron_logger.h"
#include <pthread.h>


namespace heron{namespace tati{
class	heron_worker_thread{
public:
	sint init();
	static void *start(void *arg);

protected:
	uint	m_proxy_id;
	void*	run();
	heron_log_writer	*m_log_writer;
	friend class	heron_engine;
	pthread_t	m_thread;
};//end of class heron_worker_thread
}}//namespace heron::tati

#endif//_HERON_WORKER_H_
