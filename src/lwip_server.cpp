/**
 * STM32 C++ shared library
 * Ostapenko D. V.
 * 2018-12-07
 * ver 1.0
 * NIKTES
 */


#include <lwip.hpp>


namespace niktes
{
 tcp_server_socket::tcp_server_socket(tcp_server * owner, tcp_pcb * new_pcb)
             : tcp_socket(new_pcb)
             , prev(nullptr)
             , next(nullptr)
             , m_owner (owner)



 { }


 err_t tcp_server_socket::close()
 {
     if(m_owner)
    	  return m_owner->close_socket(this);
	 return tcp_socket::close();
 }

   bool tcp_server::is_active()
   {
	return (listen_pcb && listen_pcb->state == LISTEN ) ? true : false;
   }

   tcp_server::tcp_server ()
              : tcp_socket()
              , socket_count(0)
              , socket_limit(-1)
              , listen_pcb  (nullptr)
              , socket_list (nullptr)

   {
   }


	err_t     tcp_server::start(const uint16_t port,const ip_addr_t * addr  )
	{
	  err_t & err = last_err;
      if(m_pcb && ( m_pcb->local_ip.addr != addr->addr || m_pcb->local_port != port))
      {
    	 err = stop();
      }
      if(err == ERR_OK && (err = alloc()) == ERR_OK)
      {

          err = bind(port,addr);
          if(ERR_OK == err)
        	  {
        	    listen_pcb = tcp_listen(m_pcb);
        	    if(listen_pcb)
        	    	setup_socket();
        	    else
        	    	err = ERR_MEM;
        	  }

      }
      return err;
	}


	void  tcp_server::setup_socket    ()
	{
		if(m_pcb && listen_pcb)
		{
			tcp_arg   (m_pcb, this);
			tcp_arg   (listen_pcb, this);
			tcp_accept(listen_pcb, socket_accept);
			tcp_err   (listen_pcb, socket_err   );
		}
	}

	err_t     tcp_server::stop ()
	{
       while(socket_list)
    	   close_socket(socket_list);
       if(listen_pcb)
         {
    	    tcp_close(listen_pcb);
            listen_pcb = nullptr;
         }
       return last_err;
	}

	err_t   tcp_server::close_socket    (tcp_server_socket * socket)
	{
      err_t err = ERR_OK;
	  if(socket && socket->m_owner == this)
      {
    	socket->m_owner = NULL;
    	err = socket->close();
    	if(socket->prev)
    	   socket->prev->next = socket->next;
    	else
    	{
    	  if(socket_list == socket)
    		  socket_list = socket->next;
    	}
    	 --socket_count;
      	delete socket;
      }
	  return err;
	}

	tcp_server_socket *  tcp_server::create_socket(tcp_pcb * pcb)
	{
	  return new tcp_server_socket(this,pcb);
	}

    err_t tcp_server::handle_accept   ( tcp_pcb *newpcb, err_t err)
    {
      if(err!= ERR_OK) return err;

       if( socket_count< this->socket_limit)
       {
    	  tcp_server_socket * s = create_socket(newpcb);
    	  if(s)
    	  {
    		++socket_count;
    		s->setup_socket();
    		s->keep_alive_enable(true,1000,3);
    		if(socket_list)
    		{
                socket_list->prev = s;
    			s->next = socket_list;
    		}

            socket_list = s;
            return ERR_OK;
    	  }
    	  else
    		  return ERR_MEM;
       }
    	 return ERR_RST;
    }

    void  tcp_server::handle_error    ( err_t err)
    {
    	UNUSED(err);
    }


	uint16_t  tcp_server::send_all(void * data,uint16_t data_size)
	{
        uint16_t send_count = 0;
		tcp_server_socket * s = socket_list;
		while(s)
		{
		  if(ERR_OK ==	s->send_data(data,data_size))
	        ++send_count;
		  s = s->next;
		}
		return send_count;
	}

    err_t tcp_server::close()
    {
      stop();
      return tcp_socket::close();
    }

}
