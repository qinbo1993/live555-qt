#include "StreamClientState.h"

StreamClientState::StreamClientState()
  : m_pIter(NULL), m_pSession(NULL), m_pSubSession(NULL), m_StreamTimerTask(NULL), m_dDuration(0.0) {
}

StreamClientState::~StreamClientState() {
  delete m_pIter;
  if (m_pSession != NULL) {
    // We also need to delete "session", and unschedule "streamTimerTask" (if set)
    UsageEnvironment& env = m_pSession->envir(); // alias

    env.taskScheduler().unscheduleDelayedTask(m_StreamTimerTask);
    Medium::close(m_pSession);
  }
}
