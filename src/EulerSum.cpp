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
using time_diff = std::chrono::duration<double, std::milli>;

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

time_diff MeasureTime(const IndexNum elements_count,
                      CalcFunction calc_function,
                      const bool multi_thread) {
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
  return std::chrono::steady_clock::now() - started_at;
}

struct TestCase {
  std::string name;
  CalcFunction* calc_function;
  bool multi_tread;
  size_t run_count;
  IndexNum max_n;
};

int main() {
  const IndexNum test_Ns[] = { 100, 250, 500 };//, 750, 1000, 1250, 1500 };

  const TestCase test_cases[] = {
    TestCase{ "Anton Crechetov",      &AntonCrechetov,     false, 2,  750 },
    TestCase{ "Anton Crechetov",      &AntonCrechetov,     true,  2,  750 },
    TestCase{ "Naive CPU",            &NaiveCPU,           false, 2,  750 },
    TestCase{ "Naive CPU",            &NaiveCPU,           true,  2,  750 },
    TestCase{ "First optimized CPU",  &FirstOptimizedCPU,  false, 2,  750 },
    TestCase{ "First optimized CPU",  &FirstOptimizedCPU,  true,  2,  750 },
    TestCase{ "Second optimized CPU", &SecondOptimizedCPU, false, 2,  5000 },
    TestCase{ "Second optimized CPU", &SecondOptimizedCPU, true,  2,  5000 },

    TestCase{ "Second optimized GPU", &NaiveGPU,           false, 7, 750 },
    TestCase{ "Second optimized GPU", &FirstOptimizedGPU,  false, 7, 750 },
    TestCase{ "Second optimized GPU", &SecondOptimizedGPU, false, 7, 5000 },
  };

  for (auto elements_count : test_Ns) {
    for (const TestCase& test_case : test_cases) {
      if (test_case.max_n < elements_count)
        continue;

      std::cout << " <<< " << test_case.name << " >>>\n";
      std::cout << "  N=" << elements_count;
      if (test_case.multi_tread)
        std::cout << " threads=" << thread_count;
      std::cout << "\n" << std::flush;

      std::vector<time_diff> durations;
      for (int i = 0; i < test_case.run_count; ++i) {
        auto duration = MeasureTime(elements_count,
                                    test_case.calc_function,
                                    test_case.multi_tread);
        durations.push_back(duration);
        std::cout << "    Run " << i + 1 << " time: " << duration.count() << "ms\n" << std::flush;
      }
      auto it = std::min_element(std::begin(durations), std::end(durations));
      std::cout << "  Best time: " << it->count() << "ms\n" << std::flush;
    }
  }

  return 0;
}

