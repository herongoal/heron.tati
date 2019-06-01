#ifndef _HERON_FACTORY_H_
#define _HERON_FACTORY_H_


#include "heron_define.h"


namespace heron{namespace tati{
class	heron_network_thread;
class	heron_listen_routine;
class	heron_tcp_routine;
class	heron_engine;

class	heron_factory{
public:
	static	heron_network_thread*	create_network_thread(heron_engine *engine);
	static	heron_listen_routine*	create_listen_routine(heron_engine *engine, sint fd);
	static	heron_tcp_routine*	create_tcp_routine(uint label, sint fd);
};
}}//namespace heron::tati


#endif //_HERON_FACTORY_H_
