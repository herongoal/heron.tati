#ifndef _HERON_QUEUE_H_
#define _HERON_QUEUE_H_


#include "heron_define.h"
#include <cstdlib>
#include <atomic>


using namespace std;
namespace heron{namespace tati{
class   heron_queue
{
public:
        static  heron_queue*    create(uint capacity);

        heron_queue&    operator=(const heron_queue &b) = delete;
        heron_queue(const heron_queue &b) = delete;

        ~heron_queue()
        {
        }

private:
	struct	heron_queue_node{
		uchar	cmd;
		void*	ptr;
		sint	fd;
	};
        heron_queue(unsigned capacity):m_capacity(capacity),
                m_rpos(0), m_wpos(0), m_nodes(nullptr)
        {
        }
        const unsigned int              m_capacity;
        std::atomic<unsigned int>       m_rpos;
        std::atomic<unsigned int>       m_wpos;
	heron_queue_node*		m_nodes;
};//end of class   heron_queue
}}//namespace heron::tati


#endif //_HERON_QUEUE_H_
