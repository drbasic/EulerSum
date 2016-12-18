#include "antoncrechetov.h"

#include <algorithm>
#include <iostream>

std::vector<Solution>
AntonCrechetov(const IndexNum elements_count, const std::vector<WorkNum>& powers) {
  return AntonCrechetov2(elements_count, powers, 1, elements_count);
}

std::vector<Solution>
AntonCrechetov2(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                const IndexNum from, const IndexNum to) {
  std::vector<Solution> result;
  for (IndexNum a = from; a < to; ++a) {
    for (IndexNum b = a + 1; b < elements_count; ++b) {
      for (IndexNum c = b + 1; c < elements_count; ++c) {
        for (IndexNum d = c + 1; d < elements_count; ++d) {
          WorkNum sum = powers[a] + powers[b] + powers[c] + powers[d];
          if (std::binary_search(std::begin(powers), std::end(powers), sum)) {
            const auto it = std::lower_bound(std::begin(powers), std::end(powers), sum);
            IndexNum e = static_cast<IndexNum>(std::distance(std::begin(powers), it));
            result.emplace_back(Solution{a, b, c, d, e});
            std::cout << a << " " << b << " " << c << " " << d << " " << e << "\n";
          }
        }
      }
    }
  }
  return result;
}
