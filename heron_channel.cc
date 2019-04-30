#include "heron_channel.h"
#include <string>
#include <cstring>


using namespace std;
namespace heron{namespace tati{
heron_channel*  heron_channel::create(uint capacity)
{
        /**
         * At least 1 byte should be reserved used to keep the wpos 1 byte
         * before the rpos in which case the data in the buffer is not 
         * continuous(the buffer is filled with data before the wpos or after the rpos,
         * and the data after the rpos should be consumed first) so as to be
         * distinguished from the other case, in which the data in the buffer
         * is continuous and the rpos <= wpos(rpos == wpos stands for empty buffer).
         */

        heron_channel*    pbuff = new (std::nothrow)heron_channel(capacity + 1);
        if(pbuff != nullptr)
        {
                pbuff->m_buff = (unsigned char *)malloc(pbuff->m_capacity);
                if(pbuff->m_buff == nullptr)
                {
                        delete  pbuff;
                        return  nullptr;
                }
        }

        return  pbuff;
}

bool    heron_channel::fetch(void *data, unsigned int len)
{
        unsigned int    wpos = m_wpos;
        if(wpos < m_rpos)
        {
                if(len <= m_capacity - m_rpos + wpos)
                {
                        if(m_rpos + len <= m_capacity)
                        {
                                memcpy(data, m_buff + m_rpos, len);
                        }
                        else
                        {
                                memcpy(data, m_buff + m_rpos, m_capacity - m_rpos);
                                memcpy((char *)data + m_capacity - m_rpos, m_buff, len - m_capacity + m_rpos);
                        }

                        m_rpos = (m_rpos + len) % m_capacity;
                        return  true;
                }
                else
                {
                        return  false;
                }
        }
        else
        {
                if(len <= wpos - m_rpos)
                {
                        memcpy(data, m_buff + m_rpos, len);
                        m_rpos = (m_rpos + len) % m_capacity;
                        return  true;
                }
                else
                {
                        return  false;
                }
        }
}

bool    heron_channel::append(const void* data, unsigned len)
{
        unsigned int rpos = m_rpos;
        if(m_wpos < rpos)
        {
                if(m_wpos + len + 1 <= rpos)
                {
                        memcpy(m_buff + m_wpos, data, len);
                        m_wpos = (m_wpos + len) % m_capacity;
                        return  true;
                }
                else
                {
                        return  false;
                }
        }
        else
        {
                if(len > m_capacity - (m_wpos - rpos) - 1)
                {
                        return  false;
                }

                if(len <= m_capacity - m_wpos)
                {
                        memcpy(m_buff + m_wpos, data, len);
                        m_wpos = (m_wpos + len) % m_capacity;
                }
                else
                {
                        memcpy((char *)m_buff + m_wpos, data, m_capacity - m_wpos);
                        memcpy((char *)m_buff, (char *)data + m_capacity - m_wpos, m_wpos + len - m_capacity);
                        m_wpos = m_wpos + len - m_capacity;
                }
                return  true;
        }
}
}}//namespace heron::tati
