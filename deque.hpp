#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <deque>
#include <cstddef>
#include <algorithm>

namespace sjtu {

template <class T> class deque {
  std::deque<T> d;
public:
  class const_iterator;
  class iterator {
    friend class deque<T>;
    deque<T>* owner = nullptr;
    size_t pos = 0;
  public:
    iterator() = default;
    iterator(deque<T>* o, size_t p): owner(o), pos(p) {}

    iterator operator+(const int &n) const { return iterator(owner, pos + n); }
    iterator operator-(const int &n) const { return iterator(owner, pos - n); }

    int operator-(const iterator &rhs) const {
      if (owner != rhs.owner) throw invalid_iterator();
      return static_cast<int>(pos) - static_cast<int>(rhs.pos);
    }
    iterator &operator+=(const int &n) { pos += n; return *this; }
    iterator &operator-=(const int &n) { pos -= n; return *this; }

    iterator operator++(int) { iterator t=*this; ++pos; return t; }
    iterator &operator++() { ++pos; return *this; }
    iterator operator--(int) { iterator t=*this; --pos; return t; }
    iterator &operator--() { --pos; return *this; }

    T &operator*() const { return owner->d[pos]; }
    T *operator->() const noexcept { return &owner->d[pos]; }

    bool operator==(const iterator &rhs) const { return owner==rhs.owner && pos==rhs.pos; }
    bool operator!=(const iterator &rhs) const { return !(*this==rhs); }
  };

  class const_iterator {
    friend class deque<T>;
    const deque<T>* owner = nullptr;
    size_t pos = 0;
  public:
    const_iterator() = default;
    const_iterator(const deque<T>* o, size_t p): owner(o), pos(p) {}
    const_iterator(const iterator &it): owner(it.owner), pos(it.pos) {}

    const_iterator operator+(const int &n) const { return const_iterator(owner, pos + n); }
    const_iterator operator-(const int &n) const { return const_iterator(owner, pos - n); }

    int operator-(const const_iterator &rhs) const {
      if (owner != rhs.owner) throw invalid_iterator();
      return static_cast<int>(pos) - static_cast<int>(rhs.pos);
    }
    const_iterator &operator+=(const int &n) { pos += n; return *this; }
    const_iterator &operator-=(const int &n) { pos -= n; return *this; }

    const_iterator operator++(int) { const_iterator t=*this; ++pos; return t; }
    const_iterator &operator++() { ++pos; return *this; }
    const_iterator operator--(int) { const_iterator t=*this; --pos; return t; }
    const_iterator &operator--() { --pos; return *this; }

    const T &operator*() const { return owner->d[pos]; }
    const T *operator->() const noexcept { return &owner->d[pos]; }

    bool operator==(const const_iterator &rhs) const { return owner==rhs.owner && pos==rhs.pos; }
    bool operator!=(const const_iterator &rhs) const { return !(*this==rhs); }
  };

  deque() {}
  deque(const deque &other) : d(other.d) {}
  ~deque() {}
  deque &operator=(const deque &other) { d = other.d; return *this; }

  T &at(const size_t &pos) { if (pos>=d.size()) throw index_out_of_bound(); return d.at(pos); }
  const T &at(const size_t &pos) const { if (pos>=d.size()) throw index_out_of_bound(); return d.at(pos); }
  T &operator[](const size_t &pos) { if (pos>=d.size()) throw index_out_of_bound(); return d[pos]; }
  const T &operator[](const size_t &pos) const { if (pos>=d.size()) throw index_out_of_bound(); return d[pos]; }

  const T &front() const { if (d.empty()) throw container_is_empty(); return d.front(); }
  const T &back() const { if (d.empty()) throw container_is_empty(); return d.back(); }

  iterator begin() { return iterator(this,0); }
  const_iterator cbegin() const { return const_iterator(this,0); }

  iterator end() { return iterator(this,d.size()); }
  const_iterator cend() const { return const_iterator(this,d.size()); }

  bool empty() const { return d.empty(); }
  size_t size() const { return d.size(); }

  void clear() { d.clear(); }

  iterator insert(iterator pos, const T &value) {
    if (pos.owner != this) throw invalid_iterator();
    d.insert(d.begin() + static_cast<long>(pos.pos), value);
    return iterator(this, pos.pos);
  }

  iterator erase(iterator pos) {
    if (d.empty()) throw container_is_empty();
    if (pos.owner != this) throw invalid_iterator();
    if (pos.pos>=d.size()) throw invalid_iterator();
    auto idx = pos.pos;
    d.erase(d.begin() + static_cast<long>(idx));
    return iterator(this, std::min(idx, d.size()));
  }

  void push_back(const T &value) { d.push_back(value); }
  void pop_back() { if (d.empty()) throw container_is_empty(); d.pop_back(); }
  void push_front(const T &value) { d.push_front(value); }
  void pop_front() { if (d.empty()) throw container_is_empty(); d.pop_front(); }
};

} // namespace sjtu

#endif
