/*
*  This file is part of aasdk library project.
*  Copyright (C) 2021 silverchris (Chris Schimp)
*  Copyright (C) 2018 f1x.studio (Michal Szwaj)
*
*  aasdk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 3 of the License, or
*  (at your option) any later version.

*  aasdk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with aasdk. If not, see <http://www.gnu.org/licenses/>.
*/

#include <endian.h>
#include <aasdk_proto/NavigationChannelMessageIdsEnum.pb.h>
#include <aasdk/Version.hpp>
#include <aasdk/IO/PromiseLink.hpp>
#include <aasdk/Channel/Navigation/NavigationChannel.hpp>
#include <aasdk/Channel/Navigation/INavigationChannelEventHandler.hpp>
#include <aasdk/Common/Log.hpp>
#include <aasdk_proto/ControlMessageIdsEnum.pb.h>

namespace aasdk::channel::navigation {

NavigationChannel::NavigationChannel(asio::io_service::strand &strand, messenger::IMessenger::Pointer messenger)
    : ServiceChannel(strand, messenger, messenger::ChannelId::NAVIGATION) {

}

void NavigationChannel::receive(INavigationChannelEventHandler::Pointer eventHandler) {
  auto receivePromise = messenger::ReceivePromise::defer(strand_);
  receivePromise->then([this, self = this->shared_from_this(), eventHandler](messenger::Message::Pointer message) {
                         this->messageHandler(std::move(message), eventHandler);
                       },
                       [&](const error::Error &e) { AASDK_LOG(ERROR) << "channel error: " << e.what()
                                                                     << ", channel: " << static_cast<int>(this->getId()) << "\n"; });

  messenger_->enqueueReceive(channelId_, std::move(receivePromise));
}

messenger::ChannelId NavigationChannel::getId() const
{
  return channelId_;
}

void NavigationChannel::messageHandler(messenger::Message::Pointer message,
                                       INavigationChannelEventHandler::Pointer eventHandler) {
  messenger::MessageId messageId(message->getPayload());
  common::DataConstBuffer payload(message->getPayload(), messageId.getSizeOf());

  switch (messageId.getId()) {
    case proto::ids::NavigationChannelMessage::NAVIGATION_STATUS:
      this->handleNavigationStatus(payload,
                                   std::move(eventHandler));
      break;
    case proto::ids::NavigationChannelMessage::NAVIGATION_TURN_EVENT:
      this->handleNavigationTurn(payload,
                                 std::move(eventHandler));
      break;
    case proto::ids::NavigationChannelMessage::NAVIGATION_DISTANCE_EVENT:
      this->handleNavigationDistance(payload,
                                     std::move(eventHandler));
      break;
    case proto::ids::ControlMessage::CHANNEL_OPEN_REQUEST:
      this->handleChannelOpenRequest(payload, std::move(eventHandler));
      break;
    default:AASDK_LOG(error) << "[NavigationChannel] message not handled: " << messageId.getId();
      this->receive(std::move(eventHandler));
      break;
  }
}

void NavigationChannel::handleNavigationStatus(const common::DataConstBuffer &payload,
                                               INavigationChannelEventHandler::Pointer eventHandler) {
  proto::messages::NavigationRequestMessage request;
  if (request.ParseFromArray(payload.cdata, payload.size)) {
    eventHandler->onNavigationStatus(request);
  } else {
    eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
  }
}

void NavigationChannel::handleNavigationTurn(const common::DataConstBuffer &payload,
                                             INavigationChannelEventHandler::Pointer eventHandler) {
  proto::messages::NavigationTurnMessage request;
  if (request.ParseFromArray(payload.cdata, payload.size)) {
    eventHandler->onNavigationTurn(request);
  } else {
    eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
  }
}

void NavigationChannel::handleNavigationDistance(const common::DataConstBuffer &payload,
                                                 INavigationChannelEventHandler::Pointer eventHandler) {
  proto::messages::NavigationDistanceMessage request;
  if (request.ParseFromArray(payload.cdata, payload.size)) {
    eventHandler->onNavigationDistance(request);
  } else {
    eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
  }
}

void NavigationChannel::handleChannelOpenRequest(const common::DataConstBuffer& payload, INavigationChannelEventHandler::Pointer eventHandler)
{
  proto::messages::ChannelOpenRequest request;
  if(request.ParseFromArray(payload.cdata, payload.size))
  {
    eventHandler->onChannelOpenRequest(request);
  }
  else
  {
    eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
  }
}

void NavigationChannel::sendChannelOpenResponse(const proto::messages::ChannelOpenResponse& response, SendPromise::Pointer promise)
{
  auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::CONTROL));
  message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::CHANNEL_OPEN_RESPONSE).getData());
  message->insertPayload(response);

  this->send(std::move(message), std::move(promise));
}

}
