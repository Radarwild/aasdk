#include <endian.h>
#include <aasdk/Messenger/Timestamp.hpp>


namespace aasdk
{
namespace messenger
{

Timestamp::Timestamp(ValueType stamp)
    : stamp_(stamp)
{

}

Timestamp::Timestamp(const common::DataConstBuffer& buffer)
{
    const ValueType& timestampBig = reinterpret_cast<const ValueType&>(buffer.cdata[0]);
  stamp_ = be64toh(timestampBig);
}

common::Data Timestamp::getData() const
{
  const ValueType timestampBig = be64toh(stamp_);
  const common::DataConstBuffer timestampBuffer(&timestampBig, sizeof(timestampBig));
    return common::createData(timestampBuffer);
}

Timestamp::ValueType Timestamp::getValue() const
{
    return stamp_;
}

}
}
