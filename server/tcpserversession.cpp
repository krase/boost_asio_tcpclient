#include "tcpserversession.h"

TCPServerSession::TCPServerSession(boost::asio::io_service &io_service) :
    m_socket(io_service)
{

}

boost::asio::ip::tcp::socket& TCPServerSession::socket()
{
    return m_socket;
}

void TCPServerSession::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{

}

void TCPServerSession::handle_read(const boost::system::error_code &error, size_t bytes_transferred)
{

}

void TCPServerSession::start_receive(void *pBuffer, size_t len)
{

}

void TCPServerSession::read_data(const void *data, size_t len)
{

}
