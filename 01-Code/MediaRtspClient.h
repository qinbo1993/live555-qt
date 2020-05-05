#ifndef MEDIA_RTSPCLIENT_H
#define MEDIA_RTSPCLIENT_H


#include <QCoreApplication>
#include <QDebug>

#include "liveMedia.hh"
#include "UsageEnvironment.hh"
#include "BasicUsageEnvironment.hh"
#include "Groupsock.hh"
#include "OurRtspClient.h"

#include "StreamClientState.h"

/**************************************************/
typedef qint8 	int8;
typedef qint16 int16;
typedef qint32 int32;
typedef qint64 int64;

typedef quint8	uint8;
typedef quint16	uint16;
typedef quint32	uint32;
typedef quint64	uint64;

#define RTSP_CLIENT_APP_NAME		"RtspClient"
#define RTSP_CLIENT_VERBOSITY_LEVEL	1

enum {
    eRTSP_CLIENT_SUCCESS = 0,
    eRTSP_CLIENT_FAILE = -1,
    eRTSP_CLIENT_INVALID = -2,

    eRTSP_CLIENT_CREATE_NEW_ERR = -1000,
};

/**************************************************/

typedef struct {
    char *m_pUrl;
    char *m_pUser;
    char *m_pPwd;
}RtspOpt;


class MediaRtspClient {
public:
    MediaRtspClient(const char* pUrl, const char* pUser, const char *pPwd);
    ~MediaRtspClient();
    int32 open();
    int32 close();
public:
    static void continueAfterDescribe(RTSPClient *pRtspClient, int32 nResultCode, char* pResultString);
    static void setupNextSubsession(RTSPClient* rtspClient);

private:
    /* input opt */
    RtspOpt 			m_stOpt;

    //
    TaskScheduler 		*m_pScheduler;
    UsageEnvironment 	*m_pEnv;

};





#endif // RTSPCLIENT_H
