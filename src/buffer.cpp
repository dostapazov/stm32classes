#include <buffer.hpp>
#include <memory.h>

#ifdef __cplusplus
namespace niktes
{
	Cbuffer::Cbuffer ()
	{
	}

	Cbuffer::~Cbuffer()
	{
      release();
	}

	void Cbuffer::release()
	{
      if(m_ptr)
      {
       if(this->m_extern)
    	  {
    	   m_extern = false;

    	  }
          else
          delete [] m_ptr;
       m_used = __m_used;
       m_ptr  = nullptr;
       m_size = m_used = 0;
      }

	}

	void  Cbuffer::set_external_buffer(uint8_t * _buf, uint16_t * used, uint16_t buf_size)
	{
		release();
		if(_buf && used && buf_size)
		{
           m_extern = true;
           m_ptr    = _buf;
           m_used   = *used;
           m_size   = buf_size;
           m_used   = min(m_used,m_size);
		}
	}

	void Cbuffer::alloc  (unsigned int size)
	{
     if(m_size != size)
     {
		 release();
	     m_ptr   =  new uint8_t[size];
	     m_size  = size;
     }
	}

uint16_t        Cbuffer::add      (const uint8_t *data, const uint16_t len)
{

  if(!is_full())
  {
    uint16_t add_sz = min(len,free_space());
    memcpy(m_ptr+m_used,data,add_sz);
    m_used+=add_sz;
    return add_sz;
  }
  return 0;
}

uint16_t        Cbuffer::move     (uint16_t from, uint16_t to )
{
  // Перемещение из точки в точку
  if( m_ptr &&  from<m_used && to < from)
  {
	  uint16_t move_size = m_used-from;
	  memmove(m_ptr+to,m_ptr+from,move_size);
	  m_used -= move_size;
	  return move_size;
  }
  return 0;
}

} // end of namspace niktes

#endif
