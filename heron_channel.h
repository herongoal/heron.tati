#ifndef _HERON_CHANNEL_H_
#define _HERON_CHANNEL_H_


#include "heron_define.h"
#include <cstdlib>
#include <atomic>


using namespace std;
namespace heron{namespace tati{
class   heron_channel
{
public:
        static  heron_channel*    create(uint capacity);

        heron_channel&    operator=(const heron_channel &b) = delete;
        heron_channel(const heron_channel &b) = delete;

        ~heron_channel()
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
        heron_channel(unsigned capacity):m_capacity(capacity),
                m_rpos(0), m_wpos(0), m_buff(nullptr)
        {
        }
        const unsigned int              m_capacity;
        std::atomic<unsigned int>       m_rpos;
        std::atomic<unsigned int>       m_wpos;
        unsigned char*                  m_buff;
};//end of class   heron_channel
}}//namespace heron::tati


#endif //_HERON_CHANNEL_H_
