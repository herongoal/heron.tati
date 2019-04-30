#ifndef _HERON_TIMER_H_
#define _HERON_TIMER_H_


#include "heron_define.h"
#include "heron_pool.h"


namespace heron{namespace tati{
struct	timer_info{
        timer_info(){}
        timer_info(const timer_info &ti) = default;
        timer_info& operator=(const timer_info &ti) = default;

        ulong   next_exec_time() const
        {
                return  m_create_time + (1 + m_execs) * m_interval;
        }

	slong	m_last_inspect_time;
        slong   m_timer_id;
        slong   m_create_time;
        slong   m_times;
        slong   m_interval;
        slong   m_execs;
        slong   m_total_shift;
        slong   m_max_shift;
};//end of class timer_info
}}//namespace heron::tati


#endif //_HERON_TIMER_H_
