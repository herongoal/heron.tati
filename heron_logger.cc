#include "heron_logger.h"


#include <stdarg.h>
#include <signal.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>


using namespace std;
namespace heron{namespace tati{
void    log_fatal(const char *format, ...)
{
                        //append_log_prefix(log_fatal);

                        char   buf_pos[1024];
                        int     buf_len = 0;
	cout << "fatal:" << format << endl;

                        va_list ap;
                        va_start(ap, format);
                        int msg_len = vsnprintf(buf_pos, buf_len, format, ap);
                        va_end(ap);

	cout << "fatal:" << string(buf_pos,msg_len) << endl;
                        //append_log_data(msg_len);
}


}}//namespace heron::tati
