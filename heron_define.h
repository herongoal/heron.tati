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


enum    log_level{
        log_level_trace = 1,
        log_level_debug = 2,
        log_level_event = 2,
        log_level_alert = 3,
        log_level_error = 4,
        log_level_vital = 5,
        log_level_fatal = 6,
};


slong   gen_monotonic_ms();

const   char hex_digits[] = "0123456789abcdef";

ulong   gen_id(uchar scope, ulong seq){
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
    heron_routine_attr(ulong label, ulong ts):m_label(label),m_create_time(ts){}
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

enum    heron_event{
        heron_socket_readable = 1 << 0,
        heron_socket_writable = 1 << 1,
        heron_socket_read_hup = 1 << 2,
        heron_socket_peer_hup = 1 << 3,
        heron_socket_error = 1 << 4,
};


const ulong   distinct_constant_tantalum = 0x74616e74616c756d;
const ulong   distinct_constant_titanium = 0x746974616e69756d;
const uchar   distinct_constant_version = 0x31;


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

struct  heron_segment_meta{
	heron_segment_meta(uint req_seq, uint rsp_seq):prefix_tantalum(distinct_constant_tantalum),
		req_sequence(req_seq), rsp_sequence(rsp_seq)
        {
        }
        ulong   prefix_tantalum;
        uint    req_sequence;
        uint    rsp_sequence;

        struct{
                uchar   version;
                uchar   type;
                uchar   flag;
                uchar   ordinal;
        }segment;

	uchar   routine_length;
	ushort  payload_length;
};

struct  rpc_segment_boundary{
        ulong    hybrid_checksum;
        ulong    suffix_titanium;
};

struct tati_session_t{
        uint    last_self_sequence;
        uint    last_peer_sequence;
};
}}//namespace heron::tati


#endif //_HERON_DEFINE_H_
