#pragma once

#include <iterator>
#include <cstddef>
#include <iostream>
#include <initializer_list>

template<typename T>
class Allocator {
public:
    T* allocate(size_t capacity) {
        return static_cast<T*>(::operator new(capacity * sizeof(T)));
    }

    void deallocate(T* begin) {
        ::operator delete(begin);
    }

    template <typename... Args>
    void construct(T* ptr, Args&&... args) {
        new(ptr) T(std::forward<Args>(args)...); // placement new
    }

    void destroy(T* ptr) {
        ptr->~T();
    }
};

class Vector {
public:
    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = int;
        using difference_type = ptrdiff_t;
        using pointer = int*;
        using reference = int&;
        // using const_reference = const int&;

        explicit Iterator(pointer val = nullptr) : iter_(val) {}

        // Access and iterations
        Iterator& operator++() {
            ++iter_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++iter_;
            return tmp;
        }

        Iterator& operator--() {
            --iter_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --iter_;
            return tmp;
        }

        reference operator*() const {
            return *iter_;
        }

        pointer operator->() const {
            return iter_;
        }

        reference operator[](int idx) {
            return *(iter_ + idx);
        }

        reference operator[](int idx) const {
            return *(iter_ + idx);
        }

        // Comparison
        auto operator<=>(const Iterator&) const = default;

        // Arithmetics
        Iterator& operator+=(int add) {
            iter_ += add;
            return *this;
        }

        Iterator operator+(int add) const {
            Iterator tmp = *this;
            tmp += add;
            return tmp;
        }

        Iterator& operator-=(int add) {
            iter_ -= add;
            return *this;
        }

        Iterator operator-(int add) const {
            Iterator tmp = *this;
            tmp -= add;
            return tmp;
        }

        difference_type operator-(const Iterator& rhs) const {
            return static_cast<difference_type>(iter_ - rhs.iter_); // do we need static_cast here? and when do we use it in general?
        }

    private:
        pointer iter_;
    };

    const int& operator[](size_t idx) const { // or just int? 
        if (idx >= size_) {
            throw std::runtime_error("Vector index is out of range");
        }
        return *(data_ + idx);
    }

    int& operator[](size_t idx) { // do we need const here? this method doesn't change anything
        if (idx >= size_) {
            throw std::runtime_error("Vector index is out of range");
        }
        return *(data_ + idx);
    }

    // copy-and-swap
    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            Vector tmp(rhs);
            Swap(tmp);
        }
        return *this;
    }

    // move-and-swap
    Vector& operator=(Vector&& rhs) {
        if (this != &rhs) {
            Vector tmp(std::move(rhs));
            Swap(tmp);
        }
        return *this;
    }

    Vector()
        : data_(nullptr), size_(0), capacity_(0), begin_(nullptr), end_(nullptr) {
    }

    Vector(size_t size) : size_(size), capacity_(size) { // capacity_(size) or capacity_(size_) ? -> capacity_(size) is better
        data_ = allocator_.allocate(capacity_);
        
        // initialize with zeros
        for (size_t i = 0; i < capacity_; i++) {
            allocator_.construct(data_ + i, 0); // data_ + i ? 
        }
        UpdateIterators();
    }

    Vector(std::initializer_list<int> init) {
        size_t size = init.size();
        size_ = size;
        capacity_ = size;

        data_ = allocator_.allocate(capacity_);
        // initialize using initializer_list
        for (size_t i = 0; i < capacity_; i++) {
            allocator_.construct(data_ + i, *(init.begin() + i)); // can we just do init[i] ?
        }
        UpdateIterators();
    }

    Vector(const Vector& rhs) {
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;

        data_ = allocator_.allocate(capacity_);
        for (size_t i = 0; i < size_; i++) {
            allocator_.construct(data_ + i, rhs[i]);
        }
        UpdateIterators();
    }

    Vector(Vector&& rhs)
        : data_(rhs.data_), size_(rhs.size_), capacity_(rhs.capacity_) {
        UpdateIterators();
        rhs.data_ = nullptr;
        rhs.size_ = rhs.capacity_ = 0;
        rhs.begin_ = rhs.end_ = Iterator();
    }

    // Vector(Vector&& rhs) {
    //     // can we actually do const Vector&& (seems stupid but anyways)?
    //     // yes but pretty useless

    //     // if we don't clear old data do we get a memory leak?
    //     // no, cuz we create a new object
    //     // but this problem appears in move-assignment
    //     size_ = rhs.Size();
    //     capacity_ = rhs.Capacity();
    //     data_ = rhs.data_;
    //     UpdateIterators();

    //     rhs.data_ = nullptr;
    //     rhs.size_ = 0;
    //     rhs.capacity_ = 0;
    // }

    ~Vector() {
        for (size_t i = 0; i < size_; i++) {
            allocator_.destroy(data_ + i);
        }
        allocator_.deallocate(data_);
        data_ = nullptr; // ???
        size_ = capacity_ = 0;
        begin_ = end_ = Iterator(); // or just call destructor ?
        // data_ = begin_ = end_ = nullptr; // is it good to write like that?
    }

public:
    // for the future: we can always return Iterator(data_) and Iterator(data_ + size_)
    // instead of keeping begin_ and end_

    Iterator begin() const {
        return begin_;
    }

    Iterator end() const {
        return end_;
    }

    size_t Size() const {
        return size_;
    }

    size_t Capacity() const {
        return capacity_;
    }

    void PushBack(int val) {
        if (size_ >= capacity_) {
            // call reallocation
            size_t new_capacity = (capacity_ > 0 ? capacity_ << 1 : 1); // careful with bitwise operations here!
            Realloc(new_capacity);

        }
        allocator_.construct(data_ + size_, val);
        size_++;
        UpdateIterators();
    }

    void PopBack() {
        if (!size_) {
            throw std::runtime_error("Can't do PopBack while vector is empty!");
        }
        allocator_.destroy(data_ + size_ - 1);
        size_--;
        UpdateIterators();
    }

    void Clear() {
        for (size_t i = 0; i < size_; i++) {
            allocator_.destroy(data_ + i);
        }
        size_ = 0;
        UpdateIterators();
    }

    void Reserve(size_t new_capacity) {
        if (capacity_ < new_capacity) {
            Realloc(new_capacity);
        }
    }

    void Swap(Vector& rhs) {
        std::swap(data_, rhs.data_);
        std::swap(size_, rhs.size_);
        std::swap(capacity_, rhs.capacity_);
        UpdateIterators();
        rhs.UpdateIterators();
    }

private:
    int* data_;
    size_t size_, capacity_;
    Allocator<int> allocator_;
    Iterator begin_, end_;

private:
    int* GetData() const {
        return data_;
    }

    void UpdateIterators() {
        begin_ = Iterator(data_);
        if (data_ == nullptr) {
            end_ = Iterator(data_);
        } else {
            end_ = Iterator(data_ + size_);
        }
    }

    void Realloc(size_t new_capacity) {
        // allocate memory & copy old values
        int* new_data = allocator_.allocate(new_capacity);
        for (size_t i = 0; i < size_; i++) {
            // copy element
            allocator_.construct(new_data + i, *(data_ + i));
            // erase old elements; so in the moment there are exactly n elements
            // it's not the best option because
            // if allocator_.construct throws an error you won't be able to obtain old values
            allocator_.destroy(data_ + i);
        }
        allocator_.deallocate(data_);
        
        data_ = new_data;
        new_data = nullptr; // local variable !!!
        capacity_ = new_capacity;
        UpdateIterators();
    }
};

Vector::Iterator operator+(int add, const Vector::Iterator& it) {
    return it + add;
}