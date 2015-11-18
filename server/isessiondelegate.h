#ifndef ISESSIONDELEGATE_H__
#define ISESSIONDELEGATE_H__

//#include "tcpserversession.h"



class ISessionDelegate
{
public:

    virtual void handle_read(const boost::system::error_code& error, size_t bytes_transferred, char *data) = 0;
    virtual void handle_write(const boost::system::error_code &error, size_t bytes_transferred) = 0;
};

#endif // ISERVERHANDLER

