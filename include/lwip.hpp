
#ifndef _LWIP_HPP_
#define _LWIP_HPP_

#include <lwip.h>
#include <lwip/tcp.h>
#include <lwip/priv/tcp_priv.h>
#include <common.hpp>

#ifdef __cplusplus
namespace niktes
{
  class tcp_socket
  {
    protected:
	  err_t     last_err      = ERR_OK;
	  tcp_pcb * m_pcb         = nullptr;
	  int       poll_interval = 1;
	  tcp_socket(tcp_pcb * pcb):m_pcb(pcb){};
	  virtual void  setup_socket    ();
	  virtual err_t handle_accept   ( tcp_pcb *newpcb, err_t err);
	  virtual err_t handle_recv     ( void * data, uint16_t len, err_t err);
	  virtual err_t handle_sent     ( uint16_t len);
	  virtual err_t handle_poll     ( );
	  virtual void  handle_error    ( err_t err);
	  virtual err_t handle_connected( tcp_pcb *tpcb, err_t err);
  
	  static err_t socket_accept   (void *arg, tcp_pcb *newpcb, err_t err);
	  static err_t socket_recv     (void *arg, tcp_pcb *  tpcb, pbuf *p, err_t err);
	  static err_t socket_sent     (void *arg, tcp_pcb *  tpcb,u16_t len);
	  static err_t socket_poll     (void *arg, tcp_pcb *  tpcb);
	  static void  socket_err      (void *arg, err_t err);
	  static err_t socket_connected(void *arg, tcp_pcb *tpcb, err_t err);
    public:
	     tcp_socket();
virtual ~tcp_socket();
         virtual err_t close();
                 err_t alloc       (uint16_t priority = TCP_PRIO_NORMAL);
                 err_t bind        (const uint16_t port, const ip_addr_t *ipaddr = IP_ADDR_ANY);
                 err_t connect     (const ip_addr_t *ipaddr, const uint16_t port);
                 err_t send_data   (void * data, uint16_t data_size);
                 err_t flush       (){return m_pcb ? tcp_output(m_pcb) : ERR_CONN;}
                 err_t disconnect  ();
                 err_t last_error  (){return last_err;}
                 bool  is_connected();
             tcp_state state       (){return m_pcb ? m_pcb->state : tcp_state::CLOSED;}
                 void  set_prio    (uint8_t prio){if(m_pcb) tcp_setprio(m_pcb, prio); }
                 void  keep_alive_enable(bool enabled, uint32_t keep_idle, uint8_t sent_cnt);

	  
  };


 class tcp_server;

 class tcp_server_socket: public tcp_socket
 {
 public:
	  friend class tcp_server;
	  tcp_server_socket(tcp_server * owner, tcp_pcb * new_pcb);
	  virtual err_t close() override;
 protected:
	  tcp_server_socket  * prev = nullptr;
	  tcp_server_socket  * next = nullptr;
      tcp_server * m_owner;

 };

 class tcp_server : protected tcp_socket
 {
    protected:

    uint16_t   socket_count =  0;
    uint16_t   socket_limit = -1;
    tcp_pcb * listen_pcb    = nullptr;
    tcp_server_socket * socket_list  =  nullptr;
    err_t      close_socket    (tcp_server_socket * socket);
    friend     err_t tcp_server_socket::close();

    virtual tcp_server_socket * create_socket(tcp_pcb * pcb);
    virtual void  setup_socket    () override;
    virtual err_t close() override;
    virtual err_t handle_accept   ( tcp_pcb *newpcb, err_t err) override;
    virtual void  handle_error    ( err_t err);
    public:
	 tcp_server ();
	 virtual ~tcp_server() override {}
	 err_t      start(const uint16_t port,const ip_addr_t * addr = IP_ADDR_ANY );
	 err_t      stop ();
	 uint16_t   send_all(void * data,uint16_t data_size);

	 bool is_active();
	 void set_conn_limit(uint16_t limit = -1){socket_limit = limit;}

 };



 class lwip_app : public app_part
 {
  protected:
	 netif      * m_gnetif     ;
	 bool               dhcp_enabled  = false;
	 bool               m_is_linked   = false;
	 virtual void init() override;
	 virtual void step() override;
     virtual void on_link  (bool is_linked) = 0;
	    void init_inet();
  public   :
	 lwip_app();
     void set_link_status(bool is_linked);
 };


}
#endif

#endif

