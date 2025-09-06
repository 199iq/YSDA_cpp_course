#pragma once

#include <cstddef>
#include <iterator>
#include <iostream>

struct BaseNode {
    BaseNode* prev = this;
    BaseNode* next = this;
};

template<typename T>
struct ListNode : public BaseNode {
    T val;

    template<typename... Args>
    explicit ListNode(Args&&... args)
        : val(std::forward<Args>(args)...) {}
};

template <class T>
class List {
public:
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(BaseNode* iter = nullptr) : iter_(iter) {}

        Iterator& operator++() {
            iter_ = iter_->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator--() {
            iter_ = iter_->prev;
            return *this;
        }

        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        T& operator*() const {
            return static_cast<ListNode<T>*>(iter_)->val;
        }

        T* operator->() const {
            return &(static_cast<ListNode<T>*>(iter_)->val);
        }

        bool operator==(const Iterator& other) const {
            return iter_ == other.iter_;
        }

        bool operator!=(const Iterator& other) const {
            return iter_ != other.iter_;
        }

        ListNode<T>* GetNode() const {
            return static_cast<ListNode<T>*>(iter_);
        }

    private:
        BaseNode* iter_;
    };

public:
    List()
        : size_(0) {
            head_ = new BaseNode();
    }

    List(const List& other)
        : size_(0) {
            head_ = new BaseNode();

            // otherwise copy nodes
            BaseNode* node = other.head_->next;
            while (node != other.head_) {
                PushBack(static_cast<ListNode<T>*>(node)->val);
                node = node->next;
            }
    }

    List(List&& other)
        : head_(other.head_), size_(other.size_) {
            other.head_ = new BaseNode();
            other.size_ = 0;
    }

    ~List() {
        DestroyList();
        delete head_;
    }

    List& operator=(const List& other) {
        if (this != &other) {
            DestroyList();
            List tmp = List(other);
            Swap(tmp);
        }
        return *this;
    }

    List& operator=(List&& other) {
        if (this != &other) {
            DestroyList();
            List tmp = List(std::move(other)); 
            Swap(tmp);
        }
        return *this;
    }

public:
    void Swap(List& other) {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
    }

    bool IsEmpty() const {
        return (size_ == 0);
    }
    size_t Size() const {
        return size_;
    }

    void PushBack(const T& val) {
        ListNode<T>* new_node = new ListNode<T>(val);
        LinkAfter(new_node, head_->prev);

    }

    void PushBack(T&& val) {
        ListNode<T>* new_node = new ListNode<T>(std::move(val));
        LinkAfter(new_node, head_->prev);
    }

    void PushFront(const T& val) {
        ListNode<T>* new_node = new ListNode<T>(val);
        LinkAfter(new_node, head_);
    }

    void PushFront(T&& val) {
        ListNode<T>* new_node = new ListNode<T>(std::move(val));
        LinkAfter(new_node, head_);
    }

    T& Front() {
        if (!size_) {
            throw std::runtime_error("Can't do Front. List is empty!");
        }
        return static_cast<ListNode<T>*>(head_->next)->val;
    }

    const T& Front() const {
        if (!size_) {
            throw std::runtime_error("Can't do Front. List is empty!");
        }
        return static_cast<ListNode<T>*>(head_->next)->val;
    }

    T& Back() {
        if (!size_) {
            throw std::runtime_error("Can't do Back. List is empty!");
        }
        return static_cast<ListNode<T>*>(head_->prev)->val;
    }
    const T& Back() const {
        if (!size_) {
            throw std::runtime_error("Can't do Back. List is empty!");
        }
        return static_cast<ListNode<T>*>(head_->prev)->val;
    }

    void PopBack() {
        if (!size_) {
            throw std::runtime_error("Can't do PopBack. List is empty!");
        }
        EraseNode(head_->prev);
    }

    void PopFront() {
        if (!size_) {
            throw std::runtime_error("Can't do PopFront. List is empty!");
        }
        EraseNode(head_->next);
    }

    void EraseNode(BaseNode* node) {
        Unlink(node);
        delete static_cast<ListNode<T>*>(node);
    }

    void Erase(Iterator item) {
        ListNode<T>* node = item.GetNode();
        EraseNode(node);
    }

    Iterator Begin() const {
        return Iterator(head_->next);
    }

    Iterator End() const {
        return Iterator(head_);
    }
public:
    // debug functions
    void PrintList() {
        std::cout << "List with Size = " << Size() << " is\n";
        for (auto it = Begin(); it != End(); it++) {
            std::cout << *it << " ";
        }
        std::cout << "\n-----------------------------------\n";
    }

private:
    BaseNode* head_;
    size_t size_;
private:
    void DestroyList() {
        BaseNode* node = head_->next;
        while (node != head_) {
            BaseNode* next_node = node->next;
            EraseNode(node);
            node = next_node;
        }
    }

    void Unlink(BaseNode* node) {
        node->next->prev = node->prev;
        node->prev->next = node->next;
        --size_;
    }

    void LinkAfter(BaseNode* node, BaseNode* after) {
        after->next->prev = node;
        node->next = after->next;
        node->prev = after;
        after->next = node;
        ++size_;
    }
};

template <class T>
List<T>::Iterator begin(List<T>& lst) {
    return lst.Begin();
}

template <class T>
List<T>::Iterator end(List<T>& lst) {
    return lst.End();
}
