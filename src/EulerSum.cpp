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

constexpr size_t thread_count = 4;

std::vector<Solution>
RunOnWorkerPool(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                CalcFunction calc_function) {
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

inline WorkNum pow5(const IndexNum n) {
  const WorkNum pow2 = n * n;
  return pow2 * pow2 * n;
}

void MeasureTime(const std::string& name,
                 const IndexNum elements_count,
                 CalcFunction calc_function,
                 const bool multi_thread) {
  std::cout << "Euler sum for N=" << elements_count;
  if (multi_thread)
    std::cout << " threads=" << thread_count;
  std::cout << " <<< " << name << " >>> \n";

  std::vector<WorkNum> powers(elements_count);
  for (IndexNum i = 0; i < elements_count; ++i) {
    powers[i] = pow5(i);
  }

  auto started_at = std::chrono::steady_clock::now();
  if (multi_thread) {
    RunOnWorkerPool(elements_count, powers, calc_function);
  } else {
    calc_function(elements_count, powers, 1, elements_count);
  }
  auto finished_at = std::chrono::steady_clock::now();
  std::cout << "Time: "
            << std::chrono::duration<double, std::milli>(finished_at - started_at).count()
            << "ms\n";
}

struct TestCase {
  std::string name;
  CalcFunction* calc_function;
  bool multi_tread;
  size_t run_count;
  IndexNum max_n;
};

int main() {
  const IndexNum test_Ns[] = { 250, 500, 750, 1000, 1250, 1500 };

  const TestCase test_cases[] = {
    TestCase{ "Anton Crechetov", &AntonCrechetov, false, 2, 750 },
    TestCase{ "Anton Crechetov", &AntonCrechetov, true, 2, 750 },
    TestCase{ "Naive CPU", &NaiveCPU, false, 2, 750  },
    TestCase{ "Naive CPU", &NaiveCPU, true, 2, 750  },
    TestCase{ "First optimized CPU", &FirstOptimizedCPU, false, 2, 750 },
    TestCase{ "First optimized CPU", &FirstOptimizedCPU, true, 2, 750 },
    TestCase{ "Second optimized CPU", &SecondOptimizedCPU, false, 2 },
    TestCase{ "Second optimized CPU", &SecondOptimizedCPU, true, 2 },
    TestCase{ "Second optimized GPU", &NaiveGPU, false, 5 },
    TestCase{ "Second optimized GPU", &FirstOptimizedGPU, false, 5 },
    TestCase{ "Second optimized GPU", &SecondOptimizedGPU, false, 5 },
  };

  for (auto n : test_Ns) {
    for (const TestCase& test_case : test_cases) {
      if (test_case.max_n > n)
        continue;
      for (int i = 0; i < test_case.run_count; ++i) {
        MeasureTime(test_case.name,
                    n,
                    test_case.calc_function,
                    test_case.multi_tread);
      }
    }
  }

  return 0;
}

