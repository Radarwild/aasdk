#pragma once

#include <cstdint>
#include <string>

namespace aasdk::messenger {

enum class FrameType {
  MIDDLE = 0,
  FIRST = 1 << 0,
  LAST = 1 << 1,
  BULK = FIRST | LAST
};

std::string frameTypeToString(FrameType frameType);

}
