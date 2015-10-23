#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/deadline_timer.hpp>
/*TODO:
* timer benutzen: http://think-async.com/asio/boost_asio_1_4_8/doc/html/boost_asio/example/timeouts/async_tcp_client.cpp
*
*/


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
	TCPClient();
	~TCPClient();

	void set_connected_callback(ConnectedCallback_t cb) { m_connected_callback = cb; }
	void set_disconnected_callback(DisconnectedCallback_t cb) { m_disconnected_callback = cb; }
	void set_received_callback(ReceivedCallback_t cb) { m_received_callback = cb; }
	void set_sent_callback(SentCallback_t cb) { m_sent_callback = cb; }

	void connectTo(std::string host, int16_t port, uint32_t delayed_by_ms=0);
	void send_data(const char *data, size_t len);
        void startReceive();
        void startReceive(size_t len);

	void disconnect();
	
	boost::asio::io_service& get_io_service() { return m_io_service; }

private:

	void handle_connect(const boost::system::error_code& error);
	void handle_read(const boost::system::error_code& ec, size_t bytes_transferred);
	void doDisconnect();

	void Worker();

	boost::asio::io_service        m_io_service;
	boost::asio::io_service::work  m_work;
	boost::shared_ptr<tcp::socket> m_socket;
	boost::array<char, (1 << 16)>  m_rx_buffer;
	boost::asio::deadline_timer    m_delayed_connect_timer;
	boost::thread                  m_worker;
};
