#include "tcpserver.h"



TCPServer::TCPServer() : m_acceptor(m_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1234))
{

}
