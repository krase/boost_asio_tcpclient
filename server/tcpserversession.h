#ifndef TCPSERVERSESSION_H
#define TCPSERVERSESSION_H

#include <boost/asio.hpp>

class TCPServerSession
{
public:
    typedef boost::shared_ptr<TCPServerSession> pointer;

    static pointer create(boost::asio::io_service& io_service)
    {
        return pointer(new TCPServerSession(io_service));
    }

    boost::asio::ip::tcp::socket &socket();

    void start_receive(void *pBuffer, size_t len);

    void read_data(const void *data, size_t len);


protected:
    TCPServerSession(boost::asio::io_service& io_service);

private:
    boost::asio::ip::tcp::socket m_socket;

    void handle_write(const boost::system::error_code &error, size_t bytes_transferred);

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
};

#endif // TCPSERVERSESSION_H
