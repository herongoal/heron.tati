#ifndef _HERON_WORKER_H_
#define _HERON_WORKER_H_


#include "heron_define.h"
#include "heron_thread.h"
#include "heron_pool.h"
#include <pthread.h>


namespace   heron{namespace   tati{
void    on_connected(const heron_context &ctx);
void    on_closed(const heron_context &ctx);
void    on_data(const heron_context &ctx);
void    on_timer(const heron_context &ctx);

class	heron_worker_thread:public heron_thread{
public:
	~heron_worker_thread(){
	}
	sint	process_timers();
	sint	react();
	void*   run();
protected:
	void	half_exit();
	friend  class   heron_engine;
};
}}//namespace heron::tati


#endif //_HERON_WORKER_H_
