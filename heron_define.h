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

enum	enm_state{
	state_success = 0,
	state_response_invalid = -21001,

	state_recv_timeout     = -22001,
	state_recv_peer_close  = -22002,
	state_recv_failed      = -22003,

	state_request_invalid  = -23001,

	state_send_failed      = -24001,
	state_send_peer_close  = -24002,
	state_connect_timeout  = -24003,
	state_connect_error    = -24004,
	state_connect_reset    = -24005,

	state_ceased_data_overlimit   = -25001,
	state_ceased_method_overlimit = -25002,
	state_ceased_buffer_full      = -25003,
};


struct  rpc_segment_meta{
	rpc_segment_meta(uint req_seq, uint rsp_seq):req_sequence(req_seq), rsp_sequence(rsp_seq)
        {

        }

        const ulong    prefix_tantalum = distinct_constant_tantalum;
        uint    req_sequence;
        uint    rsp_sequence;

        struct{
                uchar version;
                uchar type;
                uchar flag;
                uchar serial;
        }segment;

        struct{
                uchar routine;
                uchar data[3];
        }length;
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
