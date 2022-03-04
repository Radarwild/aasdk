/*
*  This file is part of aasdk library project.
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

#include <aasdk_proto/ControlMessageIdsEnum.pb.h>
#include <aasdk_proto/PhoneStatusChannelMessageIdsEnum.pb.h>
#include <aasdk_proto/PhoneStatusIndicationMessage.pb.h>
#include <aasdk/Channel/PhoneStatus/IPhoneStatusServiceChannelEventHandler.hpp>
#include <aasdk/Channel/PhoneStatus/PhoneStatusServiceChannel.hpp>
#include <aasdk/Common/Log.hpp>


namespace aasdk
{
    namespace channel
    {
        namespace phonestatus
        {

            PhoneStatusServiceChannel::PhoneStatusServiceChannel(asio::io_service::strand& strand,  messenger::IMessenger::Pointer messenger)
                    : ServiceChannel(strand, std::move(messenger), messenger::ChannelId::PHONE_STATUS)
            {

            }

            void PhoneStatusServiceChannel::receive(IPhoneStatusServiceChannelEventHandler::Pointer eventHandler)
            {
                auto receivePromise = messenger::ReceivePromise::defer(strand_);
                receivePromise->then([this, self = this->shared_from_this(), eventHandler](messenger::Message::Pointer message) {
                                       this->messageHandler(std::move(message), eventHandler);},
                                     [&](const error::Error &e) { AASDK_LOG(ERROR) << "channel error: " << e.what()
                                                                                   << ", channel: " << static_cast<int>(this->getId()) << "\n"; });

                messenger_->enqueueReceive(channelId_, std::move(receivePromise));
            }

            messenger::ChannelId PhoneStatusServiceChannel::getId() const
            {
                return channelId_;
            }

            void PhoneStatusServiceChannel::sendChannelOpenResponse(const proto::messages::ChannelOpenResponse& response, SendPromise::Pointer promise)
            {
                auto message(std::make_shared<messenger::Message>(channelId_, messenger::EncryptionType::ENCRYPTED, messenger::MessageType::CONTROL));
                message->insertPayload(messenger::MessageId(proto::ids::ControlMessage::CHANNEL_OPEN_RESPONSE).getData());
                message->insertPayload(response);

                this->send(std::move(message), std::move(promise));
            }

            void PhoneStatusServiceChannel::messageHandler(messenger::Message::Pointer message, IPhoneStatusServiceChannelEventHandler::Pointer eventHandler)
            {
                messenger::MessageId messageId(message->getPayload());
                common::DataConstBuffer payload(message->getPayload(), messageId.getSizeOf());

                switch(messageId.getId())
                {
                    case proto::ids::PhoneStatusChannelMessage::PHONE_STATUS:
                        this->handlePhoneStatusMessage(payload, std::move(eventHandler));
                        break;
                    case proto::ids::PhoneStatusChannelMessage::PHONE_STATUS_INPUT:
                        AASDK_LOG(info) << "[PhoneStatusServiceChannel] PHONE_STATUS_INPUT";
                        break;
                    case proto::ids::ControlMessage::CHANNEL_OPEN_REQUEST:
                        this->handleChannelOpenRequest(payload, std::move(eventHandler));
                        break;
                    default:
                        AASDK_LOG(error) << "[PhoneStatusServiceChannel] message not handled: " << messageId.getId();
                        this->receive(std::move(eventHandler));
                        break;
                }
            }

            void PhoneStatusServiceChannel::handleChannelOpenRequest(const common::DataConstBuffer& payload, IPhoneStatusServiceChannelEventHandler::Pointer eventHandler)
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

            void PhoneStatusServiceChannel::handlePhoneStatusMessage(const common::DataConstBuffer& payload, IPhoneStatusServiceChannelEventHandler::Pointer eventHandler){
                proto::messages::PhoneStatus request;
                if(request.ParseFromArray(payload.cdata, payload.size))
                {
                    eventHandler->onPhoneStatusMessage(request);
                }
                else
                {
                    eventHandler->onChannelError(error::Error(error::ErrorCode::PARSE_PAYLOAD));
                }
            }

        }
    }
}
