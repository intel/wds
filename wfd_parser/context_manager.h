#ifndef CONTEXT_MANAGER_H_
#define CONTEXT_MANAGER_H_

#include <string>

namespace wfd {

class MediaManager {
 public:  
  virtual ~MediaManager() {}
  virtual void Play() = 0;
  virtual void Pause() = 0;
  virtual void Teardown() = 0;
  virtual bool IsPaused() const = 0;
  virtual void SetRtpPorts(int port1, int port2) = 0;
};

}  // namespace wfd

#endif // CONTEXT_MANAGER_H_

