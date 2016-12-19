#pragma once

#include <vector>

#include "EulerSum.h"

std::vector<Solution>
AntonCrechetov(const IndexNum elements_count, const std::vector<WorkNum>& powers,
               const IndexNum from, const IndexNum to);
