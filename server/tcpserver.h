#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "tcpserversession.h"
#include "iserverdelegate.h"

class TCPServer
{
public:
    TCPServer(uint16_t port, IServerDelegate *pHandler);

private:
    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::thread m_worker;

    IServerDelegate *m_pHandler;

    void start_accept();
    void run();
    void handle_accept(TCPServerSession::SPointer connection, const boost::system::error_code &error);
};

#endif // TCPSERVER_H
