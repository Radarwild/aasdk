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

#include <aasdk/TCP/ITCPWrapper.hpp>


namespace aasdk
{
namespace tcp
{

class TCPWrapper: public ITCPWrapper
{
public:
    void asyncWrite(asio::ip::tcp::socket& socket, common::DataConstBuffer buffer, Handler handler) override;
    void asyncRead(asio::ip::tcp::socket& socket, common::DataBuffer buffer, Handler handler) override;
    void close(asio::ip::tcp::socket& socket) override;
    void asyncConnect(asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port, ConnectHandler handler) override;
    asio::error_code connect(asio::ip::tcp::socket& socket, const std::string& hostname, uint16_t port) override;
};

}
}
