#pragma once

#include "student.h"

#include <vector>
#include <stdexcept>

enum class SortType { kByName, kByDate };

void SortStudents(std::vector<Student>* students, SortType sort_type) {
    throw std::runtime_error{"Not implemented"};
}
