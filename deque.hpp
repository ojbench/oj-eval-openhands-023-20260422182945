#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"
#include <cstddef>
#include <vector>
#include <algorithm>

namespace sjtu {

template <class T> class deque {
  static constexpr size_t BLOCK_CAP = 256;
  std::vector<std::vector<T>> blocks;
  size_t n = 0;

  // Locate global index -> (block index, offset inside block)
  std::pair<size_t,size_t> locate(size_t idx) const {
    size_t acc = 0;
    for (size_t bi = 0; bi < blocks.size(); ++bi) {
      const auto &b = blocks[bi];
      if (idx < acc + b.size()) return {bi, idx - acc};
      acc += b.size();
    }
    return {blocks.size(), 0};
  }
  void split_if_needed(size_t bi) {
    if (bi >= blocks.size()) return;
    auto &b = blocks[bi];
    if (b.size() > BLOCK_CAP * 2) {
      size_t mid = b.size()/2;
      std::vector<T> nb(b.begin() + mid, b.end());
      b.erase(b.begin() + mid, b.end());
      blocks.insert(blocks.begin() + bi + 1, std::move(nb));
    }
  }
  void merge_if_needed(size_t bi) {
    if (blocks.size()<=1 || bi>=blocks.size()) return;
    if (bi+1<blocks.size() && blocks[bi].size() + blocks[bi+1].size() < BLOCK_CAP) {
      // merge bi and bi+1
      auto &a = blocks[bi];
      auto &c = blocks[bi+1];
      a.insert(a.end(), c.begin(), c.end());
      blocks.erase(blocks.begin()+bi+1);
    } else if (bi>0 && blocks[bi-1].size() + blocks[bi].size() < BLOCK_CAP) {
      auto &a = blocks[bi-1];
      auto &c = blocks[bi];
      a.insert(a.end(), c.begin(), c.end());
      blocks.erase(blocks.begin()+bi);
    }
  }
  T &ref_at(size_t idx) {
    auto p = locate(idx);
    return blocks[p.first][p.second];
  }
  const T &ref_at(size_t idx) const {
    auto p = locate(idx);
    return blocks[p.first][p.second];
  }
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

    T &operator*() const { if (!owner || pos >= owner->n) throw invalid_iterator(); return owner->ref_at(pos); }
    T *operator->() const noexcept { return &owner->ref_at(pos); }

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

    const T &operator*() const { if (!owner || pos >= owner->n) throw invalid_iterator(); return owner->ref_at(pos); }
    const T *operator->() const noexcept { return &owner->ref_at(pos); }

    bool operator==(const const_iterator &rhs) const { return owner==rhs.owner && pos==rhs.pos; }
    bool operator!=(const const_iterator &rhs) const { return !(*this==rhs); }
  };

  deque() {}
  deque(const deque &other) : blocks(other.blocks), n(other.n) {}
  ~deque() {}
  deque &operator=(const deque &other) { if (this==&other) return *this; blocks = other.blocks; n = other.n; return *this; }

  T &at(const size_t &pos) { if (pos>=n) throw index_out_of_bound(); return ref_at(pos); }
  const T &at(const size_t &pos) const { if (pos>=n) throw index_out_of_bound(); return ref_at(pos); }
  T &operator[](const size_t &pos) { if (pos>=n) throw index_out_of_bound(); return ref_at(pos); }
  const T &operator[](const size_t &pos) const { if (pos>=n) throw index_out_of_bound(); return ref_at(pos); }

  const T &front() const { if (n==0) throw container_is_empty(); return blocks.front().front(); }
  const T &back() const { if (n==0) throw container_is_empty(); return blocks.back().back(); }

  iterator begin() { return iterator(this,0); }
  const_iterator cbegin() const { return const_iterator(this,0); }

  iterator end() { return iterator(this,n); }
  const_iterator cend() const { return const_iterator(this,n); }

  bool empty() const { return n==0; }
  size_t size() const { return n; }

  void clear() { blocks.clear(); n=0; }

  iterator insert(iterator pos, const T &value) {
    if (pos.owner != this) throw invalid_iterator();
    if (pos.pos > n) throw invalid_iterator();
    if (blocks.empty()) blocks.emplace_back();
    if (pos.pos == n) { // append
      blocks.back().push_back(value);
      ++n;
      split_if_needed(blocks.size()-1);
      return iterator(this, n-1);
    }
    auto p = locate(pos.pos);
    blocks[p.first].insert(blocks[p.first].begin()+static_cast<long>(p.second), value);
    ++n;
    split_if_needed(p.first);
    return iterator(this, pos.pos);
  }

  iterator erase(iterator pos) {
    if (n==0) throw container_is_empty();
    if (pos.owner != this) throw invalid_iterator();
    if (pos.pos >= n) throw invalid_iterator();
    auto p = locate(pos.pos);
    auto &b = blocks[p.first];
    b.erase(b.begin()+static_cast<long>(p.second));
    --n;
    if (b.empty()) {
      blocks.erase(blocks.begin()+static_cast<long>(p.first));
      if (blocks.empty()) return iterator(this,0);
      size_t nextIndex = std::min(pos.pos, n);
      return iterator(this, nextIndex);
    } else {
      merge_if_needed(p.first);
      size_t nextIndex = std::min(pos.pos, n);
      return iterator(this, nextIndex);
    }
  }

  void push_back(const T &value) {
    if (blocks.empty()) blocks.emplace_back();
    blocks.back().push_back(value);
    ++n;
    split_if_needed(blocks.size()-1);
  }
  void pop_back() { if (n==0) throw container_is_empty(); blocks.back().pop_back(); --n; if (blocks.back().empty()) blocks.pop_back(); }
  void push_front(const T &value) {
    if (blocks.empty()) blocks.emplace_back();
    blocks.front().insert(blocks.front().begin(), value);
    ++n;
    split_if_needed(0);
  }
  void pop_front() { if (n==0) throw container_is_empty(); blocks.front().erase(blocks.front().begin()); --n; if (blocks.front().empty()) blocks.erase(blocks.begin()); }
};

} // namespace sjtu

#endif
