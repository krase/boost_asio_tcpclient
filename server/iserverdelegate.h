#ifndef ISERVERHANDLER_H__
#define ISERVERHANDLER_H__

#include "tcpserversession.h"

class IServerHandler
{
public:
    virtual void session_started(TCPServerSession::SPointer session) = 0;

};

class ISessionHandler
{
public:
    virtual void handle_read(const boost::system::error_code &error, size_t bytes_transferred) = 0;
    virtual void handle_write(const boost::system::error_code &error, size_t bytes_transferred) = 0;
};

#endif // ISERVERHANDLER

