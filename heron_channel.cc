#include "heron_channel.h"
#include "heron_logger.h"
#include "heron_engine.h"
#include <fcntl.h>
#include <string>
#include <cstring>


using namespace std;
namespace heron{namespace tati{
heron_synch_channel*	heron_synch_channel::create(heron_engine *engine, uint channel_id){
	heron_synch_channel* channel = new heron_synch_channel();
	if(socketpair(AF_UNIX, SOCK_DGRAM, 0, channel->m_socketpair) < 0)
	{
                engine->log_fatal("Create-Channel.socketpair error: channel_id=%d,errno=%d,errmsg=%s", channel_id, errno, strerror(errno));
		return nullptr;
        }

        if(fcntl(channel->m_socketpair[0], F_SETFL, O_NONBLOCK) < 0)
        {
                engine->log_fatal("Create-Channel.set-nonblock error: channel_id=%d,fd=%d,errno=%d,errmsg=%s",
				channel_id, errno, strerror(errno));
		return nullptr;
        }

        if(fcntl(channel->m_socketpair[2], F_SETFL, O_NONBLOCK) < 0)
        {
		if (engine == nullptr){
			cout << "bork" << endl;
		}
                engine->log_fatal("Create-Channel.set-nonblock error: channel_id=%d,fd=%d,errno=%d,errmsg=%s",
				channel_id, errno, strerror(errno));
		return nullptr;
        }

        const socklen_t buf_len = 64 * 1024;
        if(setsockopt(channel->m_socketpair[0], SOL_SOCKET, SO_SNDBUF, &buf_len, sizeof(buf_len)) < 0){
                engine->log_alert("Create-Channel.set-sndbuf error: channel_id=%d,fd=%d,errno=%d,errmsg=%s",
				channel_id, channel->m_socketpair[0], errno, strerror(errno));
	}
        if(setsockopt(channel->m_socketpair[0], SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(buf_len)) < 0){
                engine->log_alert("Create-Channel.set-rcvbuf error: channel_id=%d,fd=%d,errno=%d,errmsg=%s",
				channel_id, channel->m_socketpair[0], errno, strerror(errno));
	}
        if(setsockopt(channel->m_socketpair[1], SOL_SOCKET, SO_SNDBUF, &buf_len, sizeof(buf_len)) < 0){
                engine->log_alert("Create-Channel.set-sndbuf error: channel_id=%d,fd=%d,errno=%d,errmsg=%s",
				channel_id, channel->m_socketpair[0], errno, strerror(errno));
	}
        if(setsockopt(channel->m_socketpair[1], SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(buf_len)) < 0){
                engine->log_alert("Create-Channel.set-rcvbuf error: channel_id=%d,fd=%d,errno=%d,errmsg=%s",
				channel_id, channel->m_socketpair[0], errno, strerror(errno));
	}

	for(uint n = 0; n < sizeof(channel->m_queue)/sizeof(channel->m_queue[0]); ++n){
		channel->m_queue[n] = heron_queue::create(buf_len);
	}

	return  channel;
}

heron_channel_routine::heron_channel_routine(heron_queue *buff, int fd):heron_routine(0,fd),m_buff(buff)
{
}

heron_channel_routine* heron_channel_routine::create(heron_queue *buff, int fd)
{
	return	nullptr;
}
}}//namespace heron::tati
