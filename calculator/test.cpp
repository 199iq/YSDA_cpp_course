#include "calculator.h"
#include "eval.h"

#include <sstream>
#include <variant>

#include <catch2/catch_test_macros.hpp>

using Token = std::variant<int64_t, char>;

void CheckTokens(std::string expression, const std::vector<Token>& tokens) {
    std::istringstream ss{std::move(expression)};
    Tokenizer tokenizer{&ss};
    for (const auto& token : tokens) {
        if (const auto* number = std::get_if<int64_t>(&token)) {
            REQUIRE(tokenizer.GetType() == Tokenizer::kNumber);
            REQUIRE(tokenizer.GetNumber() == *number);
        } else {
            REQUIRE(tokenizer.GetType() == Tokenizer::kSymbol);
            REQUIRE(tokenizer.GetSymbol() == std::get<char>(token));
        }
        tokenizer.Consume();
    }
    REQUIRE(tokenizer.GetType() == Tokenizer::kEnd);
}

TEST_CASE("Tokenizer") {
    CheckTokens("1", {1});
    CheckTokens("512", {512});
    CheckTokens("", {});
    CheckTokens("2+2", {2, '+', 2});
    CheckTokens("-36", {'-', 36});
    CheckTokens("87/12", {87, '/', 12});
    CheckTokens("(5)", {'(', 5, ')'});
    CheckTokens("1456000123000", {1'456'000'123'000});
    CheckTokens("- 2 + 37  *5", {'-', 2, '+', 37, '*', 5});
    CheckTokens("- (-  27 -( 32-  71))", {'-', '(', '-', 27, '-', '(', 32, '-', 71, ')', ')'});
    CheckTokens("-16- 21 +3* (4/  54)", {'-', 16, '-', 21, '+', 3, '*', '(', 4, '/', 54, ')'});
}

TEST_CASE("Simple") {
    CHECK(EvaluateExpression("1") == 1);
    CHECK(EvaluateExpression("-2") == -2);
    CHECK(EvaluateExpression("1+2") == 3);
    CHECK(EvaluateExpression("2*3") == 6);
    CHECK(EvaluateExpression("10-1") == 9);
    CHECK(EvaluateExpression("10/2") == 5);
    CHECK(EvaluateExpression("-2*-4") == 8);
    CHECK(EvaluateExpression("1000000000000") == 1'000'000'000'000);
    CHECK(EvaluateExpression("2000*2000*2000*2000") == 16'000'000'000'000);
}

TEST_CASE("Complex") {
    CHECK(EvaluateExpression("2*(1+2*(3*4+105*431)*2+((2*3)*10+343))") == 362'944);
    CHECK(EvaluateExpression("(((1 + 1)*3)*4)+12") == 36);
    CHECK(EvaluateExpression("(((1 + 1)))") == 2);
    CHECK(EvaluateExpression("-(-1)") == 1);
    CHECK(EvaluateExpression("(2+3)*(4+5*1234)") == 30'870);
    CHECK(EvaluateExpression("1*2*3*4*5*6*7*8*9* 10") == 3'628'800);
    CHECK(EvaluateExpression("1+2+3+4+5+6+7+8+9+10") == 55);
    CHECK(EvaluateExpression("(((2))+(3))") == 5);
    CHECK(EvaluateExpression("-(-2-3-7)") == 12);
    CHECK(EvaluateExpression("-2*-(-3)") == -6);
    CHECK(EvaluateExpression("-1- -2-(-(-(0-2-3-4-5+ -2-(3))+1)-3-2     -19) * 17239 * 82473842") ==
          62'557'728'738'473);
    CHECK(EvaluateExpression("19 + 2 / 3 + (1 + 1 + 394) / 7 / 2 / 1") == 47);
    CHECK(EvaluateExpression("(-300  + 22) / (65 -  -12)") == -3);
    CHECK(EvaluateExpression("4532 * 54254*(452- 120 +43 - -(5434 + 451 +541) ) / 67/ 123") ==
          202'915'174);
    CHECK(EvaluateExpression("-3495 / 92 -(-34122 +5123) / 21 * 3 +-15") == 4'088);
}
