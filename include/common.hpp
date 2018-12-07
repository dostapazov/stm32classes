/**
 * STM32 C++ shared library
 * Common defines
 * Ostapenko D. V.
 * 2018-12-07
 * ver 1.0
 * NIKTES
 */

#ifndef __COMMON_DEF_HPP_
#define __COMMON_DEF_HPP_
 
  #include <sys/_stdint.h> 

namespace niktes
{
  template <typename T>
  T min(const T& v1,const T& v2) {return (v1<v2) ? v1 : v2; }
  template <typename T>
  T max(const T& v1,const T& v2) {return (v1>v2) ? v1 : v2; }

  template <typename T>
  void swap( T& v1, T& v2)
  {
	 T  temp = v1;
	 v1 = v2;
	 v2 = temp;
  }

  
  class app_part
  {
   protected:
	        app_part();
   void reg_instance    ();
   void unreg_instance  ();
   
   virtual void init(){};
   virtual void step() = 0;
   virtual ~app_part();
   
   app_part * m_prev;
   app_part * m_next;
   static   app_part * list;
   public:
   static  void run(int infinite = 0);
   
  };
  
}  
  


#endif
