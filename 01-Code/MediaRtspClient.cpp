#include "MediaRtspClient.h"

MediaRtspClient::MediaRtspClient(const char *pUrl, const char *pUser, const char *pPwd)
{
    m_stOpt.m_pUrl = strDup(pUrl);
    m_stOpt.m_pUser = strDup(pUser);
    m_stOpt.m_pPwd = strDup(pPwd);
    m_pScheduler = BasicTaskScheduler::createNew();
    m_pEnv		 = BasicUsageEnvironment::createNew(*m_pScheduler);

}

MediaRtspClient::~MediaRtspClient()
{
    /* clean opt */
    if (m_stOpt.m_pUrl) {
       free(m_stOpt.m_pUrl);
       m_stOpt.m_pUrl = NULL;
    }
    if (m_stOpt.m_pUser) {
       free(m_stOpt.m_pUser);
       m_stOpt.m_pUser = NULL;
    }
    if (m_stOpt.m_pPwd) {
       free(m_stOpt.m_pPwd);
       m_stOpt.m_pPwd = NULL;
    }

    if (m_pEnv) {
        m_pEnv->reclaim();
        m_pEnv = NULL;
    }
    if (m_pScheduler) {
        delete m_pScheduler;
        m_pScheduler = NULL;
    }
}

int32 MediaRtspClient::open()
{
    // Begin by creating a "RTSPClient" object.  Note that there is a separate "RTSPClient" object for each stream that we wish
    // to receive (even if more than stream uses the same "rtsp://" URL).
   ourRTSPClient *client = ourRTSPClient::createNew(*m_pEnv, \
                                               m_stOpt.m_pUrl, \
                                               RTSP_CLIENT_VERBOSITY_LEVEL, \
                                               RTSP_CLIENT_APP_NAME);
   if (NULL == client) {
       return eRTSP_CLIENT_CREATE_NEW_ERR;
   }
   qDebug() << "Creata new success.";

   // Next, send a RTSP "DESCRIBE" command, to get a SDP description for the stream.
   // Note that this command - like all RTSP commands - is sent asynchronously; we do not block, waiting for a response.
   // Instead, the following function call returns immediately, and we handle the RTSP response later, from within the event loop:
   client->sendDescribeCommand(continueAfterDescribe);


   m_pEnv->taskScheduler().doEventLoop();
   return 0;
}

void MediaRtspClient::continueAfterDescribe(RTSPClient *pRtspClient, \
                                            int32 nResultCode, \
                                            char* pResultString)
{
    do {
        UsageEnvironment &env = pRtspClient->envir();
        StreamClientState &scs = ((ourRTSPClient*)pRtspClient)->scs;

        if (0 != nResultCode) {
            qWarning() << "Failed to get a SDP description:" << pResultString;
            delete [] pResultString;
            break;
        }
        char* const pSdpDescription = pResultString;
        qInfo() << "Got a SDP Desctiption." << pResultString;

        // Create a media session object from this SDP description:
        scs.m_pSession = MediaSession::createNew(env, pSdpDescription);
        delete []  pSdpDescription;
        if (NULL == scs.m_pSession) {
            qWarning() << "Failed to create a MediaSession object from the SDP description: "\
                     << env.getResultMsg();
            break;
        } else if (!scs.m_pSession->hasSubsessions()) {
            qWarning() << "This session has no media subsessions (i.e., no \"m=\" lines)\n";
            break;
        }

        // Then, create and set up our data source objects for the session.  We do this by iterating over the session's 'subsessions',
        // calling "MediaSubsession::initiate()", and then sending a RTSP "SETUP" command, on each one.
        // (Each 'subsession' will have its own data source.)
        scs.m_pIter = new MediaSubsessionIterator(*scs.m_pSession);
        setupNextSubsession(pRtspClient);
        return;
    } while(0);

    // An unrecoverable error occurred with this stream.
    shutdownStream(pRtspClient);
}

// By default, we request that the server stream its data using RTP/UDP.
// If, instead, you want to request that the server stream via RTP-over-TCP,
// change the following to True:
#define REQUEST_STREAMING_OVER_TCP False
void  MediaRtspClient::setupNextSubsession(RTSPClient* rtspClient)
{
  UsageEnvironment& env = rtspClient->envir(); // alias
  StreamClientState& scs = ((ourRTSPClient*)rtspClient)->scs; // alias

  scs.m_pSubSession = scs.m_pIter->next();
  if (scs.m_pSubSession != NULL) {
    if (!scs.m_pSubSession->initiate()) {
      qWarning() << "Failed to initiate the subsession: " << env.getResultMsg() << "\n";
      setupNextSubsession(rtspClient); // give up on this subsession; go to the next one
    } else {
      qInfo() << "Initiated the subsession (";
      if (scs.m_pSubSession->rtcpIsMuxed()) {
        qInfo() << "client port " << scs.m_pSubSession->clientPortNum();
      } else {
        qInfo() << "client ports " << scs.m_pSubSession->clientPortNum() << "-" << scs.m_pSubSession->clientPortNum()+1;
      }
      env << ")\n";

      // Continue setting up this subsession, by sending a RTSP "SETUP" command:
      rtspClient->sendSetupCommand(*scs.m_pSubSession, continueAfterSETUP, False, REQUEST_STREAMING_OVER_TCP);
    }
    return;
  }

  // We've finished setting up all of the subsessions.  Now, send a RTSP "PLAY" command to start the streaming:
  if (scs.m_pSession->absStartTime() != NULL) {
    // Special case: The stream is indexed by 'absolute' time, so send an appropriate "PLAY" command:
    rtspClient->sendPlayCommand(*scs.m_pSession, continueAfterPLAY, \
                                scs.m_pSession->absStartTime(), scs.m_pSession->absEndTime());
  } else {
    scs.duration = scs.session->playEndTime() - scs.session->playStartTime();
    rtspClient->sendPlayCommand(*scs.session, continueAfterPLAY);
  }
}
















