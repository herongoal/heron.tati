#ifndef _HERON_DEFINE_H_
#define _HERON_DEFINE_H_


#include <cstring>
#include <stdint.h>
#include <string>


using namespace std;
namespace heron{namespace tati{
typedef uint64_t    ulong;
typedef	int64_t     slong;
typedef	uint32_t    uint;
typedef	int32_t     sint;
typedef	uint16_t    ushort;
typedef	int16_t     sshort;
typedef	uint8_t     uchar;
typedef	int8_t      schar;

slong   gen_monotonic_ms();

const   char hex_digits[] = "0123456789abcdef";

inline  ulong   gen_id(uchar scope, ulong seq){
        char str[32] = { 0 };
        uint len = 1;
        str[sizeof(str) - len] = 0;

        while(seq > 0){
            str[sizeof(str) - ++len]=hex_digits[seq % 9];
	    seq = seq / 9;
        }

        str[sizeof(str) - ++len] = '9';

        while(scope > 0){
            str[sizeof(str) - ++len]=hex_digits[scope % 9];
	    scope = scope / 9;
        }

        return strtoll(str + sizeof(str) - len, nullptr, 10);
}

struct  heron_routine_attr{
    heron_routine_attr():m_label(),m_create_time(0){}
    heron_routine_attr& operator=(const heron_routine_attr &attr) = default;
    heron_routine_attr(const heron_routine_attr &attr) = default;

    ulong       m_label;
    ulong       m_create_time;
};

struct  heron_routine_stat{
        ulong        send_times;
        ulong        recv_times;
        ulong        send_bytes;
        ulong        recv_bytes;
};

typedef sint	heron_event;
const	heron_event	heron_socket_event_none = 0;
const	heron_event	heron_socket_readable = 1 << 0;
const   heron_event	heron_socket_writable = 1 << 1;
const   heron_event	heron_socket_read_hup = 1 << 2;
const   heron_event	heron_socket_peer_hup = 1 << 3;
const   heron_event	heron_socket_error = 1 << 4;

enum	enm_directive{
	directive_proc_msg = 10,
	directive_send_msg = 11,
};

struct  heron_result_state{
        static const sint decode_data_less_than_needed = -8611;
        static const sint encode_buff_less_than_needed = -8612;
        static const sint failed_to_resolve_addr = -8612;
        static const sint failed_to_connect_peer = -8613;
        static const sint failed_to_send_req = -8614;
        static const sint failed_to_send_rsp = -8615;
        static const sint invalid_msg_received = -8616;
        static const sint invalid_req_received = -8617;
        static const sint invalid_rsp_received = -8618;
        static const sint done = 0;
        static const sint success = 0;
};

struct  heron_context{
        heron_routine_attr      attr;
        heron_routine_stat      stat;
};
}}//namespace heron::tati


#endif //_HERON_DEFINE_H_
