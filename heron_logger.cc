#include "heron_logger.h"


#include <stdarg.h>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>


using namespace std;
namespace heron{namespace tati{
void    log_fatal(const char *format, ...)
{
                        //append_log_prefix(log_fatal);

                        char*   buf_pos = nullptr;
                        int     buf_len = 0;

                        va_list ap;
                        va_start(ap, format);
                        int msg_len = vsnprintf(buf_pos, buf_len, format, ap);
                        va_end(ap);

                        //append_log_data(msg_len);
}

void tom()
{
	        //        dup2(m_log_fds[0], STDOUT_FILENO);
	        //                //        dup2(m_log_fds[0], STDERR_FILENO);
	        //
	        //                        const socklen_t buf_len = 4 * 1024 * 1024;
	        //                                //setsockopt(m_log_fds[1], SOL_SOCKET, SO_SNDBUF, &buf_len, sizeof(buf_len));
	        //                                        //setsockopt(m_log_fds[1], SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(buf_len));
}


}}//namespace heron::tati
