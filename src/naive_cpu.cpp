#include "naive_cpu.h"

#include <algorithm>
#include <iostream>

std::vector<Solution>
NaiveCPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
         const IndexNum from, const IndexNum to) {
  std::vector<Solution> result;
  for (IndexNum a = from; a < to; ++a) {
    for (IndexNum b = 1; b < a; ++b) {
      for (IndexNum c = 1; c < b; ++c) {
        const uint64_t sum_abc = powers[a] + powers[b] + powers[c];
        for (IndexNum d = 1; d < c; ++d) {
          const uint64_t sum = sum_abc + powers[d];
          if (std::binary_search(std::begin(powers), std::end(powers), sum)) {
            const auto it = std::lower_bound(std::begin(powers), std::end(powers), sum);
            result.emplace_back(Solution{ a, b, c, d, static_cast<IndexNum>(std::distance(std::begin(powers), it)) });
            //std::cout << a << " " << b << " " << c << " " << d << " " 
            //          << std::distance(std::begin(powers), it) << std::endl;
          }
        }
      }
    }
  }
  return result;
}
