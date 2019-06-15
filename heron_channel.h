#ifndef _HERON_CHANNEL_H_
#define _HERON_CHANNEL_H_


#include "heron_define.h"
#include "heron_queue.h"
#include "heron_routine.h"
#include <cstdlib>
#include <atomic>


using namespace std;
namespace heron{namespace tati{
class	heron_channel_routine;
class	heron_engine;

class	heron_synch_channel{
public:
	static	heron_synch_channel*	create(heron_engine *engine, uint channel_id);
private:
	heron_synch_channel(){}
	heron_queue*	m_queue[2];
	int		m_socketpair[2];
};

class	heron_channel_routine:public heron_routine{
public:
	heron_channel_routine(heron_queue *buff, int fd);
	static	heron_channel_routine*	create(heron_queue *buff, int fd);

	
        virtual sint    on_event(heron_event ev){
		return	0;
	}

        virtual sint    append_send_data(const void*, uint){
		return	0;
	}

	virtual bool    vital() const
        {
               return false;
        }
protected:
	heron_queue*	m_buff;
};
}}//namespace heron::tati


#endif //_HERON_CHANNEL_H_
