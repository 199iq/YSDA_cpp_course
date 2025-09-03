#pragma once

#include <cstddef>
#include <initializer_list>
#include <deque>
#include <iostream>

static constexpr size_t BLOCK_SIZE = 512 / sizeof(int); // 512 byte / 4 byte (int) = 128 int's

size_t divide(size_t a, size_t b) {
    return (a + b - 1) / b;
}

class DequeBlock{
public:
    DequeBlock() {
        block_ = new int[BLOCK_SIZE];
        // fill with zeros
        ClearBlock();
    }

    ~DequeBlock() {
        delete[] block_;
    }

public:
    void Add(size_t idx, int val) {
        // adds element to start/end of the block
        if (idx >= BLOCK_SIZE) {
            throw std::runtime_error("Can't add new element. Index is out of range!");
        }
        *(block_ + idx) = val;
    }

    void Remove(size_t idx) {
        // removes element from start/end of the block
        if (idx >= BLOCK_SIZE) {
            throw std::runtime_error("Can't remove element. Index is out of range!");
        }
        // set to zero
        *(block_ + idx) = 0;
    }

    int& Get(size_t idx) {
        return *(block_ + idx);
    }

    const int& Get(size_t idx) const {
        return *(block_ + idx);
    }

    void ClearBlock() {
        for (size_t i = 0; i < BLOCK_SIZE; i++) {
            block_[i] = 0;
        }
    }

private:
    int* block_;
};

class Deque {
public:
    Deque()
        : blocks_(nullptr), start_idx_(0), end_idx_(0), capacity_(0), size_(0), begin_(0), end_(0) {
    }

    explicit Deque(size_t size)
        : capacity_(divide(size, BLOCK_SIZE)), size_(size) {
            if (!size) {
                blocks_ = nullptr;
                start_idx_ = end_idx_ = capacity_ = size_ = begin_ = end_ = 0;
                return;
            }

            blocks_ = new DequeBlock*[capacity_]();
            for (size_t i = 0; i < capacity_; i++) {
                blocks_[i] = new DequeBlock();
            }
            begin_ = 0;
            end_ = capacity_ - 1;

            start_idx_ = 0;
            end_idx_ = (size - 1) % BLOCK_SIZE;
    }

    Deque(const Deque& other)
        : Deque(other.capacity_ * BLOCK_SIZE) { // delegating constructors
            start_idx_ = other.start_idx_;
            end_idx_ = other.end_idx_;
            capacity_ = other.capacity_;
            size_ = other.size_;
            begin_ = other.begin_;
            end_ = other.end_;

            size_t gen_offset = 0, block_offset = 0;

            // it would be more optimal if we traverse only over significant blocks/cells
            // but the complexity won't change -> it's ok
            for (size_t i = 0; i < capacity_ * BLOCK_SIZE; i++) {
                gen_offset += block_offset / BLOCK_SIZE;
                block_offset %= BLOCK_SIZE;

                blocks_[gen_offset]->Add(block_offset, other.blocks_[gen_offset]->Get(block_offset));
                ++block_offset;
            }
    }

    Deque(Deque&& other)
        : blocks_(other.blocks_), start_idx_(other.start_idx_), end_idx_(other.end_idx_),
          capacity_(other.capacity_), size_(other.size_), begin_(other.begin_), end_(other.end_) {
            other.blocks_ = nullptr;
            other.start_idx_ = other.end_idx_ = other.size_ = other.capacity_ = other.begin_ = other.end_ = 0;
    }

    Deque(std::initializer_list<int> list)
        : Deque(list.size()) {
            size_ = list.size();
            size_t gen_offset = 0, block_offset = 0;

            for (size_t i = 0; i < size_; i++) {
                gen_offset += block_offset / BLOCK_SIZE;
                block_offset %= BLOCK_SIZE;

                blocks_[gen_offset]->Add(block_offset, list.begin()[i]);
                ++block_offset;
            }
            // start_idx_/end_idx_ and begin_/end_ are already set in the Deque(size) constructor!
    }

    Deque& operator=(const Deque& other) {
        if (this != &other) {
            Deque tmp(other);
            Swap(tmp);
        }
        return *this;
    }

    Deque& operator=(Deque&& other) {
        if (this != &other) {
            Deque tmp(std::move(other));
            Swap(tmp);
        }
        return *this;
    }

    void Swap(Deque& other) {
        std::swap(start_idx_, other.start_idx_);
        std::swap(end_idx_, other.end_idx_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(capacity_, other.capacity_);
        std::swap(size_, other.size_);
        std::swap(blocks_, other.blocks_);
    }

    ~Deque() {
        DestroyTable();
    }

public:
    void PushBack(int val) {
        if (!size_) {
            // table is completely empty
            if (!capacity_) {
                Reallocate(1); // new_capacity = 1
            }
            start_idx_ = end_idx_ = 0;
            begin_ = end_ = 0;
            blocks_[end_]->Add(end_idx_, val);
            ++size_;
            return;
        }

        // otherwise there is at least 1 element in the table
        ++size_;
        if (end_idx_ != BLOCK_SIZE - 1) { // we got some place in the last block
            ++end_idx_;
            blocks_[end_]->Add(end_idx_, val);
            return;
        }

        // otherwise we need a new block
        if (IsTableFull()) { // we need to reallocate the table
            size_t new_capacity = (capacity_ ? capacity_ << 1 : 1);
            Reallocate(new_capacity);
        }
        end_ = GetNextIdx(end_);

        end_idx_ = 0; // ALWAYS ! (end_idx_ starts from the beginning of the block)
        blocks_[end_]->Add(end_idx_, val);
    }

    void PopBack() {
        blocks_[end_]->Remove(end_idx_);
        --size_;

        if (end_idx_ != 0) { // we are not leaving the last block free
            --end_idx_; 
            return;
        }
        // otherwise we are gonna clear the last block (return it to the buffer)
        end_ = GetPrevIdx(end_);
        end_idx_ = BLOCK_SIZE - 1;
        // when start_idx_ > end_idx_ is not a problem cuz when we do another push_back or push_front
        // everything will come to normal
    }

    void PushFront(int val) {
        if (!size_) {
            // table is completely empty
            if (!capacity_) {
                Reallocate(1); // new_capacity = 1
            }
            start_idx_ = end_idx_ = BLOCK_SIZE - 1;
            begin_ = end_ = 0;
            blocks_[begin_]->Add(start_idx_, val);
            ++size_;
            return;
        }

        // otherwise there is at least one element in the table
        ++size_;
        if (start_idx_ != 0) { // we got some place in the first block
            --start_idx_;
            blocks_[begin_]->Add(start_idx_, val);
            return;
        }

        // otherwise we need a new block
        if (IsTableFull()) { // we need to reallocate the table
            size_t new_capacity = (capacity_ ? capacity_ << 1 : 1);
            Reallocate(new_capacity);
        }

        begin_ = GetPrevIdx(begin_);
        start_idx_ = BLOCK_SIZE - 1; // ALWAYS! (start_idx_ starts from the ending of the block)
        blocks_[begin_]->Add(start_idx_, val);
    }

    void PopFront() {
        blocks_[begin_]->Remove(start_idx_);
        --size_;

        if (start_idx_ != BLOCK_SIZE - 1) { // we are not leaving the first block free
            ++start_idx_;
            return;
        }
        // otherwise we are gonna clear the first block (return it to the buffer)
        begin_ = GetNextIdx(begin_);
        start_idx_ = 0;
    }

    int& operator[](size_t idx) {
        if (idx >= size_) {
            throw std::runtime_error("Can't access index idx, it is out of range!");
        }

        size_t start_block_size = BLOCK_SIZE - start_idx_;
        if (idx < start_block_size) {
            // element is in the first block
            return blocks_[begin_]->Get(start_idx_ + idx);
        }

        size_t new_idx = idx - start_block_size;
        size_t gen_offset = new_idx / BLOCK_SIZE + 1;
        new_idx %= BLOCK_SIZE;

        return blocks_[(begin_ + gen_offset) % capacity_]->Get(new_idx);
    }

    const int& operator[](size_t idx) const {
        if (idx >= size_) {
            throw std::runtime_error("Can't access index idx, it is out of range!");
        }

        size_t start_block_size = BLOCK_SIZE - start_idx_;
        if (idx < start_block_size) {
            // element is in the first block
            return blocks_[begin_]->Get(start_idx_ + idx);
        }

        size_t new_idx = idx - start_block_size;
        size_t gen_offset = new_idx / BLOCK_SIZE + 1;
        new_idx %= BLOCK_SIZE;

        return blocks_[(begin_ + gen_offset) % capacity_]->Get(new_idx);
    }

    size_t Size() const {
        return size_;
    }

    void Clear() {
        // for each block call ClearBlock
        // it's not very optimal to move through all the blocks
        // but complexity won't change so i guess it's ok
        // note for the future: try to make it more optimal
        for (size_t i = 0; i < capacity_; i++) {
            blocks_[i]->ClearBlock();
        }
        start_idx_ = end_idx_ = begin_ = end_ = size_ = 0;
    }

private:
    DequeBlock** blocks_;
    size_t start_idx_, end_idx_, capacity_, size_;
    size_t begin_, end_; // blockwise offsets (like ringbuffer)
    // size_ = number of elements in the deque
    // capacity_ = total number of blocks reserved
private:
    // ------------ Ring buffer methods ------------
    size_t GetPrevIdx(size_t idx) const { // GO LEFT
        return (idx + capacity_ - 1) % capacity_;
    }

    size_t GetNextIdx(size_t idx) const { // GO RIGHT
        return (idx + 1) % capacity_;
    }

    bool IsTableFull() const {
        return GetNextIdx(end_) == begin_;
    }

    // ------------ Memory handling ------------

    void DestroyTable() {
        for (size_t i = 0; i < capacity_; i++) {
            delete blocks_[i];
        }
        delete[] blocks_;
        blocks_ = nullptr;
    }

    void Reallocate(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            throw std::runtime_error("Trying to reallocate less memory than it already has!");
        }
        DequeBlock** new_blocks = new DequeBlock*[new_capacity]();

        size_t iter = begin_;
        // copy old blocks
        for (size_t i = 0; i < capacity_; i++) {
            new_blocks[i] = blocks_[iter]; // make the right order
            iter = GetNextIdx(iter);
        }

        // create new blocks
        for (size_t i = capacity_; i < new_capacity; i++) {
            new_blocks[i] = new DequeBlock();
        }

        delete[] blocks_; // destroy pointers

        blocks_ = new_blocks;
        begin_ = 0;
        end_ = (capacity_ ? capacity_ - 1 : 0);
        capacity_ = new_capacity;
        // note that start_idx_ and end_idx_ should not change (we change only blocks order)
    }
};