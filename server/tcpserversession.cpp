#include "tcpserversession.h"
#include <boost/bind.hpp>


TCPServerSession::TCPServerSession(boost::asio::io_service &io_service) :
    m_socket(io_service),
    m_pDelegate(0)
{

}


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

void TCPServerSession::read_data(void *pBuffer, size_t len)
{
    if (len == 0)
        return;
    if (! m_pDelegate )
        return;

    //ensures that all data is written when ISessionDelegate::handle_read is called
    boost::asio::async_read(m_socket, boost::asio::buffer(pBuffer, len),
        boost::bind(&checkEnd, _1, _2, len),
        boost::bind(&ISessionDelegate::handle_read, m_pDelegate,
             boost::asio::placeholders::error,
             boost::asio::placeholders::bytes_transferred,
             (char*)pBuffer)
    );
}

void TCPServerSession::write_data(const void *data, size_t len)
{
    if (len == 0)
        return;
    if (! m_pDelegate )
        return;

    //ensures that all is written when handler is invoked
    async_write(m_socket, boost::asio::buffer(data, len),
                boost::bind(&ISessionDelegate::handle_write, m_pDelegate,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred)
                );
}
