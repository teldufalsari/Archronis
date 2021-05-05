#ifndef CORE_AVL_YGGDRASIL_H
#define CORE_AVL_YGGDRASIL_H

/** @file core/avl_yggdrasil.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly.
 */


namespace tld
{
/**
 * @brief An AVL tree class, designed for use in implementing template data containers.
 * @details AVL trees are balanced BS trees that allow to obtain a key in logarithmic time.
 * In this tree right child has the greater key and left child has the less key than root.
 * Warning: This tree implementation may be unsafe to use standalone.
 * @tparam T value type to be stored in the tree.
 */
template <typename T>
class avl_tree
{
public:
    ///Tree node type offering value initialization and destruction.
    struct tree_node
    {
        /// Value stored in the node.
        T _n_value;

        /// Height of the subtree with a root in this node.
        int height;

        /// Pointer to the left child node.
        tree_node* left;

        /// Pointer to the parent node.
        tree_node* parent;

        /// Pointer to the right child node.
        tree_node* right;

        /**
         * @brief Creates an empty node of height 1.
         */
        tree_node();

        /**
         * @brief Copy constructor.
         * @param that Reference to the source object.
         */
        tree_node(const tree_node& that);

        /**
         * @brief Move constructor.
         * @param that Reference to the source object.
         */
        tree_node(tree_node&& that) noexcept;

        /**
         * @brief Trivial destructor.
         */
        ~tree_node();
    };

public:
    /// Pointer to the root of the tree.
    tree_node* root_;

private:
    void in_order_traverse(void (avl_tree<T>::*p_function)(tree_node*), tree_node* p_node);
    void pre_order_traverse(void (avl_tree<T>::*p_function)(tree_node*), tree_node* p_node);
    void post_order_traverse(void (avl_tree<T>::*p_function)(tree_node*), tree_node* p_node);
    tree_node* insert(tree_node** node, const T& new_value);
    void node_destroy(tree_node* p_node);
    tree_node* find(tree_node* p_node, const T& key);
    void find_and_remove(tree_node* p_node, const T& key);
    void remove(tree_node* p_node);
    tree_node* max(tree_node* p_node);
    void copy(tree_node** p_dst, const tree_node& src);
    void rotate_left(tree_node* old_root);
    void rotate_right(tree_node* old_root);
    void balance(tree_node* p_tree);
    inline int height(tree_node* p_node);
    int diff(tree_node* p_node);
    void set_height(tree_node* p_node);

public:
    /**
     * @brief Creates an empty tree with null root pointer.
     */
    avl_tree();

    /**
     * @brief Copy constructor for a tree.
     * @param that Reference to the source object.
     */
    avl_tree(const avl_tree& that);

    /**
     * @brief Move constructor for a tree.
     * @param that Reference to the source object.
     */
    avl_tree(avl_tree&& that) noexcept;

    /**
     * @brief Recursively frees allocated memory.
     */
    ~avl_tree();

    /**
     * @brief Copy assignment operator for a tree.
     * @param that Reference to the source object.
     * @return Reference to the tree object to create assignment chains.
     */
    avl_tree& operator =(const avl_tree& that);

    /**
     * @brief Move assignment operator for a tree.
     * @param that Reference to the source object.
     * @return Reference to the tree object to create assignment chains.
     */
    avl_tree& operator =(avl_tree&& that) noexcept;

    /**
     * @brief Inserts a new element into the container.
     * @param [in] new_value An element to insert.
     * @return Pointer to the node containing the element inserted.
     */
    tree_node* insert(const T& new_value);

    /**
     * @brief Clear the tree: recursively frees allocated memory and sets root pointer null.
     */
    void destroy();

    /**
     * @brief Retrieves a value with specified key.
     * @param key Key of the element.
     * @return Pointer to the node containing the element is stored if found, otherwise nullptr.
     */
    tree_node* find(const T& key);

    /**
     * @brief Removes the element with specified key from the container.
     * @param key Key of the element.
     */
    void find_and_remove(const T& key);
};

/**
 * @brief Comparision operator for map type.
 * @param lht Left operand.
 * @param rht Right operand.
 * @return The result of comparision of the root pointers.
 *
 * @details It is used to avoid self-assignment.
 */
template <typename T>
inline bool operator ==(const avl_tree<T>& lht, const avl_tree<T>& rht);

// ### TREE_NODE ###

template <typename T>
avl_tree<T>::tree_node::tree_node() :
    _n_value(),
    height(1),
    left(nullptr),
    parent(nullptr),
    right(nullptr)
{}

template <typename T>
avl_tree<T>::tree_node::tree_node(const avl_tree::tree_node& that) :
    _n_value(that._n_value),
    height(that.height),
    left(nullptr),
    parent(nullptr),
    right(nullptr)
{}

template <typename T>
avl_tree<T>::tree_node::tree_node(tree_node&& that) noexcept :
    _n_value(std::move(that._n_value)),
    height(that.height),
    left(nullptr),
    parent(nullptr),
    right(nullptr)
{}

template <typename T>
avl_tree<T>::tree_node::~tree_node() = default;


// ### AVL_TREE ###

template <typename T>
avl_tree<T>::avl_tree() :
    root_(nullptr)
{}

template <typename T>
avl_tree<T>::avl_tree(const avl_tree& that) :
    root_(nullptr)
{
    if (that.root_)
        copy(&(this->root_), *(that.root_));
}

template <typename T>
avl_tree<T>::avl_tree(avl_tree&& that) noexcept :
    root_(that.root_)
{
    that.root_ = nullptr;
}

template <typename T>
avl_tree<T>::~avl_tree()
{
    if (root_)
        this->destroy();
}

template <typename T>
void avl_tree<T>::in_order_traverse(void (avl_tree<T>::*p_function)(tree_node*), tree_node* p_node)
{
    if (p_node == nullptr)
        return;
    if (p_node->left != nullptr)
        in_order_traverse(p_function, p_node->left);
    (this->*p_function)(p_node);
    if (p_node->right != nullptr)
        in_order_traverse(p_function, p_node->right);
}

template <typename T>
void avl_tree<T>::pre_order_traverse(void (avl_tree<T>::*p_function)(tree_node*), tree_node* p_node)
{
    if (p_node == nullptr)
        return;
    (this->*p_function)(p_node);
    if (p_node->left != nullptr)
        pre_order_traverse(p_function, p_node->left);
    if (p_node->right != nullptr)
        pre_order_traverse(p_function, p_node->right);
}

template <typename T>
void avl_tree<T>::post_order_traverse(void (avl_tree<T>::*p_function)(tree_node*), tree_node* p_node)
{
    if (p_node == nullptr)
        return;
    if (p_node->left != nullptr)
        post_order_traverse(p_function, p_node->left);
    if (p_node->right != nullptr)
        post_order_traverse(p_function, p_node->right);
    (this->*p_function)(p_node);
}

template <typename T>
void avl_tree<T>::destroy()
{
    post_order_traverse(&avl_tree::node_destroy, root_);
    root_ = nullptr;
}

template <typename T>
void avl_tree<T>::node_destroy(avl_tree::tree_node* p_node)
{
    delete p_node;
}

template <typename T>
void avl_tree<T>::rotate_left(avl_tree::tree_node* old_root)
{
    tree_node* new_root = old_root->right;
    old_root->right = new_root->left;
    if (old_root->right)
        old_root->right->parent = old_root;
    if (old_root->parent) {
        if (old_root->parent->right == old_root)
            old_root->parent->right = new_root;
        else
            old_root->parent->left = new_root;
    } else
        this->root_ = new_root;
    
    new_root->parent = old_root->parent;
    new_root->left = old_root;
    old_root->parent = new_root;
    set_height(old_root);
    set_height(new_root);
}

template <typename T>
void avl_tree<T>::rotate_right(avl_tree::tree_node* old_root)
{
    tree_node* new_root = old_root->left;
    old_root->left = new_root->right;
    if (old_root->left)
        old_root->left->parent = old_root;
    if (old_root->parent) {
        if (old_root->parent->right == old_root)
            old_root->parent->right = new_root;
        else
            old_root->parent->left = new_root;
    } else
        this->root_ = new_root;
    
    new_root->parent = old_root->parent;
    new_root->right = old_root;
    old_root->parent = new_root;
    set_height(old_root);
    set_height(new_root);
}

template <typename T>
void avl_tree<T>::balance(avl_tree::tree_node* p_tree)
{
    set_height(p_tree);
    if (diff(p_tree) == 2) {
        if (diff(p_tree->right) < 0)
            rotate_right(p_tree->right);
        rotate_left(p_tree);
    }
    
    if (diff(p_tree) == -2) {
        if (diff(p_tree->left) > 0)
            rotate_left(p_tree->left);
        rotate_right(p_tree);
    }
}

template <typename T>
int avl_tree<T>::height(avl_tree::tree_node* p_node)
{
    return (p_node ? p_node->height : 0);
}

template <typename T>
int avl_tree<T>::diff(avl_tree::tree_node* p_node)
{
    return height(p_node->right) - height(p_node->left);
}

template <typename T>
void avl_tree<T>::set_height(avl_tree::tree_node* p_node)
{
    int hl = height(p_node->left);
    int hr = height(p_node->right);
    p_node->height = (hl > hr ? hl : hr) + 1;
}

template <typename T>
typename avl_tree<T>::tree_node* avl_tree<T>::insert(avl_tree::tree_node** node, const T& new_value)
{
    if (!(*node)) {
        *node = new tree_node;
        (*node)->_n_value = new_value;
        return *node;
    }
    if (new_value == (*node)->_n_value) {
        return *node;
    }
    if (new_value < (*node)->_n_value) {
        tree_node* new_node = insert(&((*node)->left), new_value);
        if ((*node)->left->parent == 0) {
            (*node)->left->parent = *node;
        }
        balance(*node);
        return new_node;
    }
    tree_node* new_node = insert(&((*node)->right), new_value);
    if ((*node)->right->parent == 0)
        (*node)->right->parent = *node;
    balance(*node);
    return new_node;
}

template <typename T>
typename avl_tree<T>::tree_node* avl_tree<T>::insert(const T& new_value)
{
    return insert(&root_, new_value);
}

template <typename T>
void avl_tree<T>::find_and_remove(avl_tree::tree_node* p_node, const T& key)
{
    if (!p_node)
        return;
    if (key > p_node->_n_value) {
        find_and_remove(p_node->right, key);
        balance(p_node);
    } else if (key < p_node->_n_value) {
        find_and_remove(p_node->left, key);
        balance(p_node);
    } else {
        remove(p_node);
    }
}

template <typename T>
void avl_tree<T>::remove(avl_tree::tree_node* p_node)
{
    if (p_node == nullptr)
        return;
    if ((p_node->right == nullptr) && (p_node->left == nullptr)) {
        if (p_node->parent->right == p_node)
            p_node->parent->right = nullptr;
        else
            p_node->parent->left = nullptr;
        delete p_node;
    } else if (p_node->right == nullptr) {
        if (p_node->parent->right == p_node)
            p_node->parent->right = p_node->left;
        else
            p_node->parent->left = p_node->left;
        p_node->left->parent = p_node->parent;
        delete p_node;
    } else if (p_node->left == nullptr) {
        if (p_node->parent->right == p_node)
            p_node->parent->right = p_node->right;
        else
            p_node->parent->left = p_node->right;
        p_node->right->parent = p_node->parent;
        delete p_node;
    } else {
        tree_node* new_root = max(p_node->left);
        new_root->left->parent = new_root->parent;
        new_root->parent->right = new_root->left;
        if (p_node->parent->right == p_node)
            p_node->parent->right = new_root;
        else
            p_node->parent->left = new_root;
        new_root->right = p_node->right;
        new_root->left = p_node->left;
        delete p_node;
    }
}

template <typename T>
typename avl_tree<T>::tree_node* avl_tree<T>::find(avl_tree::tree_node* p_node, const T& key)
{
    if (key == p_node->_n_value) {
        return p_node;
    } else {
        if ((key < p_node->_n_value) && (p_node->left != nullptr))
            return find(p_node->left, key);
        else if (p_node->right != nullptr)
            return find(p_node->right, key);
    }
    return nullptr;
}

template <typename T>
typename avl_tree<T>::tree_node* avl_tree<T>::find(const T& key)
{
    return find(root_, key);
}

template <typename T>
void avl_tree<T>::find_and_remove(const T& key)
{
    find_and_remove(root_, key);
}


template <typename T>
typename avl_tree<T>::tree_node* avl_tree<T>::max(avl_tree::tree_node* p_node)
{
    if (p_node == nullptr)
        return p_node;
    while (p_node->right != nullptr)
        p_node = p_node->right;
    return p_node;
}

template <typename T>
void avl_tree<T>::copy(avl_tree::tree_node** p_dst, const avl_tree::tree_node& src)
{
    if (*p_dst)
        return;
    (*p_dst) = new tree_node;
    tree_node* dst = *p_dst;
    dst->_n_value = src._n_value;
    if (src.left) {
        copy(&(dst->left), *(src.left));
        dst->left->parent = dst;
    }
    if (src.right) {
        copy(&(dst->right), *(src.right));
        dst->right->parent = dst;
    }
}

template <typename T>
avl_tree<T>& avl_tree<T>::operator =(const avl_tree& that)
{
    if (this == &that)
        return *this;
    this->destroy();
    if (that.root_)
        copy(&(this->root_), *(that.root_));
    return *this;
}

template <typename T>
avl_tree<T>& avl_tree<T>::operator =(avl_tree&& that) noexcept
{
    this->root_ = that.root_;
    that.root_ = nullptr;
    return *this;
}

template <typename T>
inline bool operator ==(const avl_tree<T>& lht, const avl_tree<T>& rht)
{
    return (lht.root_ == rht.root_);
}

} //namespace tld
#endif //CORE_AVL_YGGDRASIL_H
