#include "tcpserver.h"


TCPServer::TCPServer(uint16_t port, IServerDelegate *pHandler) :
    m_acceptor(m_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    m_pHandler(pHandler),
    m_worker( boost::bind(&TCPServer::run, this) )
{
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    start_accept();
}

// Run the worker thread
void TCPServer::run()
{
    m_io_service.run();
}

void TCPServer::start_accept()
 {
     // Create a new connection to handle a client. Passing a reference
     // to db to each connection poses no problem since the server is
     // single-threaded.
     //
    TCPServerSession::SPointer session = TCPServerSession::create(m_io_service);

     // Asynchronously wait to accept a new client
     //
     m_acceptor.async_accept(session->get_socket(),
         boost::bind(&TCPServer::handle_accept, this, session,
             boost::asio::placeholders::error));
 }

 void TCPServer::handle_accept(TCPServerSession::SPointer session,
         const boost::system::error_code& error)
 {
     // A new client has connected
     //
     if (!error) {
         // Start the connection
         //
         m_pHandler->session_started(session);

         // Accept another client
         //
         start_accept();
     }
 }


