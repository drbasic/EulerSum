#pragma once

#include <stdint.h>
#include <vector>

using WorkNum = uint64_t;
using IndexNum = uint32_t;

struct Solution {
  IndexNum a;
  IndexNum b;
  IndexNum c;
  IndexNum d;
  IndexNum e;
};

using CalcFunction = std::vector<Solution>(const IndexNum elements_count,
                                           const std::vector<WorkNum>& powers);

using CalcFunction2 = std::vector<Solution>(const IndexNum elements_count,
                                            const std::vector<WorkNum>& powers,
                                            const IndexNum from,
                                            const IndexNum to);

