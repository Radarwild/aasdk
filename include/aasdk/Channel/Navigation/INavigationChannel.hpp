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

#pragma once

#include <aasdk_proto/NavigationChannelData.pb.h>
#include <aasdk_proto/NavigationChannelMessageIdsEnum.pb.h>
#include <aasdk_proto/NavigationDistanceMessage.pb.h>
#include <aasdk_proto/NavigationDistanceUnitEnum.pb.h>
#include <aasdk_proto/NavigationImageOptionsData.pb.h>
#include <aasdk_proto/NavigationRequestMessage.pb.h>
#include <aasdk_proto/NavigationRequestStateEnum.pb.h>
#include <aasdk_proto/NavigationTurnEventEnum.pb.h>
#include <aasdk_proto/NavigationTurnMessage.pb.h>
#include <aasdk_proto/NavigationTurnSideEnum.pb.h>
#include <aasdk/Common/Data.hpp>
#include <aasdk/Channel/Promise.hpp>
#include <aasdk/Channel/Navigation/INavigationChannelEventHandler.hpp>
#include <aasdk_proto/ChannelOpenResponseMessage.pb.h>



namespace aasdk::channel::navigation {

class INavigationChannel {
 public:
  typedef std::shared_ptr<INavigationChannel> Pointer;

  INavigationChannel() = default;
  virtual ~INavigationChannel() = default;

  virtual void receive(INavigationChannelEventHandler::Pointer eventHandler) = 0;
  virtual messenger::ChannelId getId() const = 0;
  virtual void sendChannelOpenResponse(const proto::messages::ChannelOpenResponse& response, SendPromise::Pointer promise) = 0;

};

}
