


#include "asio_client.h"


TCPClient::TCPClient() :
	m_work(m_io_service),
    //C++11:
    m_worker( [&] { return m_io_service.run(); } ), //creates and runs the thread
    //m_worker( boost::bind(&TCPClient::Worker, this) ), //creates and runs the thread
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


void TCPClient::connect_to(std::string host, int16_t port, uint32_t delayed_by_ms)
{
	if (
		(delayed_by_ms > 0) && // want delayed connect
		(m_delayed_connect_timer.expires_at() > boost::asio::deadline_timer::traits_type::now()) // timer not expired
	)
	{
		m_delayed_connect_timer.expires_from_now(boost::posix_time::milliseconds(delayed_by_ms));
        m_delayed_connect_timer.async_wait(boost::bind(&TCPClient::connect_to, this, host, port, 0));
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
	if (0 != m_connected_callback)
	{
		m_connected_callback(error);
	}
}

/*
void TCPClient::startReceive()
{
    m_socket->async_read_some(boost::asio::buffer(m_rx_buffer),
        boost::bind(&TCPClient::handle_read, this,
				boost::asio::placeholders::error, 
				boost::asio::placeholders::bytes_transferred));
}*/

std::size_t checkEnd(const boost::system::error_code& error, std::size_t bytes_transferred, std::size_t len)
{
    if (error)
    {
        //std::cerr << "Error on receive: " << error.message();
        return 0;
    }
    else
    {
        return len - std::min(len,bytes_transferred);
    }
}

void TCPClient::start_receive(void *pBuffer, size_t len)
{
    if (len == 0)
        return;
    if (0 == m_received_callback)
        return;

    boost::asio::async_read(*m_socket, boost::asio::buffer(pBuffer, len),
        boost::bind(&checkEnd, _1, _2, len),
        boost::bind(m_received_callback,
             boost::asio::placeholders::error,
             boost::asio::placeholders::bytes_transferred,
             (char*)pBuffer));

    //m_socket->async_read_some(boost::asio::buffer(m_rx_buffer), boost::bind(&TCPClient::handle_read, this,
    //            boost::asio::placeholders::error,
    //            boost::asio::placeholders::bytes_transferred));
}


void TCPClient::send_data(const void *data, size_t len)
{
	async_write(*m_socket, boost::asio::buffer(data, len), m_sent_callback); //ensures that all is written when handler is invoked
	//m_socket->async_send(boost::asio::buffer(data, len), m_sent_callback);
}

void TCPClient::disconnect()
{
	m_delayed_connect_timer.cancel();

    m_io_service.post(boost::bind(&TCPClient::doDisconnect, this));
}

void TCPClient::doDisconnect()
{
    m_io_service.reset(); // remove pending operations
    if (m_socket->is_open())
	{
		m_socket->close(); // close connection
	}
    m_socket.reset(new tcp::socket(m_io_service)); // reset to usable state

	m_disconnected_callback();
}

/*
void TCPClient::Worker()
{
    std::cerr << "Worker started" << std::endl;
    while (false == m_io_service.stopped())
    {
        try
        {
            m_io_service.run();
            std::cerr << "IOSERVICE ended" << std::endl;
        }
        catch(boost::system::system_error const& e)
        {
            std::cout << "Exception: " << e.what() << std::endl;
        }
    }
    std::cerr << "Worker ended" << std::endl;
}*/



