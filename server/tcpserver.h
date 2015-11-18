#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "tcpserversession.h"
#include "iserverhandler.h"

class TCPServer
{
public:
    TCPServer(uint16_t port, IServerHandler *pHandler);

private:
    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    IServerHandler *m_pHandler;

    void start_accept();
    void run();
    void handle_accept(TCPServerSession::SPointer connection, const boost::system::error_code &error);
};

#endif // TCPSERVER_H
