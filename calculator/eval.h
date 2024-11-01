#pragma once

#include "calculator.h"

#include <string>
#include <sstream>

int64_t EvaluateExpression(std::string expression) {
    std::istringstream ss{std::move(expression)};
    Tokenizer tokenizer{&ss};
    auto expr = ParseExpression(&tokenizer);
    return expr->Evaluate();
}
