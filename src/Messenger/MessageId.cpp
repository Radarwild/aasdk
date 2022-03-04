#include <iomanip>
#include <endian.h>
#include <aasdk/Messenger/MessageId.hpp>


namespace aasdk
{
namespace messenger
{

MessageId::MessageId(uint16_t id):
    id_(id)
{

}
MessageId::MessageId(const common::Data& data)
{
  id_ = be16toh(reinterpret_cast<const uint16_t &>(data[0]));
}

uint16_t MessageId::getId() const
{
    return id_;
}

common::Data MessageId::getData() const
{
  const MessageId messageIdBig = htobe16(id_);
  const common::DataConstBuffer messageIdBigBuffer(&messageIdBig, sizeof(messageIdBig));
    return common::createData(messageIdBigBuffer);
}

bool MessageId::operator>(uint16_t id) const
{
    return id > id_;
}

bool MessageId::operator<(uint16_t id) const
{
    return id < id_;
}

bool MessageId::operator==(uint16_t id) const
{
    return id == id_;
}

MessageId& MessageId::operator=(uint16_t id)
{
    id_ = id;
    return *this;
}

bool MessageId::operator>=(uint16_t id) const
{
    return id >= id_;
}

bool MessageId::operator<=(uint16_t id) const
{
    return id <= id_;
}

std::ostream& operator<<(std::ostream& stream, const aasdk::messenger::MessageId& messageId)
{
    stream << "0x" << std::hex << messageId.getId() << std::dec;
    return stream;
}

}
}
