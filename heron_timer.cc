#include "heron_timer.h"
#include "heron_logger.h"
#include "heron_pool.h"


namespace heron{namespace tati{
ulong   create_timer(heron_pool *pool, int interval, int times)
{       
        timer_info *node = nullptr;
        node->m_create_time = 0;
        
        log_trace( "add_timer.unique_timer_id=%lu, finished"
                        ",next_exec_time=%ld", 
                        node->m_timer_id, node->next_exec_time());
	return 0;
}


void    delete_timer(heron_pool *pool, ulong timer_id)
{       
	timer_info *ti = static_cast<timer_info*>(pool->search_element(timer_id));

	log_trace( "delete_timer.unique_timer_id=%lu,"
			"timer.m_create_time=%ld,timer.exec_interval=%ld,"
			"timer.exec_times=%ld,timer.total_shift=%d,timer.max_shift=%d",
			ti->m_timer_id,
			ti->m_create_time, ti->m_interval,
			ti->m_execs, ti->m_total_shift, ti->m_max_shift);

	delete        ti;
}
}}//namespace heron::tati
