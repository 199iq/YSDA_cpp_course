#pragma once

#include <numeric>

class Fraction {
public:
    Fraction(int numerator, int denominator) : numerator_{numerator}, denominator_{denominator} {
        Normalize();
    }

    // делегирование конструкторов,
    // явно вызываем предыдущий конструктор от двух аргументов
    Fraction(int numerator) : Fraction{numerator, 1} {
    }

    /* того же можно достичь, определив один конструктор с аргументом по умолчанию, так:
     * Fraction(int numerator, int denominator = 1)
     *     : numerator_{numerator}, denominator_{denominator} { Normalize(); } */

    int Numerator() const {
        return numerator_;
    }

    int Denominator() const {
        return denominator_;
    }

    /* перегружаем оператор *=, общее правило в С++ --- все "присваивающие"
     * операторы должны возвращать ссылку на текущий объект,
     * т.к. эти операторы не создают новый
     * объект, а модифицируют уже существующий
     *
     * Fraction a{1, 2};
     * Fraction b{2, 3};
     * a *= b раскрывается компилятором как a.operator*=(b);
     * таким образом оператор - обычный метод, просто с необычным названием */
    Fraction& operator*=(const Fraction& rhs) {
        numerator_ *= rhs.numerator_;
        denominator_ *= rhs.denominator_;
        Normalize();
        return *this;
    }

    /* можно реализовывать * через *= или наоборот, здесь показаны оба подхода, перегрузка *
     * вынесена за пределы класса */

    /* friend-объявление. Говорим о том, что данная функция (которая определена после класса) может
     * иметь доступ к private секции класса. Несмотря на то, что написано оно внутри класса, данная
     * функция не является методом --- в ней нет внутри this, она работает с двумя аргументами.
     *
     * когда мы пишем a + b, компилятор пытается это раскрыть как a.operator+(b)
     * (оператор перегружен внутри класса и это метод класса),
     * а при неудаче как operator+(a, b) (оператор перегружен снаружи и это просто функция).
     * В данном случае сработает второе. */
    friend Fraction operator+(const Fraction& lhs, const Fraction& rhs);

    Fraction& operator+=(const Fraction& rhs) {
        return *this = *this + rhs;  // используем объявленный выше оператор +
    }

    bool operator==(const Fraction& rhs) const {
        return (numerator_ == rhs.numerator_) && (denominator_ == rhs.denominator_);
    }

private:
    int numerator_, denominator_;

    void Normalize() {
        auto g = std::gcd(numerator_, denominator_);
        numerator_ /= g;
        denominator_ /= g;
    }
};

Fraction operator*(Fraction lhs, const Fraction& rhs) {
    return lhs *= rhs;
    /* обратите внимание, lhs мы принимаем по значению --- это копия аргумента, поэтому можно смело
     * ее изменять. Оператор *= как раз записывает результат умножения в левый аргумент и возвращает
     * его. В данном случае мы выражаем * через *= */
}

Fraction operator+(const Fraction& lhs, const Fraction& rhs) {
    return Fraction{lhs.numerator_ * rhs.denominator_ + lhs.denominator_ * rhs.numerator_,
                    lhs.denominator_ * rhs.denominator_};
}
