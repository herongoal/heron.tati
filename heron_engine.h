#ifndef _HERON_ENGINE_H_
#define _HERON_ENGINE_H_


#include "heron_define.h"
#include "heron_channel.h"
#include "heron_logger.h"
#include "heron_routine.h"
#include "heron_network.h"
#include "heron_process.h"
#include "heron_worker.h"
#include <signal.h>
#include <string>


using namespace std;
namespace heron{namespace tati{
class   heron_engine{
public:
        static heron_engine* create(const string &log_file, log_level level, uint slice_kb, uint proxy_num, uint worker_num);
        virtual ~heron_engine();

	sint    init();

        static  heron_engine *get_instance()
        {
                return  m_engine_instance;
        }

        ulong   send_message(const heron_context &ctx, const void *data, ushort len);

	void	register_conn_handle_routine(conn_handle_routine handler);
	void	register_data_handle_routine(data_handle_routine handler);
	void	register_time_handle_routine(time_handle_routine handler);


        ulong   register_timer(int interval, int times);
        ulong   delete_timer(ulong unique_timer_id);


        static  void    signal_handle(int sigid, siginfo_t *si, void *unused);


	sint    start_worker_threads();
	sint    start_network_threads();
	sint    start_threads();
	sint    start_service();
	sint    stop_service();
	void    stop_threads();
	ulong   create_listen_routine(ulong label, const char *ipaddr, uint16_t port);
        ulong   create_channel(ulong label, const char *ipaddr, uint16_t port);
        ulong   start_service_at_port(ulong label, const char *ipaddr, uint16_t port);
        ulong   stop_service_at_port(ulong label, const char *ipaddr, uint16_t port);

private:
	heron_engine();
	/*
		each network thread has a pair of channel as well as a socketpair connected to process thread, used to transfer logic data
		each heavy work thread has a pair of channel as well as a socketpair connected to process thread, used to transfer logic data
		logs were transfer from threads to main threads via channels as well. so socketpairs were created as well
		contrl signals were transfer via socketpair.
	*/
	static heron_engine*	m_engine_instance;
	log_level	        m_log_level;
	uint	m_proxy_num;
	uint	m_worker_num;

	heron_synch_channel*	m_synch_channels[32];
	heron_log_channel*	m_log_channels[32];

	heron_process_thread*	 m_process_thread;
	heron_network_thread*    m_network_threads[4];
	heron_worker_thread*     m_worker_threads[2];
};
}}//namespace heron::tati


#endif //_HERON_ENGINE_H_
