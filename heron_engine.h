#ifndef _HERON_ENGINE_H_
#define _HERON_ENGINE_H_


#include "heron_define.h"
#include "heron_channel.h"
#include "heron_logger.h"
#include "heron_routine.h"
#include "heron_network.h"
#include "heron_process.h"
#include "heron_thread.h"
#include <signal.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <iostream>


using namespace std;
namespace heron{namespace tati{
class	heron_synch_channel;
class	heron_log_channel;
class   heron_engine{
public:
        static heron_engine* create(const string &log_file, log_level level, uint slice_kb, uint proxy_num, uint worker_num);
        virtual ~heron_engine(){}

	sint    init();

        static  heron_engine *get_instance()
        {
                return  m_instance;
        }

        ulong   send_message(const heron_context &ctx, const void *data, ushort len);

	void	register_conn_handle_routine(conn_handle_routine handler);
	void	register_data_handle_routine(data_handle_routine handler);
	void	register_time_handle_routine(time_handle_routine handler);


        ulong   register_timer(int interval, int times);
        ulong   delete_timer(ulong unique_timer_id);


        static  void    signal_handle(int sigid, siginfo_t *si, void *unused);


	sint    start_threads();
	sint    start_service();
	sint    stop_service(){
		return	0;
	}
	void    stop_process();
	void    stop_threads();
	void    clear_resource();
        ulong   create_channel(ulong label, const char *ipaddr, uint16_t port);


        ulong   register_listen_port(ulong label, const char *ipaddr, uint16_t port);
        ulong   stop_service_at_port(ulong label, const char *ipaddr, uint16_t port);
        sint    run();
	sint    listen_at_port(const char *ipaddr, ushort port);

	static  const uchar     state_starting = 0;
	static  const uchar     state_running = 1;
	static  const uchar     state_exiting = 2;

	uchar	get_state() const{
		return  m_state;
	}
private:
	friend  class   heron_factory;
	struct	listen_endpoint{
		sint	fd;
		ushort	port;
	};

	sint    resource();

	vector<listen_endpoint>	m_endpoints;
	uchar   m_state;
	heron_engine():m_thread_created(0){}

	sint	log_alert(const char *format, ...){
		va_list ap;
		va_start(ap, format);
		sint    result = log_append(log_level_panic, format, ap);
		va_end(ap);
		return	result;
	}
	sint	log_vital(const char *format, ...){
		va_list ap;
		va_start(ap, format);
		sint    result = log_append(log_level_panic, format, ap);
		va_end(ap);
		return	result;
	}

	sint	log_fatal(const char *format, ...){
		va_list ap;
		va_start(ap, format);
		sint    result = log_append(log_level_panic, format, ap);
		va_end(ap);
		return	result;
	}

	sint	create_channels();
	/*
		each network thread has a pair of channel as well as a socketpair connected to process thread, used to transfer logic data
		each heavy work thread has a pair of channel as well as a socketpair connected to process thread, used to transfer logic data
		logs were transfer from threads to main threads via channels as well. so socketpairs were created as well
		contrl signals were transfer via socketpair.
	*/
	static  heron_engine*   m_instance;
	struct  sockaddr_in     m_listen_addrs[32];
	friend	class		heron_synch_channel;
	friend	class		heron_factory;

	dispatch_routine	m_dispatch_routine;
	uint                    m_thread_created;
	uint                    m_proxy_num;
	uint                    m_worker_num;
	heron_thread*           m_threads[8];
	heron_synch_channel*    m_synch_channels[8];
};
}}//namespace heron::tati


#endif //_HERON_ENGINE_H_
