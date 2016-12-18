#include "antoncrechetov.h"

#include <algorithm>
#include <iostream>

std::vector<Solution>
AntonCrechetov(const IndexNum elements_count, const std::vector<WorkNum>& powers) {
  std::vector<Solution> result;
  for (IndexNum a = 1; a < elements_count; ++a) {
    for (IndexNum b = a + 1; b < elements_count; ++b) {
      for (IndexNum c = b + 1; c < elements_count; ++c) {
        for (IndexNum d = c + 1; d < elements_count; ++d) {
          WorkNum sum = powers[a] + powers[b] + powers[c] + powers[d];
          if (std::binary_search(std::begin(powers), std::end(powers), sum)) {
            const auto it = std::lower_bound(std::begin(powers), std::end(powers), sum);
            std::cout << a << " " << b << " " << c << " " << d << " " 
                      << std::distance(std::begin(powers), it) << std::endl;
          }
        }
      }
    }
  }
  return result;
}
