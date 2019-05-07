#ifndef	_HERON_WORKER_H_
#define	_HERON_WORKER_H_


#include "heron_define.h"
#include <pthread.h>


namespace heron{namespace tati{
class	heron_heavy_work_thread{
public:
	sint init();
	static void *start(void *arg);
protected:
	friend class	heron_engine;
	pthread_t	m_thread;
};//end of class heron_heavy_work_thread
}}//namespace heron::tati

#endif//_HERON_WORKER_H_
