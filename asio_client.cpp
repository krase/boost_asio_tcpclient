

#define SLEEP_MS(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x));

#include "asio_client.h"


TCPClient::TCPClient() :
	m_work(m_io_service),
    m_worker( boost::bind(&TCPClient::Worker, this) ), //creates and runs the thread
    m_socket( new tcp::socket(m_io_service) ),
    m_disconnected_callback(0),
    m_connected_callback(0),
    m_received_callback(0),
    m_sent_callback(0),
	m_delayed_connect_timer(m_io_service)
{
}

TCPClient::~TCPClient()
{
	m_delayed_connect_timer.cancel();
    doDisconnect();
    m_io_service.stop();
    if (m_worker.joinable())
        m_worker.join();
}


void TCPClient::Worker()
{
	std::cout << "Worker started" << std::endl;
	while (! m_io_service.stopped())
	{
		try
		{
			m_io_service.run();
		}
		catch(boost::system::system_error const& e)
		{
			std::cout << "Exception: " << e.what() << std::endl;
		}
	}
	std::cout << "Worker ended" << std::endl;
}



void TCPClient::connectTo(std::string host, int16_t port, uint32_t delayed_by_ms)
{
	if (delayed_by_ms > 0)
	{
		m_delayed_connect_timer.expires_from_now(boost::posix_time::milliseconds(delayed_by_ms));
		m_delayed_connect_timer.async_wait(boost::bind(&TCPClient::connectTo, this, host, port, 0));
	}
	else
	{
		m_delayed_connect_timer.expires_at(boost::posix_time::pos_infin);

		tcp::resolver resolver(m_io_service);
		tcp::resolver::query query(host, boost::lexical_cast< std::string >(port));
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		m_socket->async_connect(*endpoint_iterator, boost::bind(&TCPClient::handle_connect, this, boost::asio::placeholders::error));
	}

}

void TCPClient::handle_connect(const boost::system::error_code& error)
{
	if (!error)
	{
        if (0 != m_connected_callback)
        {
            // Connect succeeded.
            m_connected_callback(error);
        }
	}
	else
	{
		if (error.value() == 995)
		{
			return; //ignore
		}
		if (m_socket->is_open())
		{
			m_socket->close();
		}
		std::cout << "Warning: could not connect : " << error.message() << " - " << error.value() <<std::endl;
		disconnect();
		//m_connected_callback(error);
	}
}

void TCPClient::startReceive()
{
	m_socket->async_read_some(boost::asio::buffer(m_rx_buffer), boost::bind(&TCPClient::handle_read, this, 
				boost::asio::placeholders::error, 
				boost::asio::placeholders::bytes_transferred));
}

void TCPClient::handle_read(const boost::system::error_code& ec, size_t bytes_transferred)
{
	if (!ec)
	{
        if (0 != m_received_callback)
        {
            m_received_callback(ec, bytes_transferred, m_rx_buffer.data() );
        }

        startReceive();
	}
	else
	{
		if (ec.value() == 125) // operation canceled
		{
			return;
		}
		else if (ec.value() == 107) // endpoint not connected
		{
			disconnect();
			return;
		}
		else if (ec.value() == 9) // bad file desc
		{
			return;
		}
		else if (ec.value() == 2) // end of file
        {
            disconnect();
            return;
        }
		else if (ec.value() == 1236) // connection closed
		{
			return; // ignore error
		}
		else if (ec.value() == 10054) // connection closed
		{
			disconnect();
			return; // ignore error
		}
		std::cout << "Error: could not read : " << ec.message() << " : " << ec.value() << std::endl;
		return;
	}
}

void TCPClient::send_data(const char *data, size_t len)
{
    async_write(*m_socket, boost::asio::buffer(data, len), m_sent_callback); //ensures that all is written when handler is invoked
    //m_socket->async_send(boost::asio::buffer(data, len), m_sent_callback);
}

void TCPClient::disconnect()
{
	m_delayed_connect_timer.cancel();
	//		if (m_socket->is_open())
	//{
	//}
	m_io_service.post(boost::bind(&TCPClient::doDisconnect, this));
}

void TCPClient::doDisconnect()
{
		//m_socket->cancel(); // cancel all operations on the socket
		m_io_service.reset(); // remove pending operations
		if (m_socket->is_open())
		{
			m_socket->close(); // close connection
		}
		m_socket.reset(new tcp::socket(m_io_service)); // reset to usable state

        m_disconnected_callback();
}


class Algorithm
{		//m_tcpClient.connectTo("192.168.1.143", 1234);
	//std::string m_host = "192.168.1.143";
	std::string m_host = "127.0.0.1";
	uint16_t m_port = 1234;

public:
    Algorithm() :
        m_bConnected(false)
	{
        m_tcpClient.set_connected_callback( boost::bind(&Algorithm::handle_connected, this, _1) );
        m_tcpClient.set_received_callback(boost::bind(&Algorithm::handle_read, this, _1, _2, _3));
        m_tcpClient.set_sent_callback( boost::bind(&Algorithm::handle_send, this, _1, _2));
        m_tcpClient.set_disconnected_callback( boost::bind(&Algorithm::handle_disconnect, this) );
	}

	void start()
	{
		m_tcpClient.connectTo(m_host, m_port, 0);
	}

	void stop()
	{
		m_tcpClient.disconnect();
	}

    void handle_connected(const boost::system::error_code& error)
    {
        if (! error)
        {
			m_bConnected = true;
			std::cout << "Connected - starting receive " << std::endl;
			m_tcpClient.startReceive();
			std::cout << "          - starting send " << std::endl;
			//startSend();
        }
		else if (error.value() == 10061)
		{
			std::cout << "Re-Connecting..." << std::endl;
			m_tcpClient.connectTo(m_host, m_port, 4000);
		}
    }

    void handle_disconnect()
    {
        m_bConnected = false;
        std::cout << "Algo::Disconnected" << std::endl;

		std::cout << "Algo::Re-Connecting..." << std::endl;
		m_tcpClient.connectTo(m_host, m_port, 4000);
    }

    void handle_read(const boost::system::error_code& ec, size_t bytes_transferred, const char *data)
    {
        std::string tmp;
        tmp.assign((const char*)data, bytes_transferred);
        std::cout << "RX: " << tmp << std::endl;
    }

    void startSend()
    {
        std::string tmp("ABC");
        m_tcpClient.send_data(tmp.c_str(), 3);
    }

    void handle_send(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (false == m_bConnected)
            return;

        std::cout << "Sent: " << bytes_transferred << " bytes" << std::endl;
        SLEEP_MS(150);
        std::string tmp("CCC");
        m_tcpClient.send_data(tmp.c_str(), 3);
    }

private:
	TCPClient m_tcpClient;
    bool      m_bConnected;
};

void sleeper()
{
	SLEEP_MS(100);
	//std::cout << "Started sleeper" << std::endl;
	int i = 0;
	while(1)
	{
		SLEEP_MS(4000);
		//break;
	}
	//std::cout << "sleeper end" << std::endl;
}

int main()
{
	

	Algorithm a;	
	
	SLEEP_MS(200);
	a.start();
    SLEEP_MS(500);
    SLEEP_MS(1200);
    a.stop();
	
	SLEEP_MS(300);
	a.start();
	SLEEP_MS(900);
	//a.stop();

	
	boost::thread s( boost::bind( &sleeper ) );	
	s.join();
	return 0;
}
