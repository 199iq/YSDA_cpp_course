#include "deque.h"
#include "util.h"

#include <vector>
#include <ranges>
#include <memory>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Memory usage") {
    {
        std::unique_ptr<std::vector<Deque>> p_v;
        auto size = 1'000;

        {
            auto guard = MakeMemoryGuard<int>(1'000 * size);
            p_v = std::make_unique<std::vector<Deque>>(size);
        }

        auto num_steps = 200;
        auto guard = MakeMemoryGuard<int>(6 * num_steps * size);
        for (auto& d : *p_v) {
            for (auto i : std::views::iota(0, num_steps)) {
                d.PushBack(i);
                d.PushFront(i);
            }
        }
    }

    Deque a(1'000'000);

    {
        auto size = 10'000'000;
        auto guard = MakeMemoryGuard<int>(3 * size);

        a.Clear();
        for (auto i : std::views::iota(0, size)) {
            a.PushFront(i);
        }
    }

    {
        auto size = 80'000'000;
        auto guard = MakeMemoryGuard<int>(3 * size);

        a.Clear();
        for (auto i : std::views::iota(0, size)) {
            a.PushBack(i);
        }
    }
}
