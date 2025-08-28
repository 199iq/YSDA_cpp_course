#pragma once

#include <string>
#include <cstddef>
#include <iostream>

struct State {
    State() : ref_count_(1), size_(0), capacity_(0), data_(nullptr) {}

    State(size_t capacity) 
        : ref_count_(1), size_(0), capacity_(capacity) {
        data_ = new std::string[capacity]; // maybe it's better to write allocator but i guess here it's okay
    }

    ~State() {
        delete[] data_;
    }

public:
    int ref_count_;
    size_t size_, capacity_;
    std::string* data_;
};

class COWVector {
public:
    COWVector() {
        state_ = new State();
    }

    ~COWVector() {
        --state_->ref_count_;
        if (!state_->ref_count_) {
            delete state_;
        }
    }

    COWVector(const COWVector& other) {
        state_ = other.state_;
        ++state_->ref_count_;
    }

    COWVector& operator=(const COWVector& other) {
        if (this != &other) {
            --state_->ref_count_;
            if (!state_->ref_count_) {
                delete state_;
            }
            state_ = other.state_;
            ++state_->ref_count_;
        }
        return *this;
    }

    void Realloc(size_t new_capacity) {
        if (new_capacity < state_->capacity_) {
            throw std::runtime_error("Trying to realloc less memory than it already has!");
        }

        // realloc
        State* new_state = new State(new_capacity);

        // copy
        for (size_t i = 0; i < state_->size_; i++) {
            new_state->data_[i] = state_->data_[i];
        }
        new_state->size_ = state_->size_;
        
        --state_->ref_count_;
        // clear if necessary
        if (!state_->ref_count_) {
            delete state_;
        }

        state_ = new_state;
    }

    void TryDetach() {
        if (state_->ref_count_ > 1) {
            Realloc(state_->capacity_);
        }
    }

    size_t Size() const {
        return state_->size_;
    }

    State* GetState() const {
        return state_;
    }

    void Resize(size_t new_size) {
        if (new_size > state_->capacity_) {
            Realloc(new_size); // or new_size << 1;
        } else if (new_size > state_->size_) {
            TryDetach();
            // do nothing cuz all elements in state_->data_ are initialized with empty copies
        } else if (new_size == state_->size_) {
            // do absolutely nothing (put it here just in case)
        } else { // new_size < state_->size
            TryDetach();
            for (size_t i = new_size; i < state_->size_; i++) {
                state_->data_[i].clear(); // doesn't change capacity_
            }
        }
        state_->size_ = new_size;
    }

    const std::string& Get(size_t at) const {
        if (at >= state_->size_) {
            throw std::runtime_error("Can't do Get(). Index is out of the range!");
        }
        return state_->data_[at];
    }

    const std::string& Back() const {
        if (!state_->size_) {
            throw std::runtime_error("Can't do Back() to the empty vector!");
        }
        return Get(state_->size_ - 1);
    }

    void PushBack(const std::string& value) {
        if (state_->size_ == state_->capacity_) {
            size_t new_capacity = (state_->capacity_ > 0 ? state_->capacity_ << 1 : 1);
            Realloc(new_capacity);
        } else {
            TryDetach();
        }
        state_->data_[state_->size_] = value;
        ++state_->size_;
    }

    void Set(size_t at, const std::string& value) {
        if (at >= state_->size_) {
            throw std::runtime_error("Can't do Set(). Index is out of the range!");
        }
        TryDetach();
        state_->data_[at] = value;
    }

private:
    State* state_;
};
