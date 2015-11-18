#include <iostream>
#include <boost/thread.hpp>

#include "tcpserver.h"
#include "tcpserversession.h"

#include "iserverhandler.h"

#define SLEEP_MS(x) boost::this_thread::sleep(boost::posix_time::milliseconds(x))

class NetCommSessionHandler : ISessionHandler
{

};

class NetCommServer : public IServerHandler
{
public:
    NetCommServer() {};

    void session_started(TCPServerSession::SPointer session)
    {
        m_spSession = session; //TODO: register session in array
        this->start();
    }
private:

    void start()
    {
        // register client
        // start readingmessages
        // distribute messages to other sessions
    }

    TCPServerSession::SPointer m_spSession;
};

int main(int argc, char *argv[])
{
    std::cout << "Hello World!" << std::endl;

    NetCommServer nc;

    TCPServer server(1234, &nc);

    boost::thread s( [&] { SLEEP_MS(1000); } );
    s.join();

    return 0;
}
