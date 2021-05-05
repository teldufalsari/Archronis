#ifndef TLD_MAP_H
#define TLD_MAP_H
///@file map.h
#include "core/avl_yggdrasil.h"
#include <utility>

namespace tld
{
/**
 * @brief A key-value pair used by map.
 * @tparam KeyT Key type used in comparision and search.
 * @tparam ValT Value type stored in pairs.
 */
template <typename KeyT, typename ValT>
struct pair_t {
    KeyT _p_key;
    ValT _p_value;

    /**
     * @brief Default constructor. Calls default constructors of key type and value type.
     */
    pair_t();

    /**
     * @brief Crates a pair with default value and a key provided.
     * @param nkey New key.
     */
    explicit pair_t(const KeyT& nkey);

    /**
     * @brief Creates a pair with a key and a value provided.
     * @param nkey New key.
     * @param nvalue New value.
     */
    pair_t(const KeyT& nkey, const ValT& nvalue);

    ~pair_t();

    /**
     * @brief Copy constructor for pair class.
     * @param that Reference to the source object.
     */
    pair_t(const pair_t&);

    /**
     * @brief Move constructor for pair class.
     * @param that Reference to the source object.
     */
    pair_t(pair_t&& that) noexcept;

    /**
     * @brief Copy assignment operator
     * @param that Reference to the source object.
     */
    pair_t& operator =(const pair_t& that);

    /**
     * @brief Move assignment operator
     * @param that Reference to the source object.
     */
    pair_t& operator =(pair_t&& that) noexcept;
};

/**
 * @brief Comparision operator for pairs.
 * @param lht Left operand.
 * @param rht Right operand.
 * @return The result of comparision between keys of the operands.
 */
template <typename KeyT, typename ValT>
inline bool operator <(const pair_t<KeyT, ValT>& lht, const pair_t<KeyT, ValT>& rht);

/**
 * @brief Comparision operator for pairs.
 * @param lht Left operand.
 * @param rht Right operand.
 * @return The result of comparision between keys of the operands.
 */
template <typename KeyT, typename ValT>
inline bool operator ==(const pair_t<KeyT, ValT>& lht, const pair_t<KeyT, ValT>& rht);


/**
 * @brief A data associative container type made made up of key-value pairs and providing data obtaining in logarithmic time.
 * @tparam KeyT Key type (argument) of a pair.
 * @tparam ValT	Data type (value) of a pair.
 * @details This container is based on an AVL binary search tree algorithm. Key type in the pair must have overloaded comparision operators.
 */
template <typename KeyT, typename ValT>
class map
{
public:
    typedef typename tld::avl_tree<pair_t<KeyT, ValT> >::tree_node node_t;

private:
    tld::avl_tree<pair_t<KeyT, ValT> > m_tree_;
    node_t* find(const KeyT& key, node_t* p_node);

public:
    /**
     * @brief Creates an empty map with an empty tree.
     */
    map();

    /**
     * @brief Copy constructor.
     * @param that Reference to the source object.
     * @details Recursively copies pairs stored in the source object to
     * the destination object.
     */
    map(const map& that);

    /**
     * @brief Move constructor
     * @param that Reference to the source object.
     */
    map(map&& that) noexcept;
    ~map();

    /**
     * @brief Inserts pair of suitable type into a map.
     * @param _x Reference to the pair object.
     */
    void insert(const pair_t<KeyT, ValT>& _x);

    /**
     * @brief Removes the pair with provided key from the map.
     * @param key
     */
    void erase(const KeyT& key);

    /**
     * @brief Retrieves a value by a key.
     *
     * @param key
     * @return Reference to the value component of the pair with provided key.
     *
     * @details If there is no pair with such key in the map, a new pair with
     * such key and default value is created. Reference to this pair's value
     * is returned.
     */
    ValT& find(const KeyT& key);

    /**
     * @brief Empties map object by recursively freeing allocated memory.
     */
    void clear();

    /**
     * @brief Says whether map object is empty or not.
     * @return True if empty, false if not.
     */
    bool empty();

    /**
     * @brief Searches the map for elements with a specified key and returns the number of matches
     * @param key key to search for
     * @return For a map the value either 0 or 1
     */
    unsigned count(const KeyT& key);

    /**
     * @brief An access operator. Retrieves a value by a key.
     * @param key
     * @return Reference to the value component of the pair with provided key.
     * @details If there is no pair with such key in the map, a new pair with
     * such key and default value is created. Reference to this pair's value
     * is returned.
     */
    ValT& operator [](const KeyT& key);

    /**
     * @brief Assignment-by-copy operator.
     * @param that Reference to the source object.
     * @return Reference to the map object to create assignment chains.
     * @details Recursively copies pairs stored in the source object to
     * the destination object.
     */
    map& operator =(const map& that);

    /**
     * @brief Move assignment operator
     * @param that Reference to the source object.
     */
    map& operator =(map&& that) noexcept;

    /**
     * @brief Comparision operator for map type.
     * @param lht Left operand.
     * @param rht Right operand.
     * @return The result of comparision of the trees.
     * @details It is used to avoid self-assignment.
     */
    friend inline bool operator ==(const map<KeyT, ValT>& lht, const map<KeyT, ValT>& rht)
    {
        return (lht.m_tree_ == rht.m_tree_);
    }
};


// ### PAIR ###

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>::pair_t() :
    _p_key(),
    _p_value()
{}

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>::pair_t(const KeyT& nkey) :
    _p_key(nkey),
    _p_value()
{}

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>::pair_t(const KeyT& nkey, const ValT& nvalue) :
    _p_key(nkey),
    _p_value(nvalue)
{}

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>::pair_t(const pair_t& that) :
    _p_key(that._p_key),
    _p_value(that._p_value)
{}

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>::pair_t(pair_t&& that) noexcept :
    _p_key(std::move(that._p_key)),
    _p_value(std::move(that._p_value))
{}

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>& pair_t<KeyT, ValT>::operator =(const pair_t& that)
{
    if (this != &that) {
        this->_p_key = that._p_key;
        this->_p_value = that._p_value;
    }
    return *this;
}

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>& pair_t<KeyT, ValT>::operator =(pair_t&& that) noexcept
{
    this->_p_key = std::move(that._p_key);
    this->_p_value = std::move(that._p_value);
    return *this;
}

template <typename KeyT, typename ValT>
pair_t<KeyT, ValT>::~pair_t() = default;


// ### MAP ###

template <typename KeyT, typename ValT>
map<KeyT, ValT>::map() = default;

template <typename KeyT, typename ValT>
map<KeyT, ValT>::~map() = default;

template <typename KeyT, typename ValT>
ValT& map<KeyT, ValT>::find(const KeyT& key)
{
    node_t* res_node = find(key, m_tree_.root_);
    if (res_node)
        return res_node->_n_value._p_value;
    pair_t<KeyT, ValT> n_node(key);
    return (m_tree_.insert(n_node))->_n_value._p_value;
}

template <typename KeyT, typename ValT>
typename map<KeyT, ValT>::node_t* map<KeyT, ValT>::find(const KeyT& key, map::node_t* p_node)
{
    if (p_node == nullptr)
        return nullptr;
    if (key == p_node->_n_value._p_key)
        return p_node;
    if (key < p_node->_n_value._p_key)
        return find(key, p_node->left);
    return find(key, p_node->right);
}

template <typename KeyT, typename ValT>
ValT& map<KeyT, ValT>::operator [](const KeyT& key)
{
    node_t* res_node = find(key, m_tree_.root_);
    if (res_node)
        return res_node->_n_value._p_value;
    pair_t<KeyT, ValT> n_node(key);
    return (m_tree_.insert(n_node))->_n_value._p_value;
}

template <typename KeyT, typename ValT>
map<KeyT, ValT>& map<KeyT, ValT>::operator =(const map& that)
{
    if (this != &that)
        this->m_tree_ = that.m_tree_;
    return *this;
}

template <typename KeyT, typename ValT>
map<KeyT, ValT>& map<KeyT, ValT>::operator =(map&& that) noexcept
{
    this->m_tree_ = std::move(that.m_tree_);
    return *this;
}

template <typename KeyT, typename ValT>
map<KeyT, ValT>::map(const map& that) :
    m_tree_(that.m_tree_)
{}

template <typename KeyT, typename ValT>
map<KeyT, ValT>::map(map&& that) noexcept :
    m_tree_(std::move(that.m_tree_))
{}

template <typename KeyT, typename ValT>
void map<KeyT, ValT>::erase(const KeyT& key)
{
    m_tree_.find_and_remove(key);
}

template <typename KeyT, typename ValT>
void map<KeyT, ValT>::clear()
{
    m_tree_.destroy();
}

template <typename KeyT, typename ValT>
bool map<KeyT, ValT>::empty()
{
    return (m_tree_.root_ == nullptr);
}

template <typename KeyT, typename ValT>
unsigned map<KeyT, ValT>::count(const KeyT& key)
{
    if (find(key, m_tree_.root_) != nullptr)
        return 1;
    else
        return 0;
}

template <typename KeyT, typename ValT>
void map<KeyT, ValT>::insert(const pair_t<KeyT, ValT>& _x)
{
    m_tree_.insert(_x);
}

template <typename KeyT, typename ValT>
inline bool operator <(const pair_t<KeyT, ValT>& lht, const pair_t<KeyT, ValT>& rht)
{
    return (lht._p_key < rht._p_key);
}

template <typename KeyT, typename ValT>
inline bool operator ==(const pair_t<KeyT, ValT>& lht, const pair_t<KeyT, ValT>& rht)
{
    return (lht._p_key == rht._p_key);
}

}// namespace tld

#endif //TLD_MAP_H
