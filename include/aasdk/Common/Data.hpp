#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <cstddef>
#include <cstdint>


namespace aasdk::common {

typedef std::vector<uint8_t> Data;

static constexpr size_t cStaticDataSize = 30 * 1024 * 1024;

struct DataBuffer {
  DataBuffer();
  DataBuffer(Data::value_type *_data, Data::size_type _size, Data::size_type offset = 0);
    DataBuffer(void* _data, Data::size_type _size, Data::size_type offset = 0);
    explicit DataBuffer(Data& _data, Data::size_type offset = 0);
    bool operator==(const std::nullptr_t&) const;
    bool operator==(const DataBuffer& buffer) const;

    Data::value_type* data;
    Data::size_type size;
};

struct DataConstBuffer
{
    DataConstBuffer();
    explicit DataConstBuffer(const DataBuffer& other);
    DataConstBuffer(const Data::value_type* _data, Data::size_type _size, Data::size_type offset = 0);
    DataConstBuffer(const void* _data, Data::size_type _size, Data::size_type offset = 0);
    explicit DataConstBuffer(const Data& _data, Data::size_type offset = 0);
    bool operator==(const std::nullptr_t&) const;
    bool operator==(const DataConstBuffer& buffer) const;

    const Data::value_type* cdata;
    Data::size_type size;
};

template<typename DataType>
void copy(DataType& data, const DataBuffer& buffer)
{
    size_t offset = data.size();
    data.resize(data.size() + buffer.size);
    memcpy(&data[offset], buffer.data, buffer.size);
}

template<typename DataType>
void copy(DataType& data, const DataConstBuffer& buffer)
{
    size_t offset = data.size();
    data.resize(data.size() + buffer.size);
    memcpy(&data[offset], buffer.cdata, buffer.size);
}

common::Data createData(const DataConstBuffer& buffer);

std::string dump(const Data& data);
std::string dump(const DataConstBuffer& buffer);

}
