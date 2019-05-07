#include "heron_logger.h"


#include <stdarg.h>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>


using namespace std;
namespace heron{namespace tati{
void    fatal_log(const char *format, ...)
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
}}//namespace heron::tati
