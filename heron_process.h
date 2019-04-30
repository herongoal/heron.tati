#ifndef _HERON_PROCESS_H_
#define _HERON_PROCESS_H_


#include "heron_define.h"


namespace   heron{namespace   tati{
void    on_connected(ulong routine_id, uint events);
void    on_closed(ulong routine_id, uint events);
void    on_data(ulong routine_id, uint events);
void    on_timer(ulong routine_id, uint events);

class	heron_process_thread{
public:
	heron_process_thread();
	~heron_process_thread();
	sint	process_timers();
	sint	init();
	sint	react();
};
}}//namespace heron::tati


#endif //_HERON_PROCESS_H_
