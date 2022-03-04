#pragma once

#include <map>
#include <aasdk/Transport/ITransport.hpp>
#include <aasdk/Messenger/IMessageInStream.hpp>
#include <aasdk/Messenger/ICryptor.hpp>
#include <aasdk/Messenger/FrameHeader.hpp>
#include <aasdk/Messenger/FrameSize.hpp>
#include <aasdk/Messenger/FrameType.hpp>

namespace aasdk
{
namespace messenger
{

class MessageInStream: public IMessageInStream, public std::enable_shared_from_this<MessageInStream>
{
public:
    MessageInStream(asio::io_service& ioService, transport::ITransport::Pointer transport, ICryptor::Pointer cryptor);

    void startReceive(ReceivePromise::Pointer promise) override;

private:
    using std::enable_shared_from_this<MessageInStream>::shared_from_this;

    void receiveFrameHeaderHandler(const common::DataConstBuffer& buffer);
    void receiveFrameSizeHandler(const common::DataConstBuffer& buffer);
    void receiveFramePayloadHandler(const common::DataConstBuffer& buffer);

    asio::io_service::strand strand_;
    transport::ITransport::Pointer transport_;
    ICryptor::Pointer cryptor_;
    //FrameType recentFrameType_;
	FrameType thisFrameType_;
    ReceivePromise::Pointer promise_;
	ReceivePromise::Pointer interleavedPromise_;
    Message::Pointer message_;
	
    std::map<messenger::ChannelId, Message::Pointer> messageBuffer_;
	
	int frameSize_;
    bool isValidFrame_;
	int currentMessageIndex_;
};

}
}
