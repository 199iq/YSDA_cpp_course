#include "polish_notation.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Short") {
    CHECK(EvaluateExpression("1") == 1);
}

TEST_CASE("Some tests") {
    CHECK(EvaluateExpression("2 3 +") == 5);
    CHECK(EvaluateExpression("2 2 * 2 +") == 6);
    CHECK(EvaluateExpression("10 3 3 * -") == 1);
    CHECK(EvaluateExpression("-3 11 + -8 + 4 *") == 0);
    CHECK(EvaluateExpression("-3 -0 - 3 + 1 - -19 * 1 +") == 20);
    CHECK(EvaluateExpression("1 2 3 4 5 + + + +") == 15);
    CHECK(EvaluateExpression("1 -1 2 -2 3 -3 * - + * *") == -9);
}
