#include <lwip.hpp>
//#include <hal_includes.h>
extern "C"{
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_eth.h"
}



extern "C"  netif * get_gnetif()
{
	extern netif gnetif;
	return &gnetif;
}


typedef enum
{

	ETHERNET_LINK_FIRST,
	ETHERNET_LINK_DN,
	ETHERNET_LINK_UP,

}eth_link_t;



extern "C" eth_link_t get_ethernet_link()
{

	uint32_t phyreg;
	static   eth_link_t eth_link = ETHERNET_LINK_FIRST;
	int      is_linked;
	//extern   struct netif gnetif;

	HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &phyreg);
	is_linked = (phyreg & PHY_LINKED_STATUS) ? 1 : 0;

	if (is_linked)
		{
		if (eth_link==ETHERNET_LINK_FIRST)
			MX_LWIP_Init();
		    eth_link=ETHERNET_LINK_UP;
		}
	    else
		{
		if (eth_link==ETHERNET_LINK_UP)
			{
			  eth_link = ETHERNET_LINK_DN;
			}
		}

	return eth_link;
}


namespace niktes
{

//netif lwip_app::m_gnetif = nullptr;

void lwip_app::init_inet()
{
  if(!m_gnetif)
  {
   m_gnetif = get_gnetif();
   ethernetif_init(m_gnetif);
  }
}

lwip_app::lwip_app()
{
}

void lwip_app::init()
{
  init_inet();
}



void lwip_app::step()
{
	uint8_t etl =  get_ethernet_link();
	set_link_status(etl == ETHERNET_LINK_UP ? true : false );

	if(m_is_linked)
	   MX_LWIP_Process();
}


void lwip_app::set_link_status(bool is_linked)
{
	if(m_is_linked != is_linked)
	{
		m_is_linked = is_linked;
		if(is_linked)
			netif_set_link_up(m_gnetif);
		else
			netif_set_link_down(m_gnetif);
		on_link(is_linked);
	}
}






}//end of namespace
