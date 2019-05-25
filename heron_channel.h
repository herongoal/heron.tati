#ifndef _HERON_CHANNEL_H_
#define _HERON_CHANNEL_H_


#include "heron_define.h"
#include "heron_routine.h"
#include <cstdlib>
#include <atomic>


using namespace std;
namespace heron{namespace tati{
class   heron_synch_buffer
{
public:
        static  heron_synch_buffer*    create(uint capacity);

        heron_synch_buffer&    operator=(const heron_synch_buffer &b) = delete;
        heron_synch_buffer(const heron_synch_buffer &b) = delete;

        ~heron_synch_buffer()
        {
                if(m_buff != nullptr)
                {
                        free(m_buff);
                        m_buff = nullptr;
                }
        }

        bool            fetch(void *data, unsigned int len);

        bool            append(const void* data, unsigned len);

        unsigned        unused_len() const
        {
                unsigned int    rpos = m_rpos;
                unsigned int    wpos = m_wpos;

                if(wpos < rpos)
                {
                        return  rpos - wpos - 1;
                }
                else
                {
                        return  m_capacity - (wpos - rpos) - 1;
                }
        }

        unsigned        data_len() const
        {
                int     rpos = m_rpos;
                int     wpos = m_wpos;

                if(wpos < rpos)
                {
                        return  m_capacity - rpos + wpos;
                }
                else
                {
                        return  wpos - rpos;
                }
        }

private:
        heron_synch_buffer(unsigned capacity):m_capacity(capacity),
                m_rpos(0), m_wpos(0), m_buff(nullptr)
        {
        }
        const unsigned int              m_capacity;
        std::atomic<unsigned int>       m_rpos;
        std::atomic<unsigned int>       m_wpos;
        unsigned char*                  m_buff;
};//end of class   heron_synch_buffer

class	heron_channel_routine;
class	heron_engine;

class	heron_synch_channel{
public:
	heron_synch_channel(){}
	static	heron_synch_channel*	create(heron_engine *engine, uint channel_id);
private:
	heron_synch_buffer*	m_synch_buffs[2];
	int			m_socketpair[2];
	heron_channel_routine*	m_routines[2];
};

class	heron_channel_routine:public heron_routine{
public:
	heron_channel_routine(heron_synch_buffer *buff, int fd);
	static	heron_channel_routine*	create(heron_synch_buffer *buff, int fd);

	
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
	heron_synch_buffer*	m_buff;
};
}}//namespace heron::tati


#endif //_HERON_CHANNEL_H_
