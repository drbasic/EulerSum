#include "first_optimized_cpu.h"

#include <iostream>

std::vector<Solution>
FirstOptimizedCPU(const IndexNum elements_count, const std::vector<WorkNum>& powers) {
  std::vector<Solution> result;
  IndexNum rs = 5;
  for (IndexNum x0 = 1; x0 < elements_count; x0++) {
    for (IndexNum x1 = 1; x1 < x0; x1++) {
      for (IndexNum x2 = 1; x2 < x1; x2++) {
        WorkNum s2 = powers[x0] + powers[x1] + powers[x2];
        while (rs > 0 && powers[rs] > s2)
          --rs;
        for (IndexNum x3 = 1; x3 < x2; x3++) {
          WorkNum sum = s2 + powers[x3];
          while (rs < elements_count - 1 && powers[rs] < sum)
            ++rs;
          if (powers[rs] == sum) {
            std::cout << x0 << " " << x1 << " " << x2 << " " << x3 << " "
                      << pow(sum, 1.0 / 5.0) << std::endl;
          }
        }
      }
    }
  }
  return result;
}
