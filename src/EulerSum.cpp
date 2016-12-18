#include "EulerSum.h"

#include <chrono>
#include <iostream>
#include <string>

#include "antoncrechetov.h"
#include "first_optimized_cpu.h"
#include "gpu.h"
#include "naive_cpu.h"
#include "second_optimized_cpu.h"
#include "thread_pool.h"

constexpr IndexNum elements_count = 500;
constexpr size_t thread_count = 4;

std::vector<Solution>
RunOnWorkerPool(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                CalcFunction2 calc_function) {
  std::vector<Solution> result;
  std::mutex result_guard;
  ThreadPool pool(thread_count);
  for (IndexNum i = 1; i < elements_count; ++i) {
    auto f = [i, elements_count, &powers, &calc_function, &result, &result_guard] () {
      auto r = calc_function(elements_count, powers, i, i + 1);
      if (!r.empty()) {
        std::unique_lock<std::mutex> lock(result_guard);
        result.insert(result.begin(), r.begin(), r.end());
      }
    };
    pool.enqueue(f);
  }
  pool.wait_for_idle();
  return result;
}

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

void MeasureTime(const std::string& name,
                 const std::vector<WorkNum>& powers,
                 CalcFunction2 calc_function) {
  std::cout << "<<< " << name << " mt >>> \n";
  auto started_at = std::chrono::steady_clock::now();
  RunOnWorkerPool(elements_count, powers, calc_function);
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
  MeasureTime("Anton Crechetov", powers, &AntonCrechetov2);
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

