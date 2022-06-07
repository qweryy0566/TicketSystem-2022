#ifndef TICKETSYSTEM_ROLLBACK_HPP_
#define TICKETSYSTEM_ROLLBACK_HPP_

#include "../lib/bplustree.hpp"

template <class _key_type, class _subkey_type, class _value_type, int M, int L,
          int LL>
class BptPlus {
  struct History {
    _key_type key;
    _subkey_type subkey;
    _value_type value;
    bool is_empty, is_delete;

    History() {}
    History(const _key_type &key, const _subkey_type &subkey)
        : key{key}, subkey{subkey}, value{}, is_empty{1} {}
    History(const _key_type &key, const _subkey_type &subkey,
            const _value_type &value, const bool &p = 0)
        : key{key}, subkey{subkey}, value{value}, is_empty{0}, is_delete{p} {}
  };
  int last_stamp{0}, cnt;
  BPlusTree<_key_type, _subkey_type, _value_type, M, L> data;
  BPlusTree<int, int, History, 509, LL> history;

  void UpdateCnt(const int &timestamp) {
    if (timestamp != last_stamp) last_stamp = timestamp, cnt = -1;
    ++cnt;
  }

 public:
  BptPlus(const string &name1, const string &name2)
      : data{name1 + ".dat", name2 + ".dat"},
        history{name1 + "_history.dat", name2 + "_history.dat"} {}

  void Insert(const int &timestamp, const _key_type &key,
              const _subkey_type &subkey, const _value_type &value) {
    UpdateCnt(timestamp);
    history.Insert(timestamp, cnt, History{key, subkey});
    data.Insert(key, subkey, value);
  }
  bool Delete(const int &timestamp, const _key_type &key,
              const _subkey_type &subkey) {
    UpdateCnt(timestamp);
    history.Insert(timestamp, cnt,
                   History{key, subkey, data.Get(key, subkey), 1});
    return data.Delete(key, subkey);
  }
  void Modify(const int &timestamp, const _key_type &key,
              const _subkey_type &subkey, const _value_type &value) {
    UpdateCnt(timestamp);
    history.Insert(timestamp, cnt,
                   History{key, subkey, data.Get(key, subkey)});
    data.Modify(key, subkey, value);
  }

  int Size() { return data.Size(); }
  bool Exist(const _key_type &key, const _subkey_type &subkey) {
    return data.Exist(key, subkey);
  }
  _value_type Get(const _key_type &key, const _subkey_type &subkey) {
    return data.Get(key, subkey);
  }
  vector<_value_type> Traverse(const _key_type &key) {
    return data.Traverse(key);
  }

  void RollBack(const int &timestamp) {
    for (; last_stamp > timestamp; --last_stamp) {
      vector<History> opts{history.Traverse(last_stamp)};
      for (int i = opts.size() - 1; ~i; --i) {
        if (opts[i].is_empty)
          data.Delete(opts[i].key, opts[i].subkey);
        else if (opts[i].is_delete)
          data.Insert(opts[i].key, opts[i].subkey, opts[i].value);
        else
          data.Modify(opts[i].key, opts[i].subkey, opts[i].value);
        history.Delete(last_stamp, i);
      }
    }
  }
  void Clear() {}
};

#endif  // TICKETSYSTEM_ROLLBACK_HPP_