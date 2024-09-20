#pragma once

#include "student.h"
#include "date.h"

#include <stdexcept>
#include <map>
#include <string>
#include <vector>
#include <tuple>

std::map<std::string, std::vector<StudentName>> GetStudents(
    const std::vector<std::pair<std::string, int>>& universities_info,
    const std::vector<std::tuple<StudentName, Date, int, std::vector<std::string>>>&
        students_info) {
    throw std::runtime_error{"Not implemented"};
}
