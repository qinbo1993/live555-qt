#ifndef OURRTSPCLIENT_H
#define OURRTSPCLIENT_H

#include "liveMedia.hh"
#include "UsageEnvironment.hh"
#include "BasicUsageEnvironment.hh"
#include "Groupsock.hh"
#include "StreamClientState.h"


class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
                  int verbosityLevel = 0,
                  char const* applicationName = NULL,
                  portNumBits tunnelOverHTTPPortNum = 0);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
        int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
};

#endif // OURRTSPCLIENT_H
