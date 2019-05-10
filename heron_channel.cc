#include "heron_channel.h"
#include "heron_logger.h"
#include <fcntl.h>
#include <string>
#include <cstring>


using namespace std;
namespace heron{namespace tati{
heron_synch_buffer*  heron_synch_buffer::create(uint capacity)
{
        /**
         * At least 1 byte should be reserved used to keep the wpos 1 byte
         * before the rpos in which case the data in the buffer is not 
         * continuous(the buffer is filled with data before the wpos or after the rpos,
         * and the data after the rpos should be consumed first) so as to be
         * distinguished from the other case, in which the data in the buffer
         * is continuous and the rpos <= wpos(rpos == wpos stands for empty buffer).
         */

        heron_synch_buffer*    pbuff = new (std::nothrow)heron_synch_buffer(capacity + 1);
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

bool    heron_synch_buffer::fetch(void *data, unsigned int len)
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

bool    heron_synch_buffer::append(const void* data, unsigned len)
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

heron_synch_channel*	heron_synch_channel::create()
{
	heron_synch_channel* channel = new heron_synch_channel();

	if(socketpair(AF_UNIX, SOCK_DGRAM, 0, channel->m_socketpair) < 0)
        {
                log_event("init.socketpair error,errno=%d,errmsg=%s",
                                errno, strerror(errno));
		return nullptr;
        }

        if(-1 == fcntl(channel->m_socketpair[0], F_SETFL, O_NONBLOCK)
        	|| -1 == fcntl(channel->m_socketpair[1], F_SETFL, O_NONBLOCK))
        {
                log_event("set nonblock failed,errno=%d,errmsg=%s",
                                errno, strerror(errno));
		return nullptr;
        }

	for(int n = 0; n < 2; ++n)
	{
		heron_synch_buffer *buff = &channel->m_synch_buffs[n];
		int fd = channel->m_socketpair[n];
		channel->m_routines[n] = heron_channel_routine::create(buff, fd);
	}
	return  channel;
}

heron_channel_routine::heron_channel_routine(heron_synch_buffer *buff, int fd):heron_routine(0,fd),m_buff(buff)
{
}

heron_channel_routine* heron_channel_routine::create(heron_synch_buffer *buff, int fd)
{
	return	nullptr;
}
}}//namespace heron::tati
