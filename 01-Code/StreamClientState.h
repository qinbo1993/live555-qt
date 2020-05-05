#ifndef STREAMCLIENTSTATE_H
#define STREAMCLIENTSTATE_H


#include "liveMedia.hh"
#include "UsageEnvironment.hh"
#include "BasicUsageEnvironment.hh"
#include "Groupsock.hh"

class StreamClientState
{
public:
    StreamClientState();
    virtual ~StreamClientState();

  public:
    MediaSubsessionIterator		*m_pIter;
    MediaSession				*m_pSession;
    MediaSubsession				*m_pSubSession;
    TaskToken 					m_StreamTimerTask;
    double 						m_dDuration;
};

#endif // STREAMCLIENTSTATE_H
