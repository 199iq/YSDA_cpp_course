#pragma once

#include <cstddef>
#include <iostream>
#include <vector>

class RingBuffer {
public:
    explicit RingBuffer(size_t _capacity) 
        : capacity(_capacity) {
            buf.resize(capacity);
            tail = head = cnt = 0;
    }

    size_t Size() const {
        return cnt;
    }

    bool Empty() const {
        return !cnt;
    }

    bool TryPush(int element) {
        if (cnt == capacity) {
            return false; // buffer is full
        }
        buf[tail] = element;
        tail = (tail + 1) % capacity;
        cnt++;
        return true;
    }

    bool TryPop(int* element) {
        if (!cnt) {
            return false;
        }
        *element = buf[head];
        head = (head + 1) % capacity;
        cnt--;
        return true;
    }
private:
    std::vector<int> buf;
    size_t head, tail, cnt, capacity;
};
