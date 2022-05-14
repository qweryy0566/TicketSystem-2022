#ifndef TICKETSYSTEM_VECTOR_HPP_
#define TICKETSYSTEM_VECTOR_HPP_

#include <climits>
#include <cmath>
#include <cstddef>

#include "exceptions.hpp"

namespace fqj {
template <typename T>
class vector {
  T *array;
  size_t cur_size = 0, limit;  // 当前元素个数与当前申请的空间大小。

  void DoubleSpace() {
    T *tmp = (T *)malloc((limit << 1) * sizeof(T));
    for (int i = 0; i < cur_size; ++i) {
      new (tmp + i) T(array[i]);
      array[i].~T();
    }
    free(array);
    limit <<= 1, array = tmp;
  }

 public:
  class const_iterator;
  class iterator {
    friend class vector;
    int at;
    vector *source;

   public:
    iterator() {}
    iterator(int i, vector *source) : at(i), source(source) {}

    iterator operator+(const int &n) const {
      iterator ret = *this;
      ret.at += n;
      return ret;
    }
    iterator operator-(const int &n) const {
      iterator ret = *this;
      ret.at -= n;
      return ret;
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw
    // invaild_iterator.
    int operator-(const iterator &rhs) const {
      if (source != rhs.source) throw invalid_iterator();
      return abs(rhs.at - at);
    }
    iterator &operator+=(const int &n) { return *this = *this + n; }
    iterator &operator-=(const int &n) { return *this = *this - n; }

    iterator operator++(int) {
      iterator ret = *this + 1;
      *this += 1;
      return ret;
    }
    iterator &operator++() { return *this += 1; }

    iterator operator--(int) {
      iterator ret = *this + 1;
      *this -= 1;
      return ret;
    }
    iterator &operator--() { return *this -= 1; }

    T &operator*() const { return source->array[at]; }
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory address).
     */
    bool operator==(const iterator &rhs) const {
      return source == rhs.source && at == rhs.at;
    }
    bool operator==(const const_iterator &rhs) const {
      return source == rhs.source && at == rhs.at;
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
  };
  /**
   * TODO
   * has same function as iterator, just for a const object.
   */
  class const_iterator {
    friend class vector;
    int at;
    const vector *source;

   public:
    const_iterator() {}
    const_iterator(int i, const vector *source) : at(i), source(source) {}

    const_iterator operator+(const int &n) const {
      const_iterator ret = *this;
      ret.at += n;
      return ret;
    }
    const_iterator operator-(const int &n) const {
      const_iterator ret = *this;
      ret.at -= n;
      return ret;
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw
    // invaild_iterator.
    int operator-(const const_iterator &rhs) const {
      if (source != rhs.source) throw invalid_iterator();
      return abs(rhs.at - at);
    }
    const_iterator &operator+=(const int &n) { return *this = *this + n; }
    const_iterator &operator-=(const int &n) { return *this = *this - n; }

    const_iterator operator++(int) {
      const_iterator ret = *this + 1;
      *this += 1;
      return ret;
    }
    const_iterator &operator++() { return *this += 1; }

    const_iterator operator--(int) {
      const_iterator ret = *this + 1;
      *this -= 1;
      return ret;
    }
    const_iterator &operator--() { return *this -= 1; }

    const T operator*() const { return source->array[at]; }
    /**
     * a operator to check whether two iterators are same (pointing to the same
     * memory address).
     */
    bool operator==(const iterator &rhs) const {
      return source == rhs.source && at == rhs.at;
    }
    bool operator==(const const_iterator &rhs) const {
      return source == rhs.source && at == rhs.at;
    }
    bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
  };

  vector(int cnt = 9) : limit(cnt) {
    array = (T *)malloc(limit * sizeof(T));  // 注意区别申请大小与实际大小。
  }
  vector(const vector &other) : cur_size(other.cur_size) {
    limit = cur_size + 9;
    array = (T *)malloc(limit * sizeof(T));
    for (int i = 0; i < cur_size; ++i) new (array + i) T(other[i]);
  }
  ~vector() {
    if (array) {
      for (int i = 0; i < cur_size; ++i) array[i].~T();
      free(array);  // 注意区别申请大小与实际大小。
    }
  }

  vector &operator=(const vector &other) {
    if (&other != this) {
      for (int i = 0; i < cur_size; ++i) array[i].~T();
      free(array);
      cur_size = other.cur_size, limit = cur_size + 9;
      array = (T *)malloc(limit * sizeof(T));
      for (int i = 0; i < cur_size; ++i) new (array + i) T(other[i]);
    }
    return *this;
  }

  T &at(const size_t &pos) {
    if (pos < 0 || pos >= cur_size) throw index_out_of_bound();
    return array[pos];
  }
  const T &at(const size_t &pos) const {
    if (pos < 0 || pos >= cur_size) throw index_out_of_bound();
    return array[pos];
  }

  T &operator[](const size_t &pos) { return at(pos); }
  const T &operator[](const size_t &pos) const { return at(pos); }

  const T &front() const {
    if (!cur_size) throw container_is_empty();
    return array[0];
  }

  const T &back() const {
    if (!cur_size) throw container_is_empty();
    return array[cur_size - 1];
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() { return iterator(0, this); }
  const_iterator cbegin() const { return const_iterator(0, this); }
  /**
   * returns an iterator to the end.
   */
  iterator end() { return iterator(cur_size, this); }
  const_iterator cend() const { return const_iterator(cur_size, this); }

  bool empty() const { return !cur_size; }

  size_t size() const { return cur_size; }
  /**
   * clears the contents
   */
  void clear() { cur_size = 0; }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
    if (cur_size + 1 == limit) DoubleSpace();
    for (int i = cur_size; i > pos.at; --i) array[i] = array[i - 1];
    array[pos.at] = value, ++cur_size;
    return pos;
  }
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size
   * because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
    if (ind > cur_size) throw index_out_of_bound();
    if (cur_size + 1 == limit) DoubleSpace();
    for (int i = cur_size; i > ind; --i) array[i] = array[i - 1];
    array[ind] = value, ++cur_size;
    return iterator(ind, this);
  }
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is
   * returned.
   */
  iterator erase(iterator pos) {
    array[pos.at].~T();
    for (int i = pos.at; i < cur_size; ++i) array[i] = array[i + 1];
    cur_size -= pos.at < cur_size;
    return pos;
  }
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
    if (ind >= cur_size) throw index_out_of_bound();
    array[ind].~T();
    for (int i = ind; i < cur_size; ++i) array[i] = array[i + 1];
    --cur_size;
    return iterator(ind, this);
  }
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
    if (cur_size + 1 == limit) DoubleSpace();
    new (array + cur_size) T(value), ++cur_size;
  }
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if (!cur_size) throw container_is_empty();
    array[--cur_size].~T();
  }
};

}  // namespace fqj

#endif  // TICKETSYSTEM_VECTOR_HPP_