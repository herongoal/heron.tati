#ifndef _HERON_POOL_H_
#define _HERON_POOL_H_


#include "heron_define.h"
#include <unordered_map>


using namespace std;
namespace   heron{namespace tati{
class   heron_pool{
public:
        heron_pool(uint capacity);
	uint    entity_num() const{
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
        tati_node_t     m_node_list;
        tati_node_t     m_node_pool;
};
}}//namespace heron::tati


#endif //_HERON_POOL_H_
