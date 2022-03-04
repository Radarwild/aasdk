#include <endian.h>
#include <aasdk/Messenger/FrameSize.hpp>


namespace aasdk
{
namespace messenger
{

FrameSize::FrameSize(size_t frameSize, size_t totalSize)
    : frameSizeType_(FrameSizeType::EXTENDED)
    , frameSize_(frameSize)
    , totalSize_(totalSize)
{

}

FrameSize::FrameSize(size_t frameSize)
    : frameSizeType_(FrameSizeType::SHORT)
    , frameSize_(frameSize)
    , totalSize_(0)
{

}

FrameSize::FrameSize(const common::DataConstBuffer& buffer)
{
    if(buffer.size >= 2)
    {
      frameSizeType_ = FrameSizeType::SHORT;
      frameSize_ = be16toh(reinterpret_cast<const uint16_t &>(buffer.cdata[0]));
    }

    if(buffer.size >= 6)
    {
      frameSizeType_ = FrameSizeType::EXTENDED;
      totalSize_ = be16toh(reinterpret_cast<const uint32_t &>(buffer.cdata[2]));
    }
}

common::Data FrameSize::getData() const
{
    common::Data data;

  uint16_t frameSizeBig = htobe16(static_cast<uint16_t>(frameSize_));
  const common::DataConstBuffer frameSizeBigBuffer(&frameSizeBig, sizeof(frameSizeBig));
    common::copy(data, frameSizeBigBuffer);

    if(frameSizeType_ == FrameSizeType::EXTENDED)
    {
      uint32_t totalSizeBig = htobe16(static_cast<uint32_t>(totalSize_));
      const common::DataConstBuffer totalSizeBigBuffer(&totalSizeBig, sizeof(totalSizeBig));
        common::copy(data, totalSizeBigBuffer);
    }

    return data;
}

//size_t FrameSize::getSize() const
size_t FrameSize::getFrameSize() const
{
    return frameSize_;
}

size_t FrameSize::getTotalSize() const
{
    return totalSize_;
}

size_t FrameSize::getSizeOf(FrameSizeType type)
{
    return type == FrameSizeType::EXTENDED ? 6 : 2;
}

}
}
