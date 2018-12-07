#ifndef  __BUFFER_HPP__
#define  __BUFFER_HPP__

#include <common.hpp>

#ifdef __cplusplus

namespace niktes
{
class Cbuffer
{
	Cbuffer (const Cbuffer& other)   ;
	Cbuffer & operator = (const Cbuffer& other);
public: 
    Cbuffer ();
   ~Cbuffer ();
  bool       is_full   ();
  bool       is_empty  ();
  uint16_t   free_space();
  uint16_t   used      ();
  
  uint16_t        add      (const uint8_t *data, const uint16_t len);
  uint16_t        move     (uint16_t from, uint16_t to = 0);
  const  uint8_t* get_point(uint16_t  from = 0);
  uint16_t        get_size (uint16_t  from = 0);
  void            set_external_buffer(uint8_t * _buf, uint16_t * used, uint16_t buf_size);
  
  protected:
	void release();
	void alloc  (unsigned int size);
	uint8_t   * m_ptr  = nullptr;
	uint16_t    __m_used = 0;
	uint16_t    m_size   = 0;
	uint16_t   &m_used   = m_used;
	    bool    m_extern = false;
};


inline bool   Cbuffer::is_full   ()
{
	return m_used == 0 ? true:false;
}

inline bool   Cbuffer::is_empty  ()
{
	return m_used == 0 ? true:false;
}
inline uint16_t   Cbuffer::free_space()
{
	return m_size-m_used; 
}

inline uint16_t   Cbuffer::used      ()
{
  return m_used;
}

inline const  uint8_t* Cbuffer::get_point(uint16_t  from )
{
  return m_ptr+ ((from < m_used) ? from : m_used);
}

inline uint16_t  Cbuffer::get_size (uint16_t  from)
{
  return (from<m_used) ? (m_used - from ): 0;
}

}// end of namespace niktes

#else

#error "Buffer must compiled with C++"

#endif

#endif
