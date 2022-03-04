#pragma once

#include <aasdk/Common/Data.hpp>
#include <aasdk/Messenger/FrameSizeType.hpp>


namespace aasdk
{
namespace messenger
{

class FrameSize
{
public:
    FrameSize(size_t frameSize, size_t totalSize);
    FrameSize(size_t frameSize);
    FrameSize(const common::DataConstBuffer& buffer);

    common::Data getData() const;
    //size_t getSize() const;
    size_t getFrameSize() const;
    size_t getTotalSize() const;
	
    static size_t getSizeOf(FrameSizeType type);

private:
    FrameSizeType frameSizeType_;
    size_t frameSize_;
    size_t totalSize_;
};

}
}
