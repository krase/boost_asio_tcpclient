#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <boost/asio.hpp>

class TCPServer
{
public:
    TCPServer();

private:
    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
};

#endif // TCPSERVER_H
