#ifndef TICKETSYSTEM_UNORDERED_MAP_HPP_
#define TICKETSYSTEM_UNORDERED_MAP_HPP_

// only for std::equal_to<T> and std::hash<T>
#include <cstddef>
#include <functional>

#include "exceptions.hpp"
using std::pair;

namespace fqj {
/**
 * In linked_hashmap, iteration ordering is differ from map,
 * which is the order in which keys were inserted into the map.
 * You should maintain a doubly-linked list running through all
 * of its entries to keep the correct iteration order.
 *
 * Note that insertion order is not affected if a key is re-inserted
 * into the map.
 */
template <class Key, class T, class Hash = std::hash<Key>,
          class Equal = std::equal_to<Key> >
class unordered_map {
 public:
  using value_type = pair<const Key, T>;

 private:
  static const size_t LEN = 13;
  struct Node {
    value_type *val;
    Node *time_lst{nullptr}, *time_nxt{nullptr}, *nxt{nullptr};

    Node() : val{nullptr} {}
    Node(const value_type &val, Node *lst, Node *nxt, Node *p)
        : val{new value_type{val}}, time_lst{lst}, time_nxt{nxt}, nxt{p} {}
    ~Node() {
      if (val) delete val;
    }
  };
  size_t len, load;
  Node *hd, *tl, **buc;
  Hash hash;
  Equal equal;

  void ChangeSpace(const int &d) {
    len = d > 0 ? len << d | 1 : len >> -d;
    delete[] buc, buc = new Node *[len]{nullptr};
    size_t index;
    for (Node *it = hd->time_nxt; it != tl; it = it->time_nxt) {
      index = hash(it->val->first) % len;
      it->nxt = buc[index], buc[index] = it;
    }
  }

  Node *Add(const value_type &key) {
    if (load++ > len) ChangeSpace(1);
    size_t index = hash(key.first) % len;
    Node *p{new Node{key, tl->time_lst, tl, buc[index]}};
    tl->time_lst = tl->time_lst->time_nxt = p;
    return buc[index] = p;
  }
  // 在时间链表中删除该点，减负载，并析构。
  void Del(Node *p) {
    p->time_lst->time_nxt = p->time_nxt;
    p->time_nxt->time_lst = p->time_lst;
    --load, delete p;
    if (len > LEN && load << 2 < len) ChangeSpace(-1);
  }
  void ClearNode() {
    len = LEN;
    Node *it = hd->time_nxt, *tmp;
    while (it != tl) tmp = it->time_nxt, Del(it), it = tmp;
  }
  Node *Find(const Key &x) const {
    size_t index = hash(x) % len;
    for (Node *it = buc[index]; it; it = it->nxt)
      if (equal(x, it->val->first)) return it;
    return tl;  // 返回 end()
  }

 public:
  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = linked_hashmap.begin(); --it;
   *       or it = linked_hashmap.end(); ++end();
   */
  class const_iterator;
  class iterator {
    friend class unordered_map;
    unordered_map *source{nullptr};
    Node *at{nullptr};

   public:
    iterator() = default;
    iterator(const iterator &other) : source{other.source}, at{other.at} {}
    iterator(unordered_map *source, Node *at) : source{source}, at{at} {}

    iterator operator++(int) {
      iterator tmp = *this;
      operator++();
      return tmp;
    }
    iterator &operator++() {
      if (at == source->tl) throw invalid_iterator{};
      at = at->time_nxt;
      return *this;
    }

    iterator operator--(int) {
      iterator tmp = *this;
      operator--();
      return tmp;
    }
    iterator &operator--() {
      if (at->time_lst == source->hd) throw invalid_iterator{};
      at = at->time_lst;
      return *this;
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const { return at == rhs.at; }
    bool operator==(const const_iterator &rhs) const { return at == rhs.at; }

    bool operator!=(const iterator &rhs) const { return at != rhs.at; }
    bool operator!=(const const_iterator &rhs) const { return at != rhs.at; }
    /**
     * some other operator for iterator.
     */
    value_type &operator*() const { return *at->val; }
    /**
     * for the support of it->first.
     * See
     * <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/>
     * for help.
     */
    value_type *operator->() const noexcept { return at->val; }
  };

  class const_iterator {
    // it should has similar member method as iterator.
    //  and it should be able to construct from an iterator.
    friend class unordered_map;
    const unordered_map *source{nullptr};
    const Node *at{nullptr};

   public:
    const_iterator() = default;
    const_iterator(const const_iterator &other)
        : source{other.source}, at{other.at} {}
    const_iterator(const iterator &other)
        : source{other.source}, at{other.at} {}
    const_iterator(const unordered_map *source, const Node *at)
        : source{source}, at{at} {}
    // And other methods in iterator.
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      operator++();
      return tmp;
    }
    const_iterator &operator++() {
      if (at == source->tl) throw invalid_iterator{};  // end() + 1
      at = at->time_nxt;
      return *this;
    }

    const_iterator operator--(int) {
      const_iterator tmp = *this;
      operator--();
      return tmp;
    }
    const_iterator &operator--() {
      if (at->time_lst == source->hd) throw invalid_iterator{};
      at = at->time_lst;
      return *this;
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const { return at == rhs.at; }
    bool operator==(const const_iterator &rhs) const { return at == rhs.at; }

    bool operator!=(const iterator &rhs) const { return at != rhs.at; }
    bool operator!=(const const_iterator &rhs) const { return at != rhs.at; }
    /**
     * some other operator for iterator.
     */
    const value_type &operator*() const { return *at->val; }
    const value_type *operator->() const noexcept { return at->val; }
  };

  unordered_map() : len{LEN}, load{0}, hd{new Node{}}, tl{new Node{}} {
    buc = new Node *[len]{nullptr};
    hd->time_nxt = tl, tl->time_lst = hd;
  }
  unordered_map(const unordered_map &obj)
      : len{obj.len}, load{0}, hd{new Node{}}, tl{new Node{}} {
    buc = new Node *[len]{nullptr};
    hd->time_nxt = tl, tl->time_lst = hd;
    for (Node *it = obj.hd->time_nxt; it != obj.tl; it = it->time_nxt)
      Add(*it->val);
  }

  unordered_map &operator=(const unordered_map &obj) {
    if (this != &obj) {
      ClearNode();
      delete[] buc, buc = new Node *[len = obj.len]{nullptr};
      for (Node *it = obj.hd->time_nxt; it != obj.tl; it = it->time_nxt)
        Add(*it->val);
    }
    return *this;
  }

  ~unordered_map() {
    ClearNode();
    delete hd, delete tl, delete[] buc;
  }

  /**
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent
   * to key. If no such element exists, an exception of type
   * `index_out_of_bound'
   */
  T &at(const Key &key) {
    Node *tmp = Find(key);
    if (tmp == tl) throw index_out_of_bound{};
    return tmp->val->second;
  }
  const T &at(const Key &key) const {
    const Node *tmp = Find(key);
    if (tmp == tl) throw index_out_of_bound{};
    return tmp->val->second;
  }

  T &operator[](const Key &key) {
    Node *tmp = Find(key);
    if (tmp == tl) return Add({key, T{}})->val->second;
    return tmp->val->second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const { return at(key); }

  /**
   * return a iterator to the beginning
   */
  iterator begin() { return {this, hd->time_nxt}; }
  const_iterator cbegin() const { return {this, hd->time_nxt}; }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() { return {this, tl}; }
  const_iterator cend() const { return {this, tl}; }

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const { return !load; }

  /**
   * returns the number of elements.
   */
  size_t size() const { return load; }

  /**
   * clears the contents
   */
  void clear() {
    ClearNode();
    delete[] buc, buc = new Node *[len]{nullptr};
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the
   * insertion), the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    Node *at = Find(value.first);
    if (at != tl) return {{this, at}, 0};
    return {{this, Add(value)}, 1};
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an
   * element out of this)
   */
  void erase(iterator pos) {
    if (pos.at == tl || pos.source != this) throw invalid_iterator{};
    size_t index = hash(pos.at->val->first) % len;
    if (buc[index] == pos.at) buc[index] = pos.at->nxt;
    else {
      Node *it = buc[index];
      while (it->nxt != pos.at) it = it->nxt;
      it->nxt = it->nxt->nxt;
    }
    Del(pos.at);
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   */
  size_t count(const Key &key) const { return Find(key) != tl; }
  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is
   * returned.
   */
  iterator find(const Key &key) { return {this, Find(key)}; }
  const_iterator find(const Key &key) const { return {this, Find(key)}; }
};

}  // namespace fqj

#endif  // TICKETSYSTEM_UNORDERED_MAP_HPP_