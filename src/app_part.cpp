

#include <common.hpp>

namespace niktes
{

 app_part * app_part::list = nullptr;

 app_part::app_part()
 {
 }


 app_part::~app_part()
 {

 }


 void app_part::reg_instance  ()
 {
   if(!this->m_prev && !this->m_next)
   {
	   m_next = list;
	   list = this;
   }
 }

 void app_part::unreg_instance()
 {
   if(m_prev)
	  m_prev->m_next = m_next;
	  else
	  {
		if(list == this)list = m_next;
	  }
 }


 void app_part::run(int infinite)
 {
	app_part * curr = list;
	while(curr)
	{
	 curr->step();
     curr = curr->m_next;
     if(!curr && infinite) curr = list;
	}
 }
}
