#ifndef _HERON_POOL_H_
#define _HERON_POOL_H_


#include "heron_define.h"
#include <unordered_map>


using namespace std;
namespace   heron{namespace tati{
class   heron_pool{
public:
        heron_pool(uint capacity):m_capacity(capacity){
                m_inuse_list.m_elem = nullptr;
                m_inuse_list.m_id = 0uLL;
                m_inuse_list.m_prev = &m_inuse_list;
                m_inuse_list.m_next = &m_inuse_list;

                m_cache_list.m_elem = nullptr;
                m_cache_list.m_id = 0uLL;
                m_cache_list.m_prev = &m_cache_list;
                m_cache_list.m_next = &m_cache_list;
        }

	uint    entity_num() const
	{
		return  m_index.size();
	}

	bool    insert_element(ulong id, void *elem);
	void*   remove_element(ulong id);
	void*   search_element(ulong id);

	void*   current_element();
	bool    forward_element();

private:
	struct  tati_node_t{
		tati_node_t*        m_prev;
		tati_node_t*        m_next;
		ulong               m_id;
		void*               m_elem;
	};

protected:
        unordered_map<ulong, tati_node_t*> m_index;
        uint            m_capacity;
        tati_node_t     m_inuse_list;
        tati_node_t     m_cache_list;
};
}}//namespace heron::tati


#endif //_HERON_POOL_H_
