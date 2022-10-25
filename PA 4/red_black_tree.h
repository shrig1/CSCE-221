#pragma once

#include <cstddef>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <tuple>

using std::cout, std::ostream;
using std::invalid_argument;
using std::string, std::wstring;
using std::swap, std::move;

/**
 * @brief A self-balancing Binary Search Tree.
 *
 * @tparam Comparable a data type that overloads the comparison operators
 */
template <typename Comparable>
class RedBlackTree {
public:
    /**
     * @brief An Extra bit that is used to ensure that the tree remains balanced during insertions and deletions.
     */
    enum Color { RED = 0, BLACK = 1 };

    /**
     * @brief Internal Node structure
     */
    struct Node {
        /**
         * @brief value stored in the node
         */
        Comparable value;

        /**
         * @brief The Color of the node
         */
        Color color;

        /**
         * @brief A pointer to the left child
         */
        Node* left;

        /**
         * @brief A pointer to the right child
         */
        Node* right;

        /**
         * @brief A pointer to the parent
         */
        Node* parent;

        /**
         * @brief Construct a new Node object
         *
         * @param value value to store in the node
         */
        Node(const Comparable& value) : Node(value, Color::RED) {}

        /**
         * @brief Construct a new Node object
         *
         * @param value value to store in the node
         * @param color Color of the node
         */
        Node(const Comparable& value, const Color& color) : value{Comparable(value)}, color{color}, left{nullptr}, right{nullptr}, parent{nullptr} {}

        /**
         * @brief flushed this node to an ostream
         *
         * @param os ostream to flush to
         * @param node node to flush
         * @return ostream& ostream to flush to
         */
        friend ostream& operator<<(ostream& os, const Node& node) {
            switch (node.color) {
            case Color::RED:
                os << "🟥";
                break;
            case Color::BLACK:
                os << "⬛";
                break;
            default:
                os << "🟦";
                break;
            }
            return os << ": " << node.value;
        }

        /**
         * @brief Determines if this node is the left child of its parrent
         *
         * @return true if this node is the left child of its parrent
         * @return false otherwise
         */
        bool isLeft() const { return this->parent ? this == this->parent->left : false; }

        /**
         * @brief Determines if the node is the right child of its parrent
         *
         * @return true if this node it the right child of its parrent
         * @return false otherwise
         */
        bool isRight() const { return this->parent ? this == this->parent->right : false; }

        bool isLeaf() const { return !this->left && !this->right; }

        bool hasColorChild(const Color& color) const { return (this->left && this->left->color == color) || (this->right && this->right->color == color); }

        bool hasColorChildren(const Color& color) const { return (this->isLeaf() && color == Color::BLACK) || ((this->left && this->left->color == color) && (this->right && this->right->color == color)); }

        /**
         * @brief Gets the sibling of this node
         *
         * @return Node* pointer to the sibling of this node
         */
        Node* sibling() const {
            if (!this->parent)
                return nullptr;
            return this->isLeft() ? this->parent->right : this->parent->left;
        }
    };
private:
    /**
     * @brief a pointer to the root of the 🟥⬛ Tree
     */
    Node* _root;

    /**
     * @brief Rotate leftwards arround an inputed node
     *
     * @param node a pointer to the root to rotate arround
     */
    Node* rotateLeft(Node* node) {
        // cout << "Rotate left on " << *node << "\n";
        Node* temp = node->right;
        node->right = temp->left;
        if (temp->left)
            temp->left->parent = node;
        temp->parent = node->parent;
        if (!node->parent)
            this->_root = temp;
        else if (node->isLeft())
            node->parent->left = temp;
        else if (node->isRight())
            node->parent->right = temp;
        temp->left = node;
        node->parent = temp;
        return temp;
    }

    /**
     * @brief Rotate rightwards arround an inputed node
     *
     * @param node a pointer to the root to rotates arround
     */
    Node* rotateRight(Node* node) {
        // cout << "Rotate right on " << *node << "\n";
        Node* temp = node->left;
        node->left = temp->right;
        if (temp->right)
            temp->right->parent = node;
        temp->parent = node->parent;
        if (!node->parent)
            this->_root = temp;
        else if (node->isRight())
            node->parent->right = temp;
        else if (node->isLeft())
            node->parent->left = temp;
        temp->right = node;
        node->parent = temp;
        return temp;
    }

    /**
     * @brief Search for a node in a subtree
     *
     * @param root current subtree being searched
     * @param value value to search for
     * @return Node* a pointer to the node with the value found. Or nullptr if value is not found.
     */
    Node* search(Node* root, const Comparable& value) const {
        if (!root)
            return nullptr;
        if (root->value == value)
            return root;
        return root->value < value ? this->search(root->right, value) : this->search(root->left, value);
    }

    /**
     * @brief Prevents memory leaks by deallocated a subtree
     *
     * @param root subtree currently being deallocated
     * @return Node* nullptr
     */
    Node* clear(Node*& root) {
        if (root) {
            root->left = this->clear(root->left);
            root->right = this->clear(root->right);
            root->parent = nullptr;
            delete root;
        }

        root = nullptr;
        return nullptr;
    }

    /**
     * @brief Makes a copy of a subtree
     *
     * @param root subtree being coppied from
     * @return Node* new subtree created.
     */
    Node* copy(const Node* root) {
        if (!root)
            return nullptr;

        Node* node = new Node(root->value, root->color);
        node->left = this->copy(root->left);
        if (node->left)
            node->left->parent = node;
        node->right = this->copy(root->right);
        if (node->right)
            node->right->parent = node;
        return node;
    }

    /**
     * @brief Searched for the node with the smallest value in a subtree
     *
     * @param root subtree to search through
     * @return Node* a pointer to the node with the smallest value
     */
    Node* find_min(Node* root) const {
        Node* node = root;
        while (node && node->left)
            node = node->left;
        return node;
    }

    void removeNode(Node*& node) {
        if (node->isLeaf()) {
            if (node->isLeft())
                node->parent->left = nullptr;
            else
                node->parent->right = nullptr;
            delete node;
            node = nullptr;
            return;
        }

        Node* replace = node->right ? this->find_min(node->right) : node->left;
        swap(node->value, replace->value);
        this->remove(replace, replace->value);
    }

    void remove(Node* node, const Comparable& value) {
        if (!node)
            return;
        if (node->hasColorChildren(Color::BLACK)) {
            Node* sibling = node->sibling();
            if (!sibling) {
                if (node->value == value)
                    this->removeNode(node);
                else
                    this->remove(node->value < value ? node->right : node->left, value);
                return;
            } else if (sibling->hasColorChildren(Color::BLACK)) {
                node->parent->color = Color::BLACK;
                sibling->color = node->color = Color::RED;
                if (node->value == value)
                    this->removeNode(node);
                else
                    this->remove(node->value < value ? node->right : node->left, value);
                return;
            }

            Node* closeNibling = node->isLeft() ? sibling->right : sibling->left;
            Node* distantNibling = closeNibling->sibling();
            if (closeNibling->color == Color::RED) {
                sibling = closeNibling->isLeft() ? this->rotateRight(sibling) : this->rotateLeft(sibling);
                sibling = sibling->isLeft() ? this->rotateRight(node->parent) : this->rotateLeft(node->parent);
                node->color = Color::RED;
                node->parent->color = Color::BLACK;
            } else if (distantNibling->color == Color::RED) {
                sibling = sibling->isLeft() ? this->rotateRight(node->parent) : this->rotateLeft(node->parent);
                node->color = sibling->color = Color::RED;
                node->parent->color = distantNibling->color = Color::BLACK;
            }
            this->remove(node->value < value ? node->right : node->left, value);
        } else if (node->hasColorChild(Color::RED)) {
            if (node->color == Color::BLACK) {
                if (node->isRight())
                    this->rotateRight(node->parent);
                else if (node->isLeft())
                    this->rotateLeft(node->parent);
                node->color = Color::BLACK;
                if (node != this->_root) {
                    node->parent->parent->color = Color::BLACK;
                    node->parent->color = Color::RED;
                }
            }
        }

        if (node->value == value)
            this->removeNode(node);
        else
            this->remove(node->value < value ? node->right : node->left, value);
    }

    void insert(Node* node, const Comparable& value) {
        if (!this->_root) {
            this->_root = new Node(value, Color::BLACK);
            return;
        } else if (node->value == value)
            return;
        else if (node->value > value) {
            if (!node->left) {
                node->left = new Node(value);
                node->left->parent = node;

                if (node->color == Color::BLACK)
                    return;
                if (!node->sibling()) {
                    if (node->isLeft())
                        node = this->rotateRight(node->parent);
                    else {
                        node = this->rotateRight(node);
                        node = this->rotateLeft(node->parent);
                    }
                    node->color = Color::BLACK;
                    node->left->color = node->right->color = Color::RED;
                    return;
                }
            }
        } else if (node->value < value) {
            if (!node->right) {
                node->right = new Node(value);
                node->right->parent = node;
                if (node->color == Color::BLACK)
                    return;
                if (!node->sibling()) {
                    if (node->isRight())
                        node = this->rotateLeft(node->parent);
                    else {
                        node = this->rotateLeft(node);
                        node = this->rotateRight(node->parent);
                    }
                    node->color = Color::BLACK;
                    node->right->color = node->left->color = Color::RED;
                    return;
                }
            }
        }

        if (node->hasColorChildren(Color::RED)) {
            node->color = Color::RED;
            node->right->color = node->left->color = Color::BLACK;

            if (node->parent && node->parent->color == RED) {
                if (node->isLeft() && node->parent->isLeft()) {
                    node = this->rotateRight(node->parent->parent);
                    node->color = BLACK;
                    node->left->color = node->right->color = RED;
                    this->insert(node->left, value);
                    return;
                }
                if (node->isRight() && node->parent->isRight()) {
                    node = this->rotateLeft(node->parent->parent);
                    node->color = BLACK;
                    node->left->color = node->right->color = RED;
                    this->insert(node->left, value);
                    return;
                }
                if (node->isRight() && node->parent->isLeft()) {
                    node = this->rotateLeft(node->parent);
                    node = this->rotateRight(node->parent);
                    node->color = BLACK;
                    node->left->color = node->right->color = RED;
                    this->insert(node->left, value);
                    return;
                }
                if (node->isLeft() && node->parent->isRight()) {
                    node = this->rotateRight(node->parent);
                    node = this->rotateLeft(node->parent);
                    node->color = BLACK;
                    node->left->color = node->right->color = RED;
                    this->insert(node->right, value);
                    return;
                }
            }
        }

        this->insert(node->value > value ? node->left : node->right, value);
    }

    /**
     * @brief Prints a subtree so an ostream
     *
     * @param root current subtree being flushed
     * @param os ostream to flush to
     * @param trace how much to indent each line
     */
    void print_tree(const Node* root, ostream& os, size_t trace) const {
        if (root->right)
            this->print_tree(root->right, os, trace + 1);
        os << string(trace * 2, ' ') << *root << "\n";
        if (root->left)
            this->print_tree(root->left, os, trace + 1);
    }

    bool followsRules(Node* root) const {
        bool rules = true;
        rules &= !this->doubleRed(root);
        rules &= this->blackHeight(root->left) == this->blackHeight(root->right);
        return rules;
    }

    bool doubleRed(Node* root) const {
        if (!root)
            return false;
        if (root->color == Color::RED && root->hasColorChild(Color::RED))
            return true;
        return this->doubleRed(root->left) || this->doubleRed(root->right);
    }

    size_t blackHeight(const Node* root) const {
        if (!root)
            return 1;
        size_t height = this->blackHeight(root->left);
        return height + (root->color == Color::BLACK ? 1 : 0);
    }
public:
    /**
     * @brief Construct a new Red Black Tree object
     */
    RedBlackTree() : _root{nullptr} {}

    /**
     * @brief Construct a new Red Black Tree object
     *
     * @param rhs RedBlackTree to copy from
     */
    RedBlackTree(const RedBlackTree& rhs) : _root{this->copy(rhs.get_root())} {}

    /**
     * @brief Destroy the Red Black Tree object
     */
    ~RedBlackTree() { this->make_empty(); }

    /**
     * @brief Copy assignment operator
     *
     * @param rhs Red Black Tree to copy from
     * @return RedBlackTree& *this
     */
    RedBlackTree& operator=(const RedBlackTree& rhs) {
        if (this != &rhs) {
            this->make_empty();
            this->_root = this->copy(rhs.get_root());
        }

        return *this;
    }

    /**
     * @brief Insert a value into the tree
     *
     * @param value Comparable to insert
     */
    void insert(const Comparable& value) {
        this->insert(this->_root, value);
        this->_root->color = Color::BLACK;
    }

    /**
     * @brief Remove a value from the tree
     *
     * @param value Comparable to remove
     */
    void remove(const Comparable& value) {
        if (!this->_root)
            return;

        if (this->_root->hasColorChildren(Color::BLACK))
            this->_root->color = Color::RED;

        this->remove(this->_root, value);
        if (this->_root)
            this->_root->color = Color::BLACK;
    }

    /**
     * @brief Determine if this tree contains a value
     *
     * @param value Comparable to search for
     * @return true if the tree contains the value
     * @return false otherwise
     */
    bool contains(const Comparable& value) const { return this->search(this->_root, value); }

    /**
     * @brief Determines the smallest value in the tree
     *
     * @return const Comparable& the smallest value in the tree
     */
    const Comparable& find_min() const {
        if (!this->_root)
            throw invalid_argument("Red Black Tree is empty");

        return this->find_min(this->_root)->value;
    }

    /**
     * @brief Determines the largest value in the tree
     *
     * @return const Comparable& the largest value in the tree
     */
    const Comparable& find_max() const {
        if (!this->_root)
            throw invalid_argument("Red Black Tree is empty");

        Node* node = this->_root;
        while (node->right)
            node = node->right;

        return node->value;
    }

    /**
     * @brief Determine the Color of a node
     *
     * @param node a pointer to a node to determine the color of
     * @return int Color of the node
     */
    int color(const Node* node) const { return node ? node->color : Color::BLACK; }

    /**
     * @brief Get the root object
     *
     * @return const Node* a pointer the the root of the tree
     */
    const Node* get_root() const { return this->_root; }

    bool followsRules() const {
        if (!this->_root)
            return true;
        return this->followsRules(this->_root);
    }

    // ----------------------- Optional ----------------------- //
    RedBlackTree(RedBlackTree&& rhs) : _root{nullptr} { swap(this->_root, rhs.root); }

    RedBlackTree& operator=(RedBlackTree&& rhs) {
        if (this != &rhs) {
            this->make_empty();
            swap(this->_root, rhs.root);
        }

        return *this;
    }

    void insert(Comparable&& value) {
        Comparable v{Comparable()};
        swap(v, value);
        this->insert(v);
    }

    /**
     * @brief Determine if the tree is empty
     *
     * @return true if the tree is empty
     * @return false otherwise
     */
    bool is_empty() const { return !this->_root; }

    /**
     * @brief Prevents memory leaks by deallocating the entire tree
     */
    void make_empty() { this->_root = this->clear(this->_root); }

    /**
     * @brief Flushes the entire tree to an ostream
     *
     * @param os ostream to flush to, cout by default
     */
    void print_tree(ostream& os = cout) const {
        if (this->_root)
            this->print_tree(this->_root, os, 0);
        else
            os << "<empty>";
    }
};