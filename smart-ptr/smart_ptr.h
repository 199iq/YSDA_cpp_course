#pragma once

#include <string>
#include <iostream>

class WeakPtr;

class ControlBlock {
public:
    ControlBlock(std::string* ptr = nullptr) : shared_ref_count_(1), weak_ref_count_(0), ptr_(ptr) {}

    void AddSharedRef() {
        ++shared_ref_count_;
    }

    void AddWeakRef() {
        ++weak_ref_count_;
    }

    void ReleaseSharedRef() {
        --shared_ref_count_;
    }

    void ReleaseWeakRef() {
        --weak_ref_count_;
    }

    std::string* GetPtr() const {
        return ptr_;
    }

    int SharedCount() const {
        return shared_ref_count_;
    }

    int WeakCount() const {
        return weak_ref_count_;
    }

    void DestroyObject() {
        delete ptr_;
        ptr_ = nullptr;
    }
private:
    int shared_ref_count_;
    int weak_ref_count_;
    std::string* ptr_;
};

class SharedPtr {
public:
    SharedPtr(std::string* ptr = nullptr) {
        if (ptr != nullptr) {
            control_block_ = new ControlBlock(ptr);
        } else {
            control_block_ = nullptr;
        }
    }

    SharedPtr(const SharedPtr& other) {
        control_block_ = other.control_block_;
        if (control_block_ != nullptr) {
            control_block_->AddSharedRef();
        }
    }

    SharedPtr(SharedPtr&& other) {
        control_block_ = other.control_block_;
        other.control_block_ = nullptr;
    }

    // operator= (copy)
    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            Detach();
            control_block_ = other.control_block_;
            if (control_block_ != nullptr) {
                control_block_->AddSharedRef();
            }
        }
        return *this;
    }

    // operator= (move)
    SharedPtr& operator=(SharedPtr&& other) {
        if (this != &other) {
            Detach();
            control_block_ = other.control_block_;
            other.control_block_ = nullptr;
        }
        return *this;
    }

    std::string* operator->() const {
        return control_block_->GetPtr();
    }

    // operator* (dereference?) (non-const + const)
    std::string& operator*() {
        // user's responsibility
        // if (control_block_->GetPtr() == nullptr) {
        //     throw std::runtime_error("Can't dereference nullptr!");
        // }
        return *control_block_->GetPtr();
    }

    const std::string& operator*() const {
        // user's responsibility
        // if (control_block_->GetPtr() == nullptr) {
        //     throw std::runtime_error("Can't dereference nullptr (const dereferencing)!");
        // }
        return *control_block_->GetPtr();
    }

    SharedPtr(const WeakPtr& ptr);

    ~SharedPtr() {
        Detach();
    }

public:
    std::string* Get() const {
        return (control_block_ != nullptr ? control_block_->GetPtr() : nullptr);
    }

    ControlBlock* GetControlBlock() const {
        return control_block_;
    }

    void Reset(std::string* new_ptr) {
        if (Get() == new_ptr) {
            return;
        }
        // Detach();
        *this = SharedPtr(new_ptr);
    }
private:
    ControlBlock* control_block_;
private:
    void Detach() {
        if (control_block_ == nullptr) {
            return;
        }

        control_block_->ReleaseSharedRef();
        if (control_block_ != nullptr && !control_block_->SharedCount() && !control_block_->WeakCount()) {
            control_block_->DestroyObject();
            delete control_block_;
        } else if (control_block_ != nullptr && !control_block_->SharedCount()) {
            control_block_->DestroyObject(); // just delete the object
        }
        control_block_ = nullptr;
    }
};

class WeakPtr {
public:
    WeakPtr() : control_block_(nullptr) {}

    WeakPtr(const SharedPtr& shared_ptr)
        : control_block_(shared_ptr.GetControlBlock()) {
        if (control_block_ != nullptr) {
            control_block_->AddWeakRef();
        }
    }

    WeakPtr(const WeakPtr& other) {
        control_block_ = other.control_block_;
        if (control_block_ != nullptr) {
            control_block_->AddWeakRef();
        }
    }

    WeakPtr(WeakPtr&& other) {
        control_block_ = other.control_block_; // +1 -1
        other.control_block_ = nullptr;
    }

    // operator= (copy)
    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            Detach();
            control_block_ = other.control_block_;
            if (control_block_ != nullptr) {
                control_block_->AddWeakRef();
            }
        }
        return *this;
    }

    // operator= (move)
    WeakPtr& operator=(WeakPtr&& other) {
        if (this != &other) {
            Detach();
            control_block_ = other.control_block_; // +1 -1
            other.control_block_ = nullptr;
        }
        return *this;
    }

    ~WeakPtr() {
        Detach();
    }
public:
    bool IsExpired() const {
        return (control_block_ == nullptr || !control_block_->SharedCount());
    }

    ControlBlock* GetControlBlock() const {
        return (IsExpired() ? nullptr : control_block_);
    }

    SharedPtr Lock() {
        SharedPtr new_ptr = SharedPtr(*this);
        return new_ptr;
    }

private:
    ControlBlock* control_block_;
private:
    void Detach() {
        if (control_block_ == nullptr) {
            return;
        }

        control_block_->ReleaseWeakRef();
        if (control_block_ != nullptr && !control_block_->SharedCount() && !control_block_->WeakCount()) {
            delete control_block_;
            // delete ptr_; // ptr_ should be deleted once the last shared ptr is deleted
            // here we should delete only ref_counter
        }
        control_block_ = nullptr;
    }
};

SharedPtr::SharedPtr(const WeakPtr& ptr)
    : control_block_(ptr.GetControlBlock()) {
    if (control_block_ != nullptr) {
        control_block_->AddSharedRef();
    }
}