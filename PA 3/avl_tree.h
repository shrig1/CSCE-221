#pragma once

#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

using std::ostream, std::cout;
using std::invalid_argument;
using std::string;
using std::swap;

/**
 * @brief Adelson-Velsky Landis Tree
 *
 * @tparam Comparable data type that will overload the comparison operators
 */
template <typename Comparable>
class AVLTree {
public:
    /**
     * @brief Internal node struct
     *
     * @tparam T data type that will overload the comparison operators
     */
    struct Node {
        /**
         * @brief Value stored in this node
         */
        Comparable _value;

        /**
         * @brief Largets distance to a leaf node
         */
        size_t _height;

        /**
         * @brief A pointer to the left node
         */
        Node* _left;

        /**
         * @brief A pointer to the right node
         */
        Node* _right;

        /**
         * @brief Construct a new Node object
         *
         * @param value value to put in this node
         */
        Node(const Comparable& value) : _value{Comparable(value)}, _height{1}, _left{nullptr}, _right{nullptr} {}

        /**
         * @brief Determine if this node is a leaf.
         *
         * @return true if this node is a leaf
         * @return false otherwise
         */
        bool is_leaf() const {
            return !this->_left && !this->_right;
        }
    };
private:
    /**
     * @brief A pointer for the root of the AVL Tree.
     */
    Node* _root;

    /**
     * @brief Determine the max of the two values supplied
     *
     * @tparam Type comparable type of inputs
     * @param a first value
     * @param b second value
     * @return Type the max of the two supplied values
     */
    template <typename Type>
    Type max(const Type& a, const Type& b) const { return a > b ? a : b; }

    /**
     * @brief Determine if this AVL tree contains a value
     *
     * @param root current subtree being searched
     * @param value value to find
     * @return true if this tree contains specified value
     * @return false otherwise
     */
    bool contains(Node*& root, const Comparable& value) const {
        if (!root)
            return false;
        if (root->_value == value)
            return true;
        if (root->_value < value)
            return this->contains(root->_right, value);
        return this->contains(root->_left, value);
    }

    /**
     * @brief Prevents memory leaks by deallocating the entire tree
     *
     * @param node current subtree being deallocated
     * @return Node*& reference to nullptr
     */
    Node*& clear(Node*& node) {
        if (node) {
            node->_left = this->clear(node->_left);
            node->_right = this->clear(node->_right);
            delete node;
        }

        node = nullptr;
        return node;
    }

    /**
     * @brief Writes this tree into an ostream.
     *
     * @param root current subtree being written
     * @param os ostream to write to
     * @param trace how much to indent each line
     */
    void print_tree(const Node*& root, ostream& os, size_t trace) const {
        if (!root) {
            os << "<empty>\n";
            return;
        }

        if (root->_right)
            this->print_tree(const_cast<const Node*&>(root->_right), os, trace + 1);
        os << string(trace * 2, ' ') << root->_value << "\n";
        if (root->_left)
            this->print_tree(const_cast<const Node*&>(root->_left), os, trace + 1);
    }

    /**
     * @brief Copies a avl tree into this one
     *
     * @param root current subtree being copies
     * @return Node* a pointer to nodes created
     */
    Node* copy(const Node* root) {
        if (!root)
            return nullptr;

        Node* new_root = new Node(root->_value);
        new_root->_left = this->copy(root->_left);
        new_root->_right = this->copy(root->_right);
        return new_root;
    }

    /**
     * @brief Inserts a value into the avl tree.
     *
     * @param node current subtree being inserted into
     * @param value value to insert
     * @return Node* new node created or node inputed if the value is already in the tree
     */
    Node* insert(Node* node, const Comparable& value) {
        if (!node)
            return new Node(value);
        else if (value == node->_value)
            return node;
        else if (value < node->_value)
            node->_left = this->insert(node->_left, value);
        else if (value > node->_value)
            node->_right = this->insert(node->_right, value);

        node = this->balance(node);
        return node;
    }

    /**
     * @brief Remove a value into the avl tree.
     *
     * @param root current subtree being remove
     * @param value value to remove
     * @return Node*
     */
    Node* remove(Node* root, const Comparable& value) {
        if (!root)
            return nullptr;

        if (value < root->_value)
            root->_left = this->remove(root->_left, value);
        else if (value > root->_value)
            root->_right = this->remove(root->_right, value);
        else {
            if (root->is_leaf()) {
                delete root;
                return nullptr;
            } else if (!root->_left) {
                Node* temp = root->_right;
                delete root;
                root = nullptr;
                return temp;
            } else if (!root->_right) {
                Node* temp = root->_left;
                delete root;
                root = nullptr;
                return temp;
            } else {
                const Node* temp = this->find_min(root->_right);
                root->_value = temp->_value;
                root->_right = this->remove(root->_right, temp->_value);
            }
        }

        root = this->balance(root);
        return root;
    }

    /**
     * @brief Determine the height of a subtree
     *
     * @param root subtree to determine the height of
     * @return long height of the subtree
     */
    long height(const Node* root) const {
        if (!root)
            return 0;

        return root->_height;
    }

    /**
     * @brief Calculates the raw height of a subtree
     *
     * @param root subtree to calculate the height of
     * @return size_t height calculated
     */
    size_t calcHeight(const Node*& root) const {
        return 1 + this->max(this->height(root->_left), this->height(root->_right));
    }

    /**
     * @brief Determine the balance factor of an inputed subtree
     *
     * @param root subtree to determine the balance factor of
     * @return long balance factor
     */
    long balace_factor(const Node*& root) const {
        return !root ?
            0l :
            this->height(const_cast<const Node*&>(root->_left)) - this->height(const_cast<const Node*&>(root->_right));
    }

    /**
     * @brief Right rotate the inputed subtree
     *
     * @param root subtree to rotate
     * @return Node* rotated subtree
     */
    Node* rr_rotate(Node*& root) {
        Node* temp = root->_right;
        root->_right = temp->_left;
        temp->_left = root;

        root->_height = this->calcHeight(const_cast<const Node*&>(root));
        temp->_height = this->calcHeight(const_cast<const Node*&>(temp));

        return temp;
    }

    /**
     * @brief Left rotate the inputed subtree
     *
     * @param root subtree to rotate
     * @return Node* rotated subtree
     */
    Node* ll_rotate(Node*& root) {
        Node* temp = root->_left;
        root->_left = temp->_right;
        temp->_right = root;

        root->_height = this->calcHeight(const_cast<const Node*&>(root));
        temp->_height = this->calcHeight(const_cast<const Node*&>(temp));

        return temp;
    }

    /**
     * @brief Left right rotate
     *
     * @param root subtree to rotate
     * @return Node* rotated subtree
     */
    Node* lr_rotate(Node*& root) {
        Node* temp = root->_left;
        root->_left = this->rr_rotate(temp);
        return this->ll_rotate(root);
    }

    /**
     * @brief Right Left rotate
     *
     * @param root subtree to rotate
     * @return Node* rotated subtree
     */
    Node* rl_rotate(Node*& root) {
        Node* temp = root->_right;
        root->_right = this->ll_rotate(temp);
        return this->rr_rotate(root);
    }

    /**
     * @brief Rebalance the a subtree of the avl tree
     *
     * @param root subtree to rebalance
     * @return Node* rebalanced subtree
     */
    Node* balance(Node*& root) {
        const Node*& left = const_cast<const Node*&>(root->_left);
        const Node*& right = const_cast<const Node*&>(root->_right);

        root->_height = 1 + this->max(this->height(left), this->height(right));
        signed long bf = this->balace_factor(const_cast<const Node*&>(root));
        if (bf > 1)
            root = this->balace_factor(left) > 0 ? ll_rotate(root) : lr_rotate(root);
        else if (bf < -1)
            root = this->balace_factor(right) > 0 ? rl_rotate(root) : rr_rotate(root);
        return root;
    }

    /**
     * @brief Find the smallest node in the avl tree
     *
     * @param root current subtree being searched
     * @return Node* pointer to the smallest node
     */
    Node* find_min(Node*& root) const {
        Node* node = root;
        while (node && node->_left)
            node = node->_left;

        return node;
    }

public:
    /**
     * @brief Construct a new AVLTree object
     */
    AVLTree() : _root{nullptr} {}

    /**
     * @brief Construct a new AVLTree object
     *
     * @param rhs AVLTree to copy from
     */
    AVLTree(const AVLTree& rhs) : _root{this->copy(rhs._root)} {}

    /**
     * @brief Destroy the AVLTree object
     */
    ~AVLTree() { this->make_empty(); }

    /**
     * @brief Copy assignment operator
     *
     * @param rhs AVLTree to copy from
     * @return AVLTree& *this
     */
    AVLTree& operator=(const AVLTree& rhs) {
        if (this != &rhs) {
            this->make_empty();
            this->_root = this->copy(rhs.root());
        }
        return *this;
    }

    /**
     * @brief Determine is this AVL Tree contains a value
     *
     * @param value value to find
     * @return true if the value if found within the AVL Tree
     * @return false otherwise
     */
    bool contains(const Comparable& value) {
        return this->contains(this->_root, value);
    }

    /**
     * @brief Insert a value into the AVL Tree
     *
     * @param value value to insert
     */
    void insert(const Comparable& value) {
        this->_root = this->insert(this->_root, value);
    }

    /**
     * @brief Remove a value from the AVL Tree
     *
     * @param value value to remove
     */
    void remove(const Comparable& value) {
        this->_root = this->remove(this->_root, value);
    }

    /**
     * @brief Find the smallest value in the AVL tree
     *
     * @return const Comparable& smallest value
     */
    const Comparable& find_min() const {
        if (!this->_root)
            throw invalid_argument("AVL Tree is empty");

        Node* root = this->_root;
        while (root->_left)
            root = root->_left;

        return root->_value;
    }

    /**
     * @brief Find the largest value in the AVL tree
     *
     * @return const Comparable& largest value
     */
    const Comparable& find_max() const {
        if (!this->_root)
            throw invalid_argument("AVL Tree is empty");

        Node* root = this->_root;
        while (root->_right)
            root = root->_right;

        return root->_value;
    }

    /**
     * @brief Writes this AVL tree to an ostream
     *
     * @param os ostream to write to. Defaulted to cout
     */
    void print_tree(ostream& os = cout) const {
        size_t i = 0;
        this->print_tree(const_cast<const Node*&>(this->_root), os, i);
    }

    /**
     * @brief Return the root of the tree
     *
     * @return const Node*& this->_root;
     */
    const Node* root() const { return this->_root; }

    // ----------------------- Optional ----------------------- //

    AVLTree(AVLTree&& rhs) : _root{rhs.root()} { rhs._root = nullptr; }

    AVLTree& operator=(AVLTree&& rhs) {
        if (this != &rhs) {
            this->_root = this->clear(this->_root);
            swap(this->_root, rhs._root);
        }

        return *this;
    }

    void insert(Comparable&& value) {
        Comparable v;
        swap(v, value);
        this->_root = this->insert(this->_root, v);
    }

    /**
     * @brief Determine if this AVL tree is empty
     *
     * @return true if this tree is empty
     * @return false otherwise
     */
    bool is_empty() const { return !this->_root; }

    /**
     * @brief Prevents memory errors by deallocating the entire AVL tree
     */
    void make_empty() { this->_root = this->clear(this->_root); }
};
