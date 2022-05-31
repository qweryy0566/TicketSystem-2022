#ifndef TICKETSYSTEM_TESTLOGIC_HPP_
#define TICKETSYSTEM_TESTLOGIC_HPP_

#include <map>

#include "utils.hpp"

template <typename _key_type, typename _subkey_type, typename _value_type>
class BPlusTree {
  std::map<std::pair<_key_type, _subkey_type>, _value_type> false_tree;

 public:
  BPlusTree(std::string name1, std::string name2) {}
  size_t Size() const { return false_tree.size(); }
  bool Insert(const _key_type &key, const _subkey_type &subkey,
              const _value_type &value) {
    return false_tree.insert({std::make_pair(key, subkey), value}).second;
  }
  bool Delete(const _key_type &key, const _subkey_type &subkey) {
    auto it{false_tree.find(std::make_pair(key, subkey))};
    if (it == false_tree.end()) return 0;
    return false_tree.erase(it), 1;
  }
  bool Modify(const _key_type &key, const _subkey_type &subkey,
              const _value_type &value) {
    auto it{false_tree.find(std::make_pair(key, subkey))};
    if (it == false_tree.end()) return 0;
    false_tree.erase(it);
    return false_tree.insert({std::make_pair(key, subkey), value}).second;
  }
  bool Exist(const _key_type &key, const _subkey_type subkey = _subkey_type{}) {
    auto it{false_tree.find(std::make_pair(key, subkey))};
    return it != false_tree.end();
  }
  _value_type Get(const _key_type &key, const _subkey_type &subkey) {
    return false_tree[std::make_pair(key, subkey)];
  }
  vector<_value_type> Traverse(const _key_type &key) {
    vector<_value_type> ret{};
    for (auto it{false_tree.lower_bound({key, std::numeric_limits<_subkey_type>::min()})};
         it != false_tree.end() && it->first.first == key; ++it)
      ret.push_back(it->second);
    return ret;
  }
  void Clear() { false_tree.clear(); }
};

#endif  // TICKETSYSTEM_TESTLOGIC_HPP_