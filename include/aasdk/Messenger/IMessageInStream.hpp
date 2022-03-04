#pragma once

#include <memory>
#include <aasdk/Messenger/Promise.hpp>


namespace aasdk::messenger {

class IMessageInStream {
 public:
  typedef std::shared_ptr<IMessageInStream> Pointer;

  IMessageInStream() = default;
  virtual ~IMessageInStream() = default;

    virtual void startReceive(ReceivePromise::Pointer promise) = 0;
};

}
