#ifndef _HERON_BUFFER_H_
#define _HERON_BUFFER_H_


#include "heron_define.h"
#include <string>


using	namespace std;
namespace heron{namespace tati{
class   heron_buffer
{
public:
        heron_buffer(uint capacity):m_capacity(capacity), m_pos(0)
        {
        }

        heron_buffer&   operator=(const heron_buffer &hb) = default;
        heron_buffer(const heron_buffer &hb) = default;

        ~heron_buffer()
        {
		m_pos = 0;
		m_data.clear();
        }

        bool    append(const void* buffer, uint len);

        uint    unused_len() const
        {
                return  m_capacity - m_data.size();
        }

        uint    data_len() const
        {
                return  m_data.size() - m_pos;
        }

        const   void*   data_ptr() const
        {
                return  m_data.c_str() + m_pos;
        }

        bool    consume(uint len);

private:
        unsigned        m_capacity;
        unsigned        m_pos;
        string     m_data;
};
}}//namespace heron::tati


#endif //_HERON_BUFFER_H_
