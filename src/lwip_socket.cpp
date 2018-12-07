#include <lwip.hpp>


namespace niktes
{
  tcp_socket::tcp_socket()
  {}
  tcp_socket::~tcp_socket()
  {
	close();
  }

  err_t tcp_socket::socket_accept   (void *arg, tcp_pcb *newpcb, err_t err)
  {
	tcp_socket * socket = reinterpret_cast<tcp_socket*>(arg);
	if(socket)
	{
		return socket->handle_accept(newpcb,err);
	}
	return ERR_ARG;
  }
  err_t tcp_socket::socket_recv     (void *arg, tcp_pcb *tpcb,pbuf *p, err_t err)
  {
	err_t res = ERR_OK;
	 tcp_socket * socket = reinterpret_cast<tcp_socket*>(arg);
	if(socket)
	{
		pbuf * ptr = p;
		while(ptr)
		      {
			    res = socket->handle_recv(ptr->payload,ptr->len,err);
			    ptr = ptr->next;
		      }
	}
	else
		res = ERR_ARG;

     if(p)
		{
			if(p->tot_len) tcp_recved(tpcb,p->tot_len);
            pbuf_free(p);
		}

       if(tpcb->state != ESTABLISHED || ERR_IS_FATAL(err) || err == ERR_CONN)
       {
    	   if(socket) socket->close();
    	   else
    		   tcp_close(tpcb);
       }
	return res;
  }
  err_t tcp_socket::socket_sent     (void *arg, tcp_pcb *tpcb,u16_t len)
  {
	tcp_socket * socket = reinterpret_cast<tcp_socket*>(arg);
	if(socket)
	{
		return socket->handle_sent( len);
	}
	return ERR_ARG;
  }
  err_t tcp_socket::socket_poll     (void *arg, tcp_pcb *tpcb)
  {
	tcp_socket * socket = reinterpret_cast<tcp_socket*>(arg);
	if(socket)
	{
		return socket->handle_poll();
	}
	return ERR_ARG;
  }
  void  tcp_socket::socket_err      (void *arg, err_t err)
  {
	tcp_socket * socket = reinterpret_cast<tcp_socket*>(arg);
	if(socket)
	{
		socket->handle_error(err);
	}

  }

  err_t tcp_socket::socket_connected(void *arg, tcp_pcb *tpcb, err_t err)
  {
	tcp_socket * socket = reinterpret_cast<tcp_socket*>(arg);
	if(socket)
	{
		return socket->handle_connected(tpcb, err);
	}
	return ERR_ARG;
  }

    void  tcp_socket::setup_socket    ()
    {
      if(this->m_pcb)
      {
    	tcp_arg    (m_pcb,this);
    	tcp_recv   (m_pcb,socket_recv);
    	tcp_sent   (m_pcb,socket_sent);
    	tcp_err    (m_pcb,socket_err );
    	tcp_poll   (m_pcb ,socket_poll,poll_interval);
      }
    }

    err_t  tcp_socket::alloc  (uint16_t priority )
    {
    	if(!m_pcb)
    	{
           m_pcb = tcp_alloc(priority);
           setup_socket();
           return m_pcb ? ERR_OK : ERR_MEM ;
    	}
     return ERR_ALREADY;
    }

    err_t tcp_socket::bind   (const uint16_t port, const ip_addr_t *ipaddr)
    {
    	if(this->m_pcb)
    		return tcp_bind(m_pcb,ipaddr,port);
        return last_err;
    }

    err_t tcp_socket::connect(const ip_addr_t *ipaddr,const uint16_t port)
    {
    	if(!m_pcb)
     	  alloc();

    	if(this->m_pcb )
    	{

    	  return tcp_connect(m_pcb,ipaddr,port,socket_connected);
    	}
        return ERR_MEM;

    }

    err_t  tcp_socket::close()
    {
      return m_pcb ? tcp_close(m_pcb) : ERR_MEM;
    }

	  err_t tcp_socket::handle_accept   ( tcp_pcb *newpcb, err_t err)
	  {
          return ERR_RST;
	  }

	  err_t tcp_socket::handle_recv     ( void * data, uint16_t len, err_t err)
	  {
			UNUSED(err );
			return ERR_OK;

	  }

	  err_t tcp_socket::handle_sent     ( uint16_t len)
	  {
         return ERR_OK;
	  }

	  bool tcp_socket::is_connected()
	  {
		 return state()== ESTABLISHED ? true : false;
	  }

	  err_t tcp_socket::handle_poll     ( )
	  {

		if(!is_connected()) close();
		return ERR_OK;
	  }

	  void  tcp_socket::handle_error    ( err_t err)
	  {
			UNUSED(err);

	  }

	  err_t tcp_socket::handle_connected( tcp_pcb *tpcb, err_t err)
	  {
			UNUSED(tpcb);
			UNUSED(err );
			return ERR_OK;
	  }

	  err_t tcp_socket::send_data( void * data, uint16_t len)
	  {
		  err_t & err = last_err;
		  err = tcp_write(m_pcb,data,len,TCP_WRITE_FLAG_COPY);
		  if(ERR_IS_FATAL(err) || !this->is_connected() || err == ERR_CONN  )
			  close();
		  return err;
	  }

}
