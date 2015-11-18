#ifndef ISERVERDELEGATE_H__
#define ISERVERDELEGATE_H__

#include "tcpserversession.h"


class IServerDelegate
{
public:
    virtual void session_started(TCPServerSession::SPointer session) = 0;

};

#endif // ISERVERDELEGATE_H__

