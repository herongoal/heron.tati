#include "heron_queue.h"
#include <string>
#include <cstring>


using namespace std;
namespace heron{namespace tati{
heron_queue*  heron_queue::create(uint capacity)
{
        /**
         * At least 1 byte should be reserved used to keep the wpos 1 byte
         * before the rpos in which case the data in the buffer is not 
         * continuous(the buffer is filled with data before the wpos or after the rpos,
         * and the data after the rpos should be consumed first) so as to be
         * distinguished from the other case, in which the data in the buffer
         * is continuous and the rpos <= wpos(rpos == wpos stands for empty buffer).
         */

        heron_queue*    pq = new (std::nothrow)heron_queue(capacity + 1);
        if(nullptr!=pq)
        {
                pq->m_nodes = new heron_queue_node[pq->m_capacity];
                if(nullptr == pq->m_nodes)
                {
                        delete  pq;
                        return  nullptr;
                }
        }

        return  pq;
}
}}//namespace heron::tati
