#ifndef _HERON_PROTOCOL_H_
#define _HERON_PROTOCOL_H_


#include "heron_define.h"
#include <cstring>
#include <stdint.h>
#include <string>


using namespace std;
namespace heron{namespace tati{
const ulong   distinct_constant_tantalum = 0x74616e74616c756d;
const ulong   distinct_constant_titanium = 0x746974616e69756d;
const uchar   distinct_constant_version = 0x31;


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


#endif //_HERON_PROTOCOL_H_
