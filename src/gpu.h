#pragma once

#include <vector>

#include "EulerSum.h"

std::vector<Solution>
NaiveGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers);

std::vector<Solution>
FirstOptimizedGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers);

std::vector<Solution>
SecondOptimizedGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers);
