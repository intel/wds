#ifndef MIRAC_BROKER_SOURCE_H_
#define MIRAC_BROKER_SOURCE_H_

#include <memory>

#include "mirac-broker.hpp"

namespace wfd {
class SourceMediaManager;
class Source;
}

class MiracBrokerSource : public MiracBroker {
 public:
  explicit MiracBrokerSource(int rtsp_port);
  ~MiracBrokerSource();

  wfd::Source* wfd_source() { return wfd_source_.get(); }

 private:
  virtual void got_message(const std::string& message) override;
  virtual void on_connected() override;

  std::unique_ptr<wfd::SourceMediaManager> media_manager_;
  std::unique_ptr<wfd::Source> wfd_source_;
};

#endif // MIRAC_BROKER_SOURCE_H_
