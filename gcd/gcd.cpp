#include <numeric>
#include <iostream>

int Gcd(int a, int b) {
    while (a * b != 0) {
        auto rem = a % b;
        a = b;
        b = rem;
    }
    return std::max(a, b);
}

int TryRead() {
    if (int x; std::cin >> x) {
        return x;
    }
    std::cout << "Fail on reading\n";
    std::exit(0);
}

int main() {
    auto a = TryRead();
    auto b = TryRead();
    if ((a <= 0) || (b <= 0)) {
        std::cout << "Numbers should be positive\n";
        return 0;
    }

    auto gcd1 = Gcd(a, b);
    auto gcd2 = std::gcd(a, b);
    if (gcd1 != gcd2) {
        std::cout << "Shit happens\n";
        return 1;
    }
    std::cout << "Try again...\n";
}
