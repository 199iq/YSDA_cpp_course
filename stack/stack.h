#pragma once

#include <cstddef>
#include <iostream>
#include <vector>

class Stack {
public:
    void Push(int x) {
        stack.push_back(x);
    }

    bool Pop() {
        if (!stack.size()) {
            return false;
        }
        stack.pop_back();
        return true;
    }

    int Top() const {
        return stack.back();
    }

    bool Empty() const {
        return !stack.size();
    }

    size_t Size() const {
        return stack.size();
    }
private:
    std::vector<int> stack;
};
