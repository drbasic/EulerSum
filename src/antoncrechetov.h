#pragma once

#include <vector>

#include "EulerSum.h"

std::vector<Solution>
AntonCrechetov(const IndexNum elements_count, const std::vector<WorkNum>& powers);

std::vector<Solution>
AntonCrechetov2(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                const IndexNum from, const IndexNum to);
