#ifndef _HERON_ENGINE_H_
#define _HERON_ENGINE_H_


#include "heron_define.h"
#include "heron_logger.h"
#include "heron_routine.h"
#include "heron_network.h"
#include <signal.h>
#include <string>


using namespace std;
namespace heron{namespace tati{
class   heron_engine{
public:
        heron_engine(const string &log_file, log_level level, uint slice_kb);
        virtual ~heron_engine();

	int init();

        static  heron_engine *get_instance()
        {
                return  m_engine_instance;
        }

        ulong   send_message(const string &addr, short port, const void *data, unsigned len);
        ulong   send_message(heron_routine *session, const void *data, unsigned len);


	void	register_conn_handle_routine(conn_handle_routine handler);
	void	register_data_handle_routine(data_handle_routine handler);
	void	register_time_handle_routine(time_handle_routine handler);


        ulong   register_timer(int interval, int times);
        ulong   delete_timer(ulong unique_timer_id);


        static  void    signal_handle(int sigid, siginfo_t *si, void *unused);


	ulong   create_listen_routine(ulong label, const char *ipaddr, uint16_t port);
        ulong   create_channel(ulong label, const char *ipaddr, uint16_t port);
        ulong   start_service_at_port(ulong label, const char *ipaddr, uint16_t port);
        ulong   stop_service_at_port(ulong label, const char *ipaddr, uint16_t port);

private:
	static heron_engine     *m_engine_instance;
	heron_network_thread    m_net_threads;
	log_level		m_log_level;
	int	m_log_fds[2];
};
}}//namespace heron::tati


#endif //_HERON_ENGINE_H_
