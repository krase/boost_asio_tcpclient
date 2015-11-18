#ifndef TCPSERVERSESSION_H
#define TCPSERVERSESSION_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <stdint.h>

#include "isessiondelegate.h"

class TCPServerSession : public boost::enable_shared_from_this<TCPServerSession>
{
public:
    typedef boost::shared_ptr<TCPServerSession> SPointer;

    static SPointer create(boost::asio::io_service& io_service)
    {
        return SPointer(new TCPServerSession(io_service));
    }

    void setSessionDelegate(ISessionDelegate *delegate) { m_pDelegate = delegate; }

    boost::asio::ip::tcp::socket &get_socket() { return m_socket; }

    void read_data(void *pBuffer, size_t len);

    void write_data(const void *data, size_t len);


protected:
    TCPServerSession(boost::asio::io_service &io_service);

private:
    boost::asio::ip::tcp::socket m_socket;
    ISessionDelegate            *m_pDelegate;

};

#endif // TCPSERVERSESSION_H
