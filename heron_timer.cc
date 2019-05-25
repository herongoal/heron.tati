#include "heron_timer.h"
#include "heron_pool.h"


namespace heron{namespace tati{
ulong   create_timer(heron_pool *pool, int interval, int times)
{       
        timer_info *node = nullptr;
        node->m_create_time = 0;
	return 0;
}


void    delete_timer(heron_pool *pool, ulong timer_id)
{       
	timer_info *ti = static_cast<timer_info*>(pool->search_element(timer_id));
	delete        ti;
}
}}//namespace heron::tati
