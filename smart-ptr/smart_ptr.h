#pragma once

#include <string>

class WeakPtr;

class SharedPtr {
public:
    SharedPtr(const WeakPtr& ptr);
};

class WeakPtr {
public:
};

SharedPtr::SharedPtr(const WeakPtr& ptr) {
}
