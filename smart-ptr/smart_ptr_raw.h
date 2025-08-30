#pragma once

#include <string>
#include <iostream>

class WeakPtr;

class RefCounter {
public:
    RefCounter() : shared_ref_count_(1), weak_ref_count_(0) {}
public:
    int shared_ref_count_;
    int weak_ref_count_;
};

class SharedPtr {
public:
    SharedPtr() : ref_counter_(nullptr), ptr_(nullptr) {}
    SharedPtr(std::string* ptr)
        : ptr_(ptr) {
            if (ptr != nullptr) {
                ref_counter_ = new RefCounter();
            } else {
                ref_counter_ = nullptr;
            }
    }

    SharedPtr(const SharedPtr& other) {
        ptr_ = other.ptr_;
        ref_counter_ = other.ref_counter_;
        AddRef();
    }

    SharedPtr(SharedPtr&& other) {
        ptr_ = other.ptr_;
        ref_counter_ = other.ref_counter_; // ref_counter_ doesn't change (+1 -1)

        other.ptr_ = nullptr;
        other.ref_counter_ = nullptr;
    }

    // operator= (copy)
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            Detach();
            ref_counter_ = other.ref_counter_;
            AddRef();
            ptr_ = other.ptr_;
        }
        return *this;
    }

    // operator= (move)
    SharedPtr& operator=(SharedPtr&& other) {
        if (this != &other) {
            Detach();
            ref_counter_ = other.ref_counter_; // don't change (+1 -1)
            ptr_ = other.ptr_;

            other.ptr_ = nullptr;
            other.ref_counter_ = nullptr;
        }
        return *this;
    }

    std::string* operator->() const {
        return ptr_;
    }

    // operator* (dereference?) (non-const + const)
    std::string& operator*() {
        if (ptr_ == nullptr) {
            throw std::runtime_error("Can't dereference nullptr!");
        }
        return *ptr_;
    }

    const std::string& operator*() const {
        if (ptr_ == nullptr) {
            throw std::runtime_error("Can't dereference nullptr (const dereferencing)!");
        }
        return *ptr_;
    }

    SharedPtr(const WeakPtr& ptr);

    ~SharedPtr() {
        Detach();
    }

public:
    std::string* Get() const {
        return ptr_;
    }

    RefCounter* GetRefCounter() const {
        return ref_counter_;
    }

    void Reset(std::string* new_ptr) {
        if (ptr_ == new_ptr) {
            return;
        }
        // Detach();
        *this = SharedPtr(new_ptr);
    }
private:
    RefCounter* ref_counter_;
    std::string* ptr_;
private:
    void Detach() {
        ReleaseRef();
        if (ref_counter_ != nullptr && !ref_counter_->shared_ref_count_ && !ref_counter_->weak_ref_count_) {
            delete ref_counter_;
            // ptr_->~std::string(); // only with placement-new
            delete ptr_;
        } else if (ref_counter_ != nullptr && !ref_counter_->shared_ref_count_) {
            delete ptr_; // just delete the object
        }
        ref_counter_ = nullptr;
        ptr_ = nullptr;
    }

    void AddRef() {
        if (ref_counter_ != nullptr) {
            ++ref_counter_->shared_ref_count_;
        }
    }

    void ReleaseRef() {
        if (ref_counter_ != nullptr) {
            --ref_counter_->shared_ref_count_;
        }
    }
};

class WeakPtr {
public:
    WeakPtr() : ptr_(nullptr), ref_counter_(nullptr) {}

    WeakPtr(const SharedPtr& shared_ptr)
        : ptr_(shared_ptr.Get()), ref_counter_(shared_ptr.GetRefCounter()) {
        AddRef();
    }

    WeakPtr(const WeakPtr& other) {
        ptr_ = other.ptr_;
        ref_counter_ = other.ref_counter_;
        AddRef();
    }

    WeakPtr(WeakPtr&& other) {
        ptr_ = other.ptr_;
        ref_counter_ = other.ref_counter_; // +1 -1

        other.ptr_ = nullptr;
        other.ref_counter_ = nullptr;
    }

    // operator= (copy)
    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            Detach();
            ref_counter_ = other.ref_counter_;
            ptr_ = other.ptr_;
            AddRef();
        }
        return *this;
    }

    // operator= (move)
    WeakPtr& operator=(WeakPtr&& other) {
        if (this != &other) {
            Detach();
            ref_counter_ = other.ref_counter_; // +1 -1
            ptr_ = other.ptr_;

            other.ref_counter_ = nullptr;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    ~WeakPtr() {
        Detach();
    }
public:
    bool IsExpired() const {
        return (!ref_counter_ || !ref_counter_->shared_ref_count_);
    }

    std::string* Get() const {
        return ptr_;
    }

    RefCounter* GetRefCounter() const {
        return ref_counter_;
    }

    SharedPtr Lock() { // is it really SharedPtr or SharedPtr& (i think first cuz second will get killed after out of scope)
        SharedPtr new_ptr = SharedPtr(*this);
        return new_ptr;
    }

private:
    std::string* ptr_;
    RefCounter* ref_counter_;
private:
    void Detach() {
        ReleaseRef();
        if (ref_counter_ != nullptr && !ref_counter_->shared_ref_count_ && !ref_counter_->weak_ref_count_) {
            delete ref_counter_;
            // delete ptr_; // ptr_ should be deleted once the last shared ptr is deleted
            // here we should delete only ref_counter
        }
        ref_counter_ = nullptr;
        ptr_ = nullptr;
    }

    void AddRef() {
        if (ref_counter_ != nullptr) {
            ++ref_counter_->weak_ref_count_;
        }
    }

    void ReleaseRef() {
        if (ref_counter_ != nullptr) {
            --ref_counter_->weak_ref_count_;
        }
    }
};

SharedPtr::SharedPtr(const WeakPtr& ptr) {
    if (ptr.IsExpired()) {
        ptr_ = nullptr;
        ref_counter_ = nullptr;
    } else {
        ptr_ = ptr.Get();
        ref_counter_ = ptr.GetRefCounter();
    }
    AddRef();
}