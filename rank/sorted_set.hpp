#ifndef SORTED_SET
#define SORTED_SET

#include <memory>
#include <set>
#include <unordered_map>

template <class Key, class T>
class sorted_set {
 public:
  using key_type = Key;
  using value_type = T;
  using size_type = std::size_t;

  // default: care of top 1000
  static constexpr size_type default_top = 1000;

  struct key_value_type {
    key_type first;
    value_type second;
    key_value_type(const key_type& k, const value_type& v)
        : first(k), second(v) {}
  };
  using key_value_ptr = std::shared_ptr<key_value_type>;

  struct key_value_less {
    constexpr bool operator()(const key_value_ptr& left,
                              const key_value_ptr& right) const {
      if (left->second == right->second) {
        return left->first < right->first;
      }
      return left->second < right->second;
    }
  };

  sorted_set() : top_(default_top) {}
  explicit sorted_set(size_type top) : top_(top == 0 ? default_top : top) {}
  sorted_set(const sorted_set&) = delete;
  sorted_set& operator=(const sorted_set&) = delete;
  ~sorted_set() = default;

  sorted_set(sorted_set&& right)
      : top_(right.top_),
        map_(std::move(right.map_)),
        set_(std::move(right.set_)) {}

  sorted_set& operator=(sorted_set&& right) noexcept {
    top_ = right.top_;
    map_ = std::move(right.map_);
    set_ = std::move(right.set_);
    return *this;
  }

  // ranged-for: large->small
  auto begin() const { return set_.rbegin(); }
  auto end() const { return set_.rend(); }

  auto find(key_type k) { return map_.find(k); }
  auto map_end() { return map_.end(); }

  void insert(const key_type& k, const value_type& v) {
    auto add = [&] {
      key_value_ptr ptr = std::make_shared<key_value_type>(k, v);
      map_.emplace(k, ptr);
      set_.insert(ptr);
    };

    auto it = map_.find(k);
    if (it == map_.end()) {
      if (set_.size() < top_) {
        add();
      } else if (set_.size() == top_) {
        auto val = v;
        if (val < (*set_.begin())->second) return;
        map_.erase((*set_.begin())->first);
        set_.erase(set_.begin());
        add();
      }
    } else {
      set_.erase(it->second);
      map_.erase(it);
      add();
    }
  }

  void erase(const key_type& k) {
    auto it = map_.find(k);
    if (it == map_.end()) return;
    set_.erase(it->second);
    map_.erase(it);
  }

  void clear() {
    set_.clear();
    map_.clear();
  }

  size_type size() { return set_.size(); }

  size_type rank(const key_type& k) {
    auto it = map_.find(k);
    if (it == map_.end() || set_.empty()) return 0;
    size_type rank = 0;
    for (auto n = set_.rbegin(); n != set_.rend(); ++n) {
      ++rank;
      if ((*n)->first == k) return rank;
    }
    return 0;
  }

 private:
  size_type top_;
  std::unordered_map<key_type, key_value_ptr> map_;
  std::set<key_value_ptr, key_value_less> set_;
};

#endif
