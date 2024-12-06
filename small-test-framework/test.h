#pragma once

#include <memory>
#include <vector>
#include <string>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() = default;
};

class TestRegistry {
public:
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        return nullptr;
    }

    void RunTest(const std::string& test_name) {
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        return {};
    }

    std::vector<std::string> ShowAllTests() const {
        return {};
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
    }

    void Clear() {
    }
};
