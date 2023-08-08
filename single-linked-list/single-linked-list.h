#pragma once

#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iterator>


template <typename Type>
class SingleLinkedList {
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
                : value(val)
                , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;
        explicit BasicIterator(Node* node)
                : node_(node)
        {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept
                : node_(other.node_)
        {
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        template <typename ListType> // ListType == Type, const Type
        [[nodiscard]] bool operator==(const BasicIterator<ListType>& rhs) const noexcept {
            return rhs.node_ == node_;
        }

        template <typename ListType> // ListType == Type, const Type
        [[nodiscard]] bool operator!=(const BasicIterator<ListType>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator pre_it = *this;
            ++(*this);
            return pre_it;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator();
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return IsEmpty() ? ConstIterator() : ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator();
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return begin();
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return end();
    }

    SingleLinkedList()
            : size_(0)
    {
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            delete std::exchange(head_.next_node, head_.next_node->next_node);
            --size_;
        }
    }

    ~SingleLinkedList() {
        Clear();
    }

    SingleLinkedList(std::initializer_list<Type> values)
            : SingleLinkedList()
    {
        createList(*this, values);
    }

    SingleLinkedList(const SingleLinkedList& other)
            : SingleLinkedList()
    {
        // Сначала надо удостовериться, что текущий список пуст
        assert(size_ == 0 && head_.next_node == nullptr);
        createList(*this, other);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (rhs.head_.next_node != head_.next_node) {
            SingleLinkedList CopyList(rhs);
            swap(CopyList);
        }
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);

        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator(new_node);
    }

    void PopFront() noexcept {
        assert(head_.next_node != nullptr);

        delete std::exchange(head_.next_node, head_.next_node->next_node);
        --size_;

    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr);
        assert(pos.node_->next_node != nullptr);

        delete std::exchange(pos.node_->next_node, pos.node_->next_node->next_node);
        --size_;

        return Iterator(pos.node_->next_node);
    }

private:
    // Фиктивный узел, используется для вставки "перед первым элементом"
    Node head_;
    size_t size_;

    template <typename ContainerForCopy>
    void createList(SingleLinkedList& target, const ContainerForCopy& container) {
        SingleLinkedList tmp;
        Node* current_node = nullptr;
        bool isFirst = true;
        for (auto val : container) {
            ++tmp.size_;
            if (isFirst) {
                tmp.head_.next_node = new Node(val, nullptr);
                current_node = tmp.head_.next_node;
                isFirst = false;
                continue;
            }
            current_node->next_node = new Node(val, nullptr);
            current_node = current_node->next_node;
        }
        target.swap(tmp);
    }
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
