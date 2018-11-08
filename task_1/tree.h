#pragma once

#include <memory>

namespace bintree {
    template <typename T>
    struct TNode : std::enable_shared_from_this<TNode<T>> {
        using TNodePtr = std::shared_ptr<TNode<T>>;
        using TNodeWeakPtr = std::weak_ptr<TNode<T>>;
        using TNodeConstPtr = std::shared_ptr<const TNode<T>>;

        bool hasLeft() const {
            return bool(left);
        }

        bool hasRight() const {
            return bool(right);
        }

        bool hasParent() const {
            return bool(parent);
        }

        T& getValue() {
            return value;
        }

        const T& getValue() const {
            return value;
        }

        TNodePtr getLeft() {
            return left;
        }

        TNodeConstPtr getLeft() const {
            return left;
        }

        TNodePtr getRight() {
            return right;
        }

        TNodeConstPtr getRight() const {
            return right;
        }

        TNodePtr getParent() {
            return parent.lock();
        }

        TNodeConstPtr getParent() const {
            return parent.lock();
        }

        static TNodePtr createLeaf(T v) {
            auto obj = new TNode(v);
            auto ptr = std::shared_ptr<TNode>(obj);
            return ptr;
        }

        static TNodePtr fork(T v, TNode* left, TNode* right) {
            auto obj = new TNode(v, left, right);
            auto ptr = std::shared_ptr<TNode>(obj);
            setParent(ptr->getLeft(), ptr);
            setParent(ptr->getRight(), ptr);
            return ptr;
        }

        TNodePtr replaceLeft(TNodePtr l) {
            setParent(l, this->shared_from_this());
            setParent(left, nullptr);
            std::swap(l, left);
            return l;
        }

        TNodePtr replaceRight(TNodePtr r) {
            setParent(r, this->shared_from_this());
            setParent(right, nullptr);
            std::swap(r, right);
            return r;
        }

        TNodePtr replaceRightWithLeaf(T v) {
            return replaceRight(createLeaf(v));
        }

        TNodePtr replaceLeftWithLeaf(T v) {
            return replaceLeft(createLeaf(v));
        }

        TNodePtr removeLeft() {
            return replaceLeft(nullptr);
        }
        TNodePtr removeRight() {
            return replaceRight(nullptr);
        }

    private:
        T value;
        TNodePtr left = nullptr;
        TNodePtr right = nullptr;
        TNodeWeakPtr parent;

        TNode(T v)
            : value(v)
        {}

        // implicit conversion to shared_ptr didn't take into account shared_from_this
        // btw, ru.cppreference contains wrong information
        TNode(T v, TNode* left, TNode* right)
            : value(v)
            , left(left ? left->shared_from_this() : TNodePtr{nullptr})
            , right(right ? right->shared_from_this() : TNodePtr{nullptr})
        {}

        static void setParent(TNodePtr node, TNodePtr parent) {
            if (node)
                node->parent = parent;
        }
    };
}