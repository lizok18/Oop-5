#ifndef LIST_H
#define LIST_H

#include <memory_resource>
#include <iterator>
#include <utility>
#include <cstddef>

template <typename T>
class DoublyLinkedList
{
private:
    struct Node
    {
        T value;
        Node *prev;
        Node *next;

        template <typename... Args>
        Node(Args &&...args)
            : value(std::forward<Args>(args)...), prev(nullptr), next(nullptr) {}
    };

public:
    struct iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        Node *node;
        iterator(Node *n = nullptr) : node(n) {}

        reference operator*() const { return node->value; }
        pointer operator->() const { return &node->value; }

        iterator &operator++()
        {
            if (node)
                node = node->next;
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator &other) const { return node == other.node; }
        bool operator!=(const iterator &other) const { return node != other.node; }
    };

    struct const_iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;

        const Node *node;
        const_iterator(const Node *n = nullptr) : node(n) {}
        const_iterator(const iterator &it) : node(it.node) {}

        reference operator*() const { return node->value; }
        pointer operator->() const { return &node->value; }

        const_iterator &operator++()
        {
            if (node)
                node = node->next;
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const const_iterator &other) const { return node == other.node; }
        bool operator!=(const const_iterator &other) const { return node != other.node; }
    };

    using allocator_type = std::pmr::polymorphic_allocator<Node>;

    explicit DoublyLinkedList(std::pmr::memory_resource *mr = std::pmr::get_default_resource())
        : alloc_(mr), head_(nullptr), tail_(nullptr), size_(0) {}

    ~DoublyLinkedList() { clear(); }
    
    DoublyLinkedList(const DoublyLinkedList &) = delete;
    DoublyLinkedList &operator=(const DoublyLinkedList &) = delete;

    DoublyLinkedList(DoublyLinkedList &&other) noexcept
        : alloc_(other.alloc_), head_(other.head_), tail_(other.tail_), size_(other.size_)
    {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    DoublyLinkedList &operator=(DoublyLinkedList &&other) noexcept
    {
        if (this != &other)
        {
            clear();
            
        
            
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;

            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    template <typename U>
    void push_back(U &&value)
    {
        Node *n = allocate_node(std::forward<U>(value));
        if (!tail_)
            head_ = tail_ = n;
        else
        {
            tail_->next = n;
            n->prev = tail_;
            tail_ = n;
        }
        size_++;
    }

    template <typename U>
    void push_front(U &&value)
    {
        Node *n = allocate_node(std::forward<U>(value));
        if (!head_)
            head_ = tail_ = n;
        else
        {
            n->next = head_;
            head_->prev = n;
            head_ = n;
        }
        size_++;
    }

    iterator insert(iterator pos, const T& value)
    {
        if (pos == end())
        {
            push_back(value);
            return iterator(tail_);
        }
        
        Node *n = allocate_node(value);
        Node *curr = pos.node;
        
        n->prev = curr->prev;
        n->next = curr;
        
        if (curr->prev)
            curr->prev->next = n;
        else
            head_ = n;
            
        curr->prev = n;
        size_++;
        
        return iterator(n);
    }

    iterator erase(iterator pos)
    {
        if (pos == end()) return end();
        
        Node *curr = pos.node;
        Node *next_node = curr->next;
        
        if (curr->prev)
            curr->prev->next = curr->next;
        else
            head_ = curr->next;
            
        if (curr->next)
            curr->next->prev = curr->prev;
        else
            tail_ = curr->prev;
            
        destroy_node(curr);
        size_--;
        
        return iterator(next_node);
    }

    void pop_front()
    {
        erase(begin());
    }

    void pop_back()
    {
        if (!tail_) return;
        erase(iterator(tail_));
    }

    void clear()
    {
        Node *cur = head_;
        while (cur)
        {
            Node *next = cur->next;
            destroy_node(cur);
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    T& front() { return head_->value; }
    const T& front() const { return head_->value; }
    
    T& back() { return tail_->value; }
    const T& back() const { return tail_->value; }

    iterator begin() { return iterator(head_); }
    iterator end() { return iterator(nullptr); }

    const_iterator begin() const { return const_iterator(head_); }
    const_iterator end() const { return const_iterator(nullptr); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

    allocator_type get_allocator() const { return alloc_; }
    
    // Получение указателя на memory_resource
    std::pmr::memory_resource* get_memory_resource() const {
        return alloc_.resource();
    }

private:
    template <typename U>
    Node *allocate_node(U &&value)
    {
        Node *p = alloc_.allocate(1);
        try
        {
            std::allocator_traits<allocator_type>::construct(alloc_, p, std::forward<U>(value));
        }
        catch (...)
        {
            alloc_.deallocate(p, 1);
            throw;
        }
        return p;
    }

    void destroy_node(Node *p)
    {
        std::allocator_traits<allocator_type>::destroy(alloc_, p);
        alloc_.deallocate(p, 1);
    }

    allocator_type alloc_;
    Node *head_;
    Node *tail_;
    std::size_t size_;
};

#endif // LIST_H