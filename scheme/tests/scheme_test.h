#pragma once

#include "scheme.h"
#include "error.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

class SchemeTest {
public:
    void ExpectEq(const std::string& expression, const std::string& expected) {
        REQUIRE(scheme_.Evaluate(expression) == expected);
    }

    void ExpectNoError(const std::string& expression) {
        REQUIRE_NOTHROW(scheme_.Evaluate(expression));
    }

    void ExpectSyntaxError(const std::string& expression) {
        REQUIRE_THROWS_AS(scheme_.Evaluate(expression), SyntaxError);
    }

    void ExpectRuntimeError(const std::string& expression) {
        REQUIRE_THROWS_AS(scheme_.Evaluate(expression), RuntimeError);
    }

    void ExpectNameError(const std::string& expression) {
        REQUIRE_THROWS_AS(scheme_.Evaluate(expression), NameError);
    }

private:
    Scheme scheme_;
};
