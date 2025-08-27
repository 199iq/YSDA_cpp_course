#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

class ListNode;

namespace cache_types {
    using map_type = std::unordered_map<std::string, ListNode*>;
    using map_iterator = map_type::iterator;
}

// storing map_iterator inside ListNode is potentially dangerous cuz of rehash
// so it's better to store keys inside ListNode too
// be careful next time!

// warning! dangling pointers/iterators (again: store key inside ListNode!)

class ListNode {
public:
    ListNode(const std::string& val, cache_types::map_iterator it, ListNode* prev = nullptr, ListNode* next = nullptr)
        : val_(val), it_(it), prev_(prev), next_(next) {
        // it's better to do connection inside List but not ListNode
        if (prev) {
            prev->next_ = this;
        }
        if (next) {
            next->prev_ = this;
        }
    }
    // maybe add other constructors (only with prev/next)

    ~ListNode() = default; // ??

    void SetIterator(cache_types::map_iterator it) {
        it_ = it;
    }

    void SetValue(const std::string& val) {
        val_ = val;
    }

public:
    std::string val_;
    cache_types::map_iterator it_;
    ListNode* prev_;
    ListNode* next_;
};

class List {
public:
    explicit List(ListNode* head = nullptr) : head_(head), tail_(head) {
        if (head == nullptr) {
            size_ = 0;
        } else {
            size_ = 1;
        }
    }

    ~List() {
        while (head_ != nullptr) {
            ListNode* new_head = head_->next_;
            delete head_;
            head_ = new_head;
        }
        head_ = tail_ = nullptr;
    }

    ListNode* GetHead() const {
        return head_;
    }

    ListNode* GetTail() const {
        return tail_;
    }

    size_t GetSize() const {
        return size_;
    }

    void MoveNodeToTheEnd(ListNode* node) {
        if (node == nullptr) {
            throw std::runtime_error("Can't move node = nullptr to the end!");
        }
        if (node == tail_) {
            // we don't need to move it
            return;
        }

        if (node->prev_ != nullptr) {
            node->prev_->next_ = node->next_;
        }
        if (node->next_ != nullptr) {
            node->next_->prev_ = node->prev_;
        }

        node->prev_ = tail_;

        if (node == head_) {
            head_ = node->next_;
        }

        node->next_ = nullptr;
        tail_->next_ = node;
        tail_ = node;
    }

    void PopNode() {
        if (head_ == nullptr) {
            throw std::runtime_error("Can't pop a node from an empty list!");
        }

        --size_;
        
        if (head_ == tail_) {
            // list = one node
            delete head_;
            head_ = tail_ = nullptr;
            return;
        }
        // otherwise list = more than one node
        head_->next_->prev_ = nullptr;
        ListNode* new_head = head_->next_;
        delete head_;
        head_ = new_head;
    }

    ListNode* AddNode(const std::string& val, cache_types::map_iterator it) {
        ListNode* new_node = new ListNode(val, it, tail_); // or ListNode(val, tail_, nullptr);
        if (head_ == nullptr) {
            head_ = new_node;
        }
        tail_ = new_node;
        ++size_;
        return new_node;
    }
private:
    ListNode* head_;
    ListNode* tail_;
    size_t size_; // helper field
};

class LruCache {
public:
    explicit LruCache(size_t max_size) : max_size_(max_size), list_(List()) {} // do we need list_(List()) ?

    LruCache(const LruCache&) = delete;
    LruCache& operator=(const LruCache&) = delete;

    void PopElement() {
        cache_types::map_iterator del_it = list_.GetHead()->it_;
        list_.PopNode();
        cache.erase(del_it);
    }

    void Set(const std::string& key, const std::string& value) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            // got new element
            auto [it, inserted] = cache.emplace(key, nullptr);
            ListNode* node = list_.AddNode(value, it);
            it->second = node;
            // ListNode* new_node = list_.AddNode(value, cache.end()); // cache.end() is just a placeholder here -> think how to improve that
            // cache[key] = new_node;
            // it = cache.find(key);
            // new_node->SetIterator(it); // seems a bit stupid -> fix that later
        } else {
            // move old element to the end
            ListNode* node = cache[key];
            list_.MoveNodeToTheEnd(node);
            node->SetValue(value);
        }

        size_t cur_size = list_.GetSize();
        if (cur_size > max_size_) {
            // pop the oldest element (head)
            PopElement();
        }
    }

    bool Get(const std::string& key, std::string* value) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            // no element with that key
            return false;
        }
        // ListNode* node = cache[key];
        ListNode* node = it->second;
        *value = node->val_;
        list_.MoveNodeToTheEnd(node);
        return true;
    }
private:
    size_t max_size_;
    List list_; // important -> so there won't be dangling pointers (for cache destructor) (but will be dangling iterators!)
    std::unordered_map<std::string, ListNode*> cache;
};
