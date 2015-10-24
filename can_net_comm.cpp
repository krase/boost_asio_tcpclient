#include "asio_client.h"
#include "can_net_comm.h"

#include "frame.h"

#include <memory>

#define SLEEP_MS(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x));




Connection::Connection() :
     m_host("127.0.0.1")
    ,m_port(1234)
    ,m_bConnected(false)
    ,m_bWantReconnect(true)
    ,m_mode(READ_SIZE)
{
    m_tcpClient.set_connected_callback( boost::bind(&Connection::handle_connected, this, _1) );
    m_tcpClient.set_received_callback(boost::bind(&Connection::handle_read, this, _1, _2, _3));
    m_tcpClient.set_sent_callback( boost::bind(&Connection::handle_send, this, _1, _2));
    m_tcpClient.set_disconnected_callback( boost::bind(&Connection::handle_disconnect, this) );
}

void Connection::start()
{
    //m_tcpClient.connectTo("192.168.1.143", 1234);
    m_bWantReconnect = true;
    m_tcpClient.connect_to(m_host, m_port, 100);
}

void Connection::stop()
{
    m_bWantReconnect = false;
    m_tcpClient.disconnect();
}

void Connection::handle_connected(const boost::system::error_code& error)
{
    if (! error)
    {
        m_bConnected = true;
        std::cout << "Connected - starting receive " << std::endl;
        m_mode = READ_SIZE;
        m_tcpClient.start_receive(m_rx_buffer.data(), m_size_field_len);
    }
    else
    {
        if (error.value() == 125) // linux: operation canceled
        {
            return;
        }
        std::cout << error.message() << ": "<< error.value() << " -> Disconnecting" << std::endl;
        m_tcpClient.disconnect();
    }
}

void Connection::handle_disconnect()
{
    m_bConnected = false;
    std::cout << "Algo::Disconnected" << std::endl;
    if (m_bWantReconnect)
    {
        std::cout << "Algo::Re-Connecting..." << std::endl;
        m_tcpClient.connect_to(m_host, m_port, 4000);
    }
}

void Connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred, const char *data)
{
    if (error)
    {
        if (error.value() == 125) // linux: operation canceled
        {
            return;
        }
        if (error.value() == 1236) // win: networkconnection locally closed
        {
            return;
        }
        if (error.value() == 9) // linux: bad file descriptor
        {
            return;
        }
        std::cout << "Algo::handle_read: could not read : " << error.message() << " : " << error.value() << std::endl;
        m_tcpClient.disconnect(); // will cause a delayed re-connect through handle_disconnect()
    }
    else
    {
        if (m_mode == READ_SIZE)
        {
            // read data with size...
            const size_t size = data[0]; // unpack size
            //std::cout << "len: " << size << std::endl;
            m_mode = READ_DATA;
            m_tcpClient.start_receive(m_rx_buffer.data(), size);
        }
        else if (m_mode == READ_DATA)
        {
            //std::string tmp;
            //tmp.assign((const char*)data, bytes_transferred);
            //std::cout << "size: " << bytes_transferred << " data: " << tmp << std::endl;
            // work with received data:

            // parse frame
            std::shared_ptr<Frame> f = Frame::make_shared((const uint8_t*)data, bytes_transferred);
            std::cout << "RX: " << std::string(*f) << std::endl;

            m_mode = READ_SIZE; //read size again
            m_tcpClient.start_receive(m_rx_buffer.data(), m_size_field_len);
        }
        else
        {
            std::cout << "Should not happen" << std::endl;
        }
    }
}

void Connection::startSend()
{
    std::string tmp("ABC");
    m_tcpClient.send_data(tmp.c_str(), 3);
}

void Connection::handle_send(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (false == m_bConnected)
        return;

    std::cout << "Sent: " << bytes_transferred << " bytes" << std::endl;
    SLEEP_MS(150);
    std::string tmp("CCC");
    m_tcpClient.send_data(tmp.c_str(), 3);
}


static void sleeper()
{
	SLEEP_MS(100);
	//std::cout << "Started sleeper" << std::endl;
	int i = 0;
	while(1)
	{
		SLEEP_MS(4000);
        //std::cout << "." << std::endl;
		//break;
	}
	//std::cout << "sleeper end" << std::endl;
}

int main()
{
    Connection conn;

	SLEEP_MS(200);
    conn.start();
	SLEEP_MS(3000);
    conn.stop();

	SLEEP_MS(300);
    conn.start();

	
    boost::thread s( boost::bind( &sleeper ) );
    s.join();
	return 0;
}
