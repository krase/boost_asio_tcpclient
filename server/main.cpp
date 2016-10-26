#include <iostream>
#include <boost/thread.hpp>
#include <boost/array.hpp>

#include "tcpserver.h"
#include "tcpserversession.h"

#include "iserverdelegate.h"

#define SLEEP_MS(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x))

// A generic function to show contents of a container holding byte data
// as a string with hex representation for each byte.
//
template <class CharContainer>
std::string show_hex(const CharContainer& c)
{
    std::string hex;
    char buf[16];
    typename CharContainer::const_iterator i;
    for (i = c.begin(); i != c.end(); ++i) {
        std::sprintf(buf, "%02X ", static_cast<unsigned>(*i) & 0xFF);
        hex += buf;
    }
    return hex;
}

class NetCommSessionDelegate : ISessionDelegate
{
public:
    typedef boost::shared_ptr<NetCommSessionDelegate> SPointer;

    NetCommSessionDelegate(TCPServerSession::SPointer session)
    {

    }

    static NetCommSessionDelegate::SPointer create(TCPServerSession::SPointer serverSession)
    {
        NetCommSessionDelegate *sd = new NetCommSessionDelegate(serverSession);
        serverSession->setSessionDelegate(sd);
        return SPointer(sd);
    }

    virtual void handle_read(const boost::system::error_code &error, size_t bytes_transferred, char *data)
    {
        std::cout << "RX: " << (long)bytes_transferred << "; " << show_hex(std::string(data, bytes_transferred)) << std::endl;

        // send data back
        //serverSession->write_data(data, bytes_transferred);
    }

    virtual void handle_write(const boost::system::error_code &error, size_t bytes_transferred)
    {
        std::cout << "TX: " << bytes_transferred << std::endl;

    }
private:
    TCPServerSession::SPointer m_spSession;

};


class NetCommServer : public IServerDelegate
{
public:
    NetCommServer() {};

    void session_started(TCPServerSession::SPointer session)
    {
        m_spNcSession = NetCommSessionDelegate::create( session );

        std::cout << "Started session" << std::endl;

        session->read_data(m_rx_buffer.data(), 4);
        //TODO: register session in array

        // register client
        // start reading messages
        // distribute messages to other sessions
    }
private:
    boost::array<char, (1 << 16)>  m_rx_buffer;

    NetCommSessionDelegate::SPointer m_spNcSession;
};

int main(int argc, char *argv[])
{
    std::cout << "Hello World!" << std::endl;

    NetCommServer nc;

    TCPServer server(1234, &nc);

    boost::thread s( [] { while(1) { SLEEP_MS(1000); } } );
    s.join();

    return 0;
}
