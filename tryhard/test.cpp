#include "try.h"

#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <cstdio>
#include <type_traits>
#include <concepts>
#include <cstring>
#include <memory>
#include <list>
#include <ranges>
#include <system_error>
#include <cerrno>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

using Catch::Matchers::Equals;
using Catch::Matchers::Message;

template <std::derived_from<std::exception> E>
bool operator==(const E& a, const E& b) {
    return std::strcmp(a.what(), b.what()) == 0;
}

template <class T>
void RequireEmpty(const Try<T>& a) {
    REQUIRE_FALSE(a.IsFailed());
    if constexpr (!std::same_as<T, void>) {
        REQUIRE_THROWS_MATCHES(a.Value(), std::exception, Message("Object is empty"));
    }
    REQUIRE_THROWS_MATCHES(a.Throw(), std::exception, Message("No exception"));
}

template <class T>
void RequireValue(const Try<T>& a, const std::type_identity_t<T>& value) {
    REQUIRE_FALSE(a.IsFailed());
    REQUIRE((a.Value() == value));
    REQUIRE_THROWS_MATCHES(a.Throw(), std::exception, Message("No exception"));
}

template <class T, class E>
void RequireError(const Try<T>& a, const E& expected) {
    REQUIRE(a.IsFailed());
    try {
        a.Value();
        FAIL("No throw");
    } catch (const E& e) {
        REQUIRE((e == expected));
    }
    try {
        a.Throw();
        FAIL("No throw");
    } catch (const E& e) {
        REQUIRE((e == expected));
    }
}

template <class E>
void RequireError(const Try<void>& a, const E& expected) {
    REQUIRE(a.IsFailed());
    try {
        a.Throw();
        FAIL("No throw");
    } catch (const E& e) {
        REQUIRE((e == expected));
    }
}

void RequireError(const Try<void>& a, int code) {
    REQUIRE(a.IsFailed());
    try {
        a.Throw();
        FAIL("No throw");
    } catch (const std::system_error& e) {
        REQUIRE((e.code().value() == code));
    }
}

TEST_CASE("Try constructors") {
    Try<int> a;
    RequireEmpty(a);

    Try b{5};
    RequireValue(b, 5);

    Try<std::string> c{tag::kEx, std::runtime_error{"err"}};
    RequireError(c, std::runtime_error{"err"});

    auto x = 3;
    Try d{x};
    RequireValue(d, x);

    Try e{std::vector{1, 2, 3}};
    RequireValue(e, {1, 2, 3});

    std::logic_error error{"err"};
    Try<double> f{tag::kEx, error};
    RequireError(f, error);
}

TEST_CASE("Tricky types") {
    std::runtime_error obj{"Some text"};
    Try a{obj};
    RequireValue(a, obj);

    Try b{std::logic_error{"No error"}};
    RequireValue(b, std::logic_error{"No error"});

    Try<double> c{tag::kEx, 5.2};
    RequireError(c, 5.2);

    Try<std::runtime_error> d{tag::kEx, std::vector{1, 2, 3}};
    RequireError(d, std::vector{1, 2, 3});

    auto p = std::make_exception_ptr(std::list{1, 2, 3});
    Try e{p};
    RequireValue(e, p);

    Try f{std::exception{}};
    RequireValue(f, {});

    Try<std::out_of_range> g{tag::kEx, std::out_of_range{"out"}};
    RequireError(g, std::out_of_range{"out"});
}

template <class T>
concept HasValue = requires(T t) {
    { t.Value() };
};

TEST_CASE("Try<void> is really void") {
    STATIC_CHECK_FALSE(HasValue<Try<void>>);
    STATIC_CHECK_FALSE(std::constructible_from<Try<void>, int>);
}

TEST_CASE("Try doesn't use public inheritance") {
    STATIC_CHECK_FALSE(std::derived_from<Try<void>, Try<int>>);
    STATIC_CHECK_FALSE(std::derived_from<Try<int>, Try<void>>);
}

TEST_CASE("Try<void> constructors") {
    Try<void> a;
    RequireEmpty(a);

    Try<void> b{tag::kEx, std::bad_weak_ptr{}};
    RequireError(b, std::bad_weak_ptr{});
}

template <class T>
void CheckCopyAndMove() {
    STATIC_CHECK_FALSE(std::copy_constructible<T>);
    STATIC_CHECK_FALSE(std::move_constructible<T>);
    STATIC_CHECK_FALSE(std::assignable_from<T&, const T&>);
    STATIC_CHECK_FALSE(std::assignable_from<T&, T&&>);
}

TEST_CASE("Try copy / move") {
    CheckCopyAndMove<Try<int>>();
    CheckCopyAndMove<Try<std::vector<double>>>();
    CheckCopyAndMove<Try<void>>();
}

TEST_CASE("Check constructors for lvalue and rvalue handling") {
    std::vector v = {4, -2, 7};
    const auto expected = v;

    RequireValue(Try{v}, expected);
    REQUIRE_THAT(v, Equals(expected));

    RequireValue(Try{std::move(v)}, expected);
    REQUIRE(v.empty());

    v = expected;
    RequireError(Try<bool>(tag::kEx, v), expected);
    REQUIRE_THAT(v, Equals(expected));

    RequireError(Try<bool>(tag::kEx, std::move(v)), expected);
    REQUIRE(v.empty());

    v = expected;
    RequireError(Try<void>(tag::kEx, v), expected);
    REQUIRE_THAT(v, Equals(expected));

    RequireError(Try<void>(tag::kEx, std::move(v)), expected);
    REQUIRE(v.empty());
}

TEST_CASE("TryRun wrapper works properly") {
    auto f1 = [] { throw std::invalid_argument{"invalid"}; };
    auto f2 = [](const std::string& s) { return s; };
    auto f3 = [](int left, int right) { return (left < right) ? left : throw "Shit happens"; };
    auto f4 = [](const std::vector<char>& data, size_t pos) { return data.at(pos); };
    auto f5 = [](const char*, const char*, int) { throw ENOENT; };
    auto f6 = [](std::vector<int>& elems, int x) {
        elems.push_back(x);
        if (elems.size() == 3) {
            throw elems;  // wtf
        }
    };

    RequireError(TryRun(f1), std::invalid_argument{"invalid"});
    RequireValue(TryRun(f2, "hello!"), "hello!");

    {
        RequireValue(TryRun(f3, 3, 5), 3);
        auto a = TryRun(f3, 5, 3);
        REQUIRE(a.IsFailed());
        REQUIRE_THROWS_WITH(a.Value(), "Shit happens");
        REQUIRE_THROWS_WITH(a.Throw(), "Shit happens");
    }

    {
        std::vector data{'a', 'b', 'c'};
        RequireValue(TryRun(f4, data, 0), 'a');

        auto a = TryRun(f4, data, 3);
        REQUIRE(a.IsFailed());
        REQUIRE_THROWS_AS(a.Throw(), std::out_of_range);
        REQUIRE_THROWS_AS(a.Value(), std::out_of_range);
    }

    RequireError(TryRun(f5, "test.cpp", "r", 0), ENOENT);

    {
        std::vector<int> elems;
        auto a = TryRun(
            [f6, &elems](int count) {
                for (auto i : std::views::iota(0, count)) {
                    f6(elems, i);
                }
            },
            4);
        REQUIRE(a.IsFailed());
        REQUIRE_THROWS_WITH(a.Throw(), "Unknown exception");
    }
}

TEST_CASE("Rvalue reference parameter") {
    {
        auto f = [](int&&) {};
        CHECK_FALSE(TryRun(f, 3).IsFailed());
    }
    {
        auto f = [](bool&& b) { return b + 2.2; };
        auto b = true;
        CHECK(TryRun(f, std::move(b)).Value() == 3.2);
    }
    {
        auto p = std::make_unique<int>(42);
        auto f = [](std::unique_ptr<int> p) { return p; };
        auto a = TryRun(f, std::move(p));
        CHECK(*a.Value() == 42);
    }
    {
        std::vector v1 = {1, 2, 3};
        const auto* p = v1.data();
        auto f = [&v1] { return std::move(v1); };
        auto a = TryRun(f);
        CHECK(a.Value().data() == p);
    }
}

TEST_CASE("Overload") {
    struct X {
        bool operator()(int&) {
            return true;
        };
        double operator()(int&&) {
            return 1.2;
        };
        int operator()(const int&) {
            return 3;
        };
    };
    auto a = 3;
    STATIC_CHECK(std::same_as<decltype(TryRun(X{}, a)), Try<bool>>);
    STATIC_CHECK(std::same_as<decltype(TryRun(X{}, 3)), Try<double>>);
    STATIC_CHECK(std::same_as<decltype(TryRun(X{}, std::move(a))), Try<double>>);
}

TEST_CASE("Try saves object") {
    class MySuperException : public std::exception {
    public:
        const char* what() const noexcept override {
            return "Check";
        }
    };

    auto f = [](int x) { return (x > 10) ? x : throw MySuperException{}; };
    auto result = TryRun(f, 5);
    RequireError(result, MySuperException{});

    const std::exception* addr = nullptr;
    try {
        result.Throw();
        FAIL("No throw");
    } catch (const MySuperException& e) {
        addr = &e;
    }

    try {
        result.Value();
        FAIL("No throw");
    } catch (const MySuperException& e) {
        CHECK(&e == addr);
    }
}
