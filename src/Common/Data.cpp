#include <boost/algorithm/hex.hpp>
#include <aasdk/Common/Data.hpp>
#include <aasdk/Common/Log.hpp>
#include <string>
#include <sstream>
#include <iomanip>

namespace aasdk
{
namespace common
{

DataBuffer::DataBuffer()
    : data(nullptr)
    , size(0)
{

}

DataBuffer::DataBuffer(Data::value_type* _data, Data::size_type _size, Data::size_type offset)
{
    if(offset > _size || _data == nullptr || _size == 0)
    {
        data = nullptr;
        size = 0;
    }
    else if(offset <= _size)
    {
        data = _data + offset;
        size = _size - offset;
    }
}

DataBuffer::DataBuffer(void* _data, Data::size_type _size, Data::size_type offset)
    : DataBuffer(reinterpret_cast<Data::value_type*>(_data), _size, offset)
{

}

DataBuffer::DataBuffer(Data& _data, Data::size_type offset)
    : DataBuffer(_data.empty() ? nullptr : &_data[0], _data.size(), offset > _data.size() ? 0 : offset)
{

}

bool DataBuffer::operator==(const std::nullptr_t&) const
{
    return data == nullptr || size == 0;
}

bool DataBuffer::operator==(const DataBuffer& buffer) const
{
    return data == buffer.data && size == buffer.size;
}

DataConstBuffer::DataConstBuffer()
    : cdata(nullptr)
    , size(0)
{

}

DataConstBuffer::DataConstBuffer(const DataBuffer& other)
    : cdata(other.data)
    , size(other.size)
{
}

DataConstBuffer::DataConstBuffer(const Data::value_type* _data, Data::size_type _size, Data::size_type offset)
{
    if(offset > _size || _data == nullptr || _size == 0)
    {
        cdata = nullptr;
        size = 0;
    }
    else if(offset <= _size)
    {
        cdata = _data + offset;
        size = _size - offset;
    }
}

DataConstBuffer::DataConstBuffer(const void* _data, Data::size_type _size, Data::size_type offset)
    : DataConstBuffer(reinterpret_cast<const Data::value_type*>(_data), _size, offset)
{

}

DataConstBuffer::DataConstBuffer(const Data& _data, Data::size_type offset)
    : DataConstBuffer(_data.empty() ? nullptr : &_data[0], _data.size(), offset > _data.size() ? 0 : offset)
{

}

bool DataConstBuffer::operator==(const std::nullptr_t&) const
{
    return cdata == nullptr || size == 0;
}

bool DataConstBuffer::operator==(const DataConstBuffer& buffer) const
{
    return cdata == buffer.cdata && size == buffer.size;
}

common::Data createData(const DataConstBuffer& buffer)
{
    common::Data data;
    copy(data, buffer);
    return data;
}

std::string dump(const Data& data)
{
    //std::string buffer;
    //boost::algorithm::hex(data, back_inserter(buffer));
    //return buffer;
    return dump(DataConstBuffer(data));
}

/*std::string uint8_to_hex_string(const uint8_t *v, const size_t s) {
  std::stringstream ss;

  ss << std::hex << std::setfill('0');

  for (int i = 0; i < s; i++) {
    ss << " ";
    ss << std::hex << std::setw(2) << static_cast<int>(v[i]);
  }

  return ss.str();
}*/

std::string dump(const DataConstBuffer& buffer)
{
    if(buffer.size == 0)
    {
        return "[0] null";
    }
    else
    {
        std::string hexDump = "[" + std::to_string(buffer.size) + "] ";
        //std::string hexDump = "[" + uint8_to_hex_string(buffer.cdata, buffer.size) + " ] ";
        //boost::algorithm::hex(bufferBegin(buffer), bufferEnd(buffer), back_inserter(hexDump));
        return hexDump;
    }
}

}
}
