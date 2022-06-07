#ifndef TICKETSYSTEM_ROLLBACK_HPP_
#define TICKETSYSTEM_ROLLBACK_HPP_

#include "../lib/bplustree.hpp"

template <class _key_type, class _subkey_type, class _value_type, int M, int L,
          int LL>
class BptPlus {
  struct History {
    enum Opt { INSERT, DELETE, MODIFY };
    _key_type key;
    _subkey_type subkey;
    Opt opt;
    int timestamp, cnt;
    _value_type value;

    History() {}
    History(const _key_type &key, const _subkey_type &subkey, const Opt &opt,
            const int &timestamp, const int &cnt,
            const _value_type &value = _value_type{})
        : key{key},
          subkey{subkey},
          opt{opt},
          timestamp{timestamp},
          cnt{cnt},
          value{value} {}
  };
  int last_stamp{0}, cnt;
  BPlusTree<_key_type, _subkey_type, _value_type, M, L> data;
  BPlusTree<int, int, History, 253, LL> history;

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
    history.Insert(timestamp, cnt,
                   History{key, subkey, History::INSERT, timestamp, cnt});
    data.Insert(key, subkey, value);
  }
  bool Delete(const int &timestamp, const _key_type &key,
              const _subkey_type &subkey) {
    UpdateCnt(timestamp);
    history.Insert(timestamp, cnt,
                   History{key, subkey, History::DELETE, timestamp, cnt,
                           data.Get(key, subkey)});
    return data.Delete(key, subkey);
  }
  void Modify(const int &timestamp, const _key_type &key,
              const _subkey_type &subkey, const _value_type &value) {
    UpdateCnt(timestamp);
    history.Insert(timestamp, cnt,
                   History{key, subkey, History::MODIFY, timestamp, cnt,
                           data.Get(key, subkey)});
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

  void RollBack(const int &timestamp, const int &now) {
    int pos{now};
    do {
      pos = std::max(timestamp, pos - 100000);
      vector<History> opts{history.GetSuf(pos)};
      for (int i = opts.size() - 1; ~i; --i) {
        if (opts[i].opt == History::INSERT)
          data.Delete(opts[i].key, opts[i].subkey);
        else if (opts[i].opt == History::DELETE)
          data.Insert(opts[i].key, opts[i].subkey, opts[i].value);
        else
          data.Modify(opts[i].key, opts[i].subkey, opts[i].value);
        history.Delete(opts[i].timestamp, opts[i].cnt);
      }
    } while (pos > timestamp);
  }
  void Clear() {}
};

#endif  // TICKETSYSTEM_ROLLBACK_HPP_