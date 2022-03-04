#include <aasdk/Error/Error.hpp>
#include <aasdk/Messenger/Messenger.hpp>
#include <utility>
#include <aasdk/Common/Log.hpp>

namespace aasdk
{
namespace messenger
{

Messenger::Messenger(asio::io_service& ioService, IMessageInStream::Pointer messageInStream, IMessageOutStream::Pointer messageOutStream)
    : receiveStrand_(ioService)
    , sendStrand_(ioService)
    , messageInStream_(std::move(messageInStream))
    , messageOutStream_(std::move(messageOutStream))
{

}

void Messenger::enqueueReceive(ChannelId channelId, ReceivePromise::Pointer promise)
{
    receiveStrand_.dispatch([this, self = this->shared_from_this(), channelId, promise = std::move(promise)]() mutable {
        if(!channelReceiveMessageQueue_.empty(channelId))
        {
            promise->resolve(channelReceiveMessageQueue_.pop(channelId));
        }
        else
        {
            channelReceivePromiseQueue_.push(channelId, std::move(promise));

            if(channelReceivePromiseQueue_.size() == 1)
            {
                auto inStreamPromise = ReceivePromise::defer(receiveStrand_);
                inStreamPromise->then([this, self = this->shared_from_this()](Message::Pointer message){inStreamMessageHandler(std::move(message));},
                                     [this, self = this->shared_from_this()](const error::Error &e){rejectReceivePromiseQueue(e);});
                messageInStream_->startReceive(std::move(inStreamPromise));
            }
        }
    });
}

void Messenger::enqueueSend(Message::Pointer message, SendPromise::Pointer promise)
{
    sendStrand_.dispatch([this, self = this->shared_from_this(), message = std::move(message), promise = std::move(promise)]() mutable {
        channelSendPromiseQueue_.emplace_back(std::make_pair(std::move(message), std::move(promise)));

        if(channelSendPromiseQueue_.size() == 1)
        {
            this->doSend();
        }
    });
}

void Messenger::inStreamMessageHandler(Message::Pointer message)
{
    auto channelId = message->getChannelId();

    if(channelReceivePromiseQueue_.isPending(channelId))
    {
        channelReceivePromiseQueue_.pop(channelId)->resolve(std::move(message));
    }
    else
    {
        channelReceiveMessageQueue_.push(std::move(message));
    }

    if(!channelReceivePromiseQueue_.empty())
    {
        auto inStreamPromise = ReceivePromise::defer(receiveStrand_);
        inStreamPromise->then([this, self = this->shared_from_this()](Message::Pointer message){inStreamMessageHandler(std::move(message));},
                              [this, self = this->shared_from_this()](const error::Error &e){rejectReceivePromiseQueue(e);});
        messageInStream_->startReceive(std::move(inStreamPromise));
    }
}

void Messenger::doSend()
{
    auto queueElementIter = channelSendPromiseQueue_.begin();
    auto outStreamPromise = SendPromise::defer(sendStrand_);
    outStreamPromise->then([this, self = this->shared_from_this(), queueElementIter](){outStreamMessageHandler(queueElementIter);},
                           [this, self = this->shared_from_this()](const error::Error &e){rejectSendPromiseQueue(e);});

    messageOutStream_->stream(std::move(queueElementIter->first), std::move(outStreamPromise));
}

void Messenger::outStreamMessageHandler(ChannelSendQueue::iterator queueElement)
{
    queueElement->second->resolve();
    channelSendPromiseQueue_.erase(queueElement);

    if(!channelSendPromiseQueue_.empty())
    {
        this->doSend();
    }
}

void Messenger::rejectReceivePromiseQueue(const error::Error& e)
{
    while(!channelReceivePromiseQueue_.empty())
    {
        channelReceivePromiseQueue_.pop()->reject(e);
    }
}

void Messenger::rejectSendPromiseQueue(const error::Error& e)
{
    while(!channelSendPromiseQueue_.empty())
    {
        auto queueElement(std::move(channelSendPromiseQueue_.front()));
        channelSendPromiseQueue_.pop_front();
        queueElement.second->reject(e);
    }
}

void Messenger::stop()
{
    receiveStrand_.dispatch([this, self = this->shared_from_this()]() {
        channelReceiveMessageQueue_.clear();
    });
}

}
}
