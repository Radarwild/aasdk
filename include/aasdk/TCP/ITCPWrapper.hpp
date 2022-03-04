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

#pragma once

#include <functional>
#include <asio/ip/tcp.hpp>
#include <aasdk/Common/Data.hpp>

namespace aasdk::tcp {

class ITCPWrapper {
 public:
  typedef std::function<void(const asio::error_code &, size_t)> Handler;
  typedef std::function<void(const asio::error_code &)> ConnectHandler;

  virtual ~ITCPWrapper() = default;

    virtual void asyncWrite(asio::ip::tcp::socket& socket, common::DataConstBuffer buffer, Handler handler) = 0;
    virtual void asyncRead(asio::ip::tcp::socket& socket, common::DataBuffer buffer, Handler handler) = 0;
    virtual void close(asio::ip::tcp::socket& socket) = 0;
    virtual void asyncConnect(asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port, ConnectHandler handler) = 0;
    virtual asio::error_code connect(asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port) = 0;
};

}
