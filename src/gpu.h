#pragma once

#include <vector>

#include "EulerSum.h"

std::vector<Solution>
AntonCrechetovGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                  const IndexNum from, const IndexNum to);

std::vector<Solution>
NaiveGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
         const IndexNum from, const IndexNum to);

std::vector<Solution>
FirstOptimizedGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                  const IndexNum from, const IndexNum to);

std::vector<Solution>
SecondOptimizedGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                   const IndexNum from, const IndexNum to);
