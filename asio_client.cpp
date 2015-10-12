
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

/*TODO:
 * timer benutzen: http://think-async.com/asio/boost_asio_1_4_8/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp
 *
 */

#define SLEEP_MS(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x));

typedef boost::asio::ip::tcp tcp;

class TCPClient
{
    typedef boost::function<void(const boost::system::error_code& error)> ConnectedCallback_t;
    typedef boost::function<void()> DisconnectedCallback_t;
    typedef boost::function<void(const boost::system::error_code& error, size_t bytesTransferred, const char *data)> ReceivedCallback_t;
    typedef boost::function<void(const boost::system::error_code& error, size_t bytesTransferred)> SentCallback_t;

    ConnectedCallback_t    m_connected_callback;
    DisconnectedCallback_t m_disconnected_callback;
    ReceivedCallback_t     m_received_callback;
    SentCallback_t         m_sent_callback;

public: 
	TCPClient() : 
		m_work(m_io_service),
        m_worker( boost::bind(&TCPClient::Worker, this) ), //creates and runs the thread
        m_socket( new tcp::socket(m_io_service) ),
        m_disconnected_callback(0),
        m_connected_callback(0),
        m_received_callback(0),
        m_sent_callback(0)
	{
	}

    ~TCPClient()
    {
        doDisconnect();
        m_io_service.stop();
        if (m_worker.joinable())
            m_worker.join();
    }

    void set_connected_callback(ConnectedCallback_t cb) { m_connected_callback = cb; }
    void set_disconnected_callback(DisconnectedCallback_t cb) { m_disconnected_callback = cb; }
    void set_received_callback(ReceivedCallback_t cb) { m_received_callback = cb; }
    void set_sent_callback(SentCallback_t cb) { m_sent_callback = cb; }

	void Worker()
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
				std::cout << "Error: " << e.what() << std::endl;
			}
		}
		std::cout << "Worker ended" << std::endl;
	}

	boost::asio::io_service& get_io_service() { return m_io_service; }	

	void connectTo(std::string host, int16_t port)
	{
		tcp::resolver resolver(m_io_service);
		tcp::resolver::query query(host, boost::lexical_cast< std::string >(port));
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		
		m_socket->async_connect(*endpoint_iterator, boost::bind(&TCPClient::handle_connect, this, boost::asio::placeholders::error) );
	}

	void handle_connect(const boost::system::error_code& error)
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
			if (m_socket->is_open())
			{
				m_socket->close();
			}
			std::cout << "Warning: could not connect : " << error.message() << std::endl;
		}
	}

	void startReceive()
	{
		m_socket->async_read_some(boost::asio::buffer(m_rx_buffer), boost::bind(&TCPClient::handle_read, this, 
					boost::asio::placeholders::error, 
					boost::asio::placeholders::bytes_transferred));
	}

	void handle_read(const boost::system::error_code& ec, size_t bytes_transferred)
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
            std::cout << "Error: could not read : " << ec.message() << " : " << ec.value() << std::endl;
			return;
		}
	}

    void send_data(const char *data, size_t len)
    {
        async_write(*m_socket, boost::asio::buffer(data, len), m_sent_callback); //ensures that all is written when handler is invoked
        //m_socket->async_send(boost::asio::buffer(data, len), m_sent_callback);
    }

	void disconnect()
	{
		if (m_socket->is_open())
		{
			m_io_service.post( boost::bind(&TCPClient::doDisconnect, this) );
		}
	}

	void doDisconnect()
	{
		if (m_socket->is_open())
		{
			m_socket->cancel(); // cancel all operations on the socket
			m_io_service.reset(); // remove pending operations
            m_socket->close(); // close connection
            m_socket.reset( new tcp::socket(m_io_service) ); // reset to usable state

            m_disconnected_callback();
		}
	}

private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_work;
	boost::shared_ptr<tcp::socket> m_socket;
    boost::array<char, 65000> m_rx_buffer;

    boost::thread m_worker;
};

class Algorithm
{
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
		m_tcpClient.connectTo("127.0.0.1", 1234);
		//m_tcpClient.connectTo("192.168.1.143", 1234);
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
			startSend();
        }
    }

    void handle_disconnect()
    {
        m_bConnected = false;
        std::cout << "Alog::Disconnected" << std::endl;
    }

    void handle_read(const boost::system::error_code& ec, size_t bytes_transferred, const char *data)
    {
        std::string tmp;
        tmp.assign((const char*)data, bytes_transferred);
        //std::cout << "RX: " << tmp << std::endl;
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
		SLEEP_MS(1000);
		break;
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
	a.stop();

	
	boost::thread s( boost::bind( &sleeper ) );	
	s.join();
	return 0;
}
