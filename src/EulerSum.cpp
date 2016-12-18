#include "EulerSum.h"

#include <chrono>
#include <iostream>
#include <string>

#include "antoncrechetov.h"
#include "first_optimized_cpu.h"
#include "gpu.h"
#include "naive_cpu.h"
#include "second_optimized_cpu.h"

constexpr IndexNum elements_count = 250;

void MeasureTime(const std::string& name,
                 const std::vector<WorkNum>& powers,
                 CalcFunction calc_function) {
  std::cout << "<<< " << name << " >>> \n";
  auto started_at = std::chrono::steady_clock::now();
  calc_function(elements_count, powers);
  auto finished_at = std::chrono::steady_clock::now();
  std::cout << "Time: "
            << std::chrono::duration<double, std::milli>(finished_at - started_at).count()
            << "ms\n";
}

inline WorkNum pow5(const IndexNum n) {
  const WorkNum pow2 = n * n;
  return pow2 * pow2 * n;
}

int main() {
  std::vector<WorkNum> powers(elements_count);
  for (IndexNum i = 0; i < elements_count; ++i) {
    powers[i] = pow5(i);
  }

  std::cout << "Euler sum for N=" << elements_count << "\n";
  MeasureTime("Anton Crechetov", powers, &AntonCrechetov);
  MeasureTime("Naive CPU", powers, &NaiveCPU);
  for (int i = 0; i < 1; ++i)
    MeasureTime("Naive GPU", powers, &NaiveGPU);
  MeasureTime("First optimized CPU", powers, &FirstOptimizedCPU);
  for (int i = 0; i < 1; ++i)
    MeasureTime("First optimized GPU", powers, &FirstOptimizedGPU);
  MeasureTime("Second optimized CPU", powers, &SecondOptimizedCPU);
  for (int i = 0; i < 5; ++i)
    MeasureTime("Second optimized GPU", powers, &SecondOptimizedGPU);

  return 0;
}

