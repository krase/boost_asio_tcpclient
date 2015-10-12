#include "asio_client.h"

#define SLEEP_MS(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x));

class Algorithm
{		//m_tcpClient.connectTo("192.168.1.143", 1234);
	const std::string m_host;
	const uint16_t m_port;

public:
    Algorithm() :
		 m_host("127.0.0.1")
		,m_port(1234)
        ,m_bConnected(false)
		,m_bWantReconnect(true)
	{
        m_tcpClient.set_connected_callback( boost::bind(&Algorithm::handle_connected, this, _1) );
        m_tcpClient.set_received_callback(boost::bind(&Algorithm::handle_read, this, _1, _2, _3));
        m_tcpClient.set_sent_callback( boost::bind(&Algorithm::handle_send, this, _1, _2));
        m_tcpClient.set_disconnected_callback( boost::bind(&Algorithm::handle_disconnect, this) );
	}

	void start()
	{
		m_bWantReconnect = true;
		m_tcpClient.connectTo(m_host, m_port, 0);
	}

	void stop()
	{
		m_bWantReconnect = false;
		m_tcpClient.disconnect();
	}

    void handle_connected(const boost::system::error_code& error)
    {
        if (! error)
        {
			m_bConnected = true;
			std::cout << "Connected - starting receive " << std::endl;
			m_tcpClient.startReceive();
        }
        else
		{
			if (error.value() == 125) // operation canceled
			{
				return;
			}
			std::cout << error.message() << ": "<< error.value() << " -> Disconnecting" << std::endl;
			m_tcpClient.disconnect();
		}
    }

    void handle_disconnect()
    {
        m_bConnected = false;
        std::cout << "Algo::Disconnected" << std::endl;
		if (m_bWantReconnect)
		{
			std::cout << "Algo::Re-Connecting..." << std::endl;
			m_tcpClient.connectTo(m_host, m_port, 4000);
		}
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred, const char *data)
    {
		if (error)
		{
			if (error.value() == 1236) // win: networkconnection locally closed
			{
				return;
			}
			std::cout << "Algo::handle_read: could not read : " << error.message() << " : " << error.value() << std::endl;
			m_tcpClient.disconnect(); // will cause a delayed re-connect
			return;
		}

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
	bool      m_bWantReconnect;
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
    SLEEP_MS(3000);
    a.stop();

	SLEEP_MS(300);
	a.start();

	
	boost::thread s( boost::bind( &sleeper ) );	
	s.join();
	return 0;
}