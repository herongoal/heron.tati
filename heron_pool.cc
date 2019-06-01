#include "heron_pool.h"
#include <iostream>


using namespace std;


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
	
	tati_node_t *node = nullptr;
	if (m_node_pool.m_next != &m_node_pool){
		node = m_node_pool.m_next;
		m_node_pool.m_next = m_node_pool.m_next->m_next;
		m_node_pool.m_next->m_prev = &m_node_pool;
	}else{
		node = new tati_node_t();
	}

	node->m_id = id;
	node->m_elem = elem;
	node->m_prev = m_node_list.m_prev;
	node->m_next = &m_node_list;
	m_node_list.m_prev->m_next = node;
	m_node_list.m_prev = node;

	m_index[node->m_id] = node;
	return	true;
}

void*   heron_pool::remove_element(ulong id)
{
	unordered_map<ulong, tati_node_t*>::iterator pos = m_index.find(id);
        if(pos == m_index.end()){
                return  nullptr;
        }

	tati_node_t *node = pos->second;
	node->m_next->m_prev = node->m_prev;
	node->m_prev->m_next = node->m_next;
	m_index.erase(pos);

	node->m_next = m_node_pool.m_next;
	node->m_prev = &m_node_pool;
	m_node_pool.m_next->m_prev = node;
	m_node_pool.m_next = node;
	return	nullptr;
}

void*   heron_pool::search_element(ulong id)
{
	unordered_map<ulong, tati_node_t*>::iterator pos = m_index.find(id);
	if(pos == m_index.end()){
		return  nullptr;
	}
	tati_node_t* node = pos->second;
	return	node->m_elem;
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
