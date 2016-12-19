#include "second_optimized_cpu.h"

#include <iostream>

std::vector<Solution>
SecondOptimizedCPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                   const IndexNum from, const IndexNum to)
{
  std::vector<Solution> result;
  IndexNum rs = 5;
  for (IndexNum x0 = from; x0 < to; x0++) {
    for (IndexNum x1 = 1; x1 < x0; x1++) {
      for (IndexNum x2 = 1; x2 < x1; x2++) {
        WorkNum s2 = powers[x0] + powers[x1] + powers[x2];
        while (rs > 0 && powers[rs] > s2)
          --rs;
        for (IndexNum x3 = 1; x3 < x2; x3++) {
          // go straight to the first appropriate x3, mod 30
          if (IndexNum err30 = (x0 + x1 + x2 + x3 - rs) % 30)
            x3 += 30 - err30;
          if (x3 >= x2)
            break;
          auto sum = s2 + powers[x3];
          while (rs < elements_count - 1 && powers[rs] < sum)
            ++rs;
          if (powers[rs] == sum) {
            result.emplace_back(Solution{ x0, x1, x2, x3, rs });
            // std::cout << x0 << " " << x1 << " " << x2 << " " << x3 << " " << rs << std::endl;
          }
        }
      }
    }
  }
  return result;
}
