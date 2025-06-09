#pragma once

#include <utility>
#include <vector>
#include <cstdint>
#include <stdexcept>

std::vector<std::pair<int64_t, int>> Factorize(int64_t x) {
    std::vector<std::pair<int64_t, int>> ans;
    int64_t tmp = x;

    for (int64_t d = 2; d * d <= x; d++) {
        int cnt = 0;
        while (tmp % d == 0) {
            tmp /= d;
            cnt++;
        }
        if (cnt > 0) {
            ans.emplace_back(d, cnt);
        }
    }
    if (tmp > 1) {
        ans.emplace_back(tmp, 1);
    }

    return ans;
}
