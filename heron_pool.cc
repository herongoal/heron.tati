#include "heron_pool.h"


namespace heron{namespace tati{
heron_pool::heron_pool(uint capacity):m_capacity(capacity)
{
	m_node_list.m_elem = nullptr;
	m_node_list.m_id = 0uLL;
	m_node_list.m_prev = &m_node_list;
	m_node_list.m_next = &m_node_list;

	m_node_pool.m_elem = nullptr;
	m_node_pool.m_id = 0uLL;
	m_node_pool.m_prev = &m_node_pool;
	m_node_pool.m_next = &m_node_pool;
}

bool    heron_pool::insert_element(ulong id, void *elem){
	unordered_map<ulong, tati_node_t*>::iterator pos = m_index.find(id);

	if(pos != m_index.end()){
		return	false;
	}

	return	true;
}

void*   heron_pool::remove_element(ulong id)
{
	unordered_map<ulong, tati_node_t*>::iterator pos = m_index.find(id);
        
        if(pos == m_index.end()){
                return  nullptr;
        }

	void*   ptr = pos->second;
	return  ptr;
}

void*   heron_pool::search_element(ulong id)
{
	unordered_map<ulong, tati_node_t*>::iterator pos = m_index.find(id);
	if(pos == m_index.end()){
		return  nullptr;
	}
	return	pos->second;
}

void*   heron_pool::current_element()
{
	return	nullptr;
}

bool    heron_pool::forward_element()
{
	return	false;
}
}}//namespace heron::tati
