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
 sint    log_append(log_level level, const char *fmt, va_list ap){
	 const   char*   levels[] = {"trace","debug","event","alert",
		 "error","vital","panic","fatal"};
	 char    buf[1024] = { 0 };

	 struct  timeval         m_time;
	 struct  tm  tt;

	 gettimeofday(&m_time, nullptr);
	 localtime_r(&m_time.tv_sec, &tt);

	 sint shift = snprintf(buf, sizeof(buf), "[%04d%02d%02d %02d:%02d:%02d-%s]",
			 tt.tm_year + 1900, tt.tm_mon, tt.tm_mday,
			 tt.tm_hour, tt.tm_min, tt.tm_sec, levels[level-1]);


	 sint len = vsnprintf(buf+shift, sizeof(buf)-shift, fmt, ap);
	 buf[len+shift]='\0';
	 cout << buf << endl;
	 return  heron_result_state::success;
 }

sint	log_fatal(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	sint    result = log_append(log_level_panic, format, ap);
	va_end(ap);
	return	result;
}


}}//namespace heron::tati
