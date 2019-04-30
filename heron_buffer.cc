#include "heron_buffer.h"


#include <string>

using namespace std;
namespace heron{namespace tati{
bool    heron_buffer::append(const void* buffer, const unsigned len)
{
        if(m_data.size() + len <= m_capacity){
                m_data.append(static_cast<const char *>(buffer), len);
                return  true;
        }
	else {
                return  false;
        }
}

bool    heron_buffer::consume(unsigned len)
{
	if (len > m_data.size() - m_pos) {
        	return  false;
        }

        m_pos += len;
        if(m_pos < m_data.size() && m_pos >= m_capacity / 4){
		m_data.erase(0, m_pos);
		m_pos = 0;
        }
        else    if(m_pos == m_data.size())
        {
                m_pos = 0;
                m_data.clear();
        }
        return  true;
}
}}//namespace heron::tati
