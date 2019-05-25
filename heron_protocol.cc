#include "heron_protocol.h"


namespace heron{namespace tati{
uint    encode(void *buf, uint buf_len, const string &method, void *data, uint data_len)
{
        uchar *ptr = static_cast<uchar *>(buf);
        heron_segment_meta meta(0, 0);

        uint len_required = sizeof(meta);

        if(len_required >= buf_len)
        {
                return  heron_result_state::encode_buff_less_than_needed;
        }
        memcpy(ptr, &meta, sizeof(meta));

        if(len_required + method.length() + data_len + sizeof(rpc_segment_boundary) > buf_len)
        {
                return  heron_result_state::encode_buff_less_than_needed;
        }

        for(uint n = 0; n < method.size(); ++n)
        {
                ptr[len_required++] = method[n];
        }
        memcpy(ptr + len_required, data, data_len);
        len_required += data_len;

        //uint checksum = crc32(ptr, len_required);
        //uint hashtm31 = tm31(ptr, len_required);

        rpc_segment_boundary boundary;

        memcpy(ptr + len_required, &boundary, sizeof(boundary));
        return len_required += sizeof(boundary);
}
}}//end of namespace heron::tati
