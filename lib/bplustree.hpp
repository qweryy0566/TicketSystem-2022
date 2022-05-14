#include <fstream>

template <typename _key_type, typename _subkey_type, typename _value_type>
struct Element {
  _key_type key;
  _subkey_type subkey;
  _value_type value;
  friend bool operator<(const Element &x, const Element &y) {
    if (x.key != y.key)
      return (x.key < y.key);
    else
      return (x.subkey < y.subkey);
  }
  friend bool operator==(const Element &x, const Element &y) {
    return (x.key == y.key && x.subkey == y.subkey);
  }
  friend bool operator>(const Element &x, const Element &y) {
    if (x.key != y.key)
      return (x.key > y.key);
    else
      return (x.subkey > y.subkey);
  }
  friend bool operator<=(const Element &x, const Element &y) {
    return (x < y || x == y);
  }
  friend bool operator>=(const Element &x, const Element &y) {
    return (x > y || x == y);
  }
  friend bool operator!=(const Element &x, const Element &y) {
    return !(x.key == y.key && x.subkey == y.subkey);
  }
  Element<_key_type, _subkey_type, _value_type> &operator=(
      const Element<_key_type, _subkey_type, _value_type> &x) {
    this->key = x.key;        // need operator "="
    this->subkey = x.subkey;  // need operator "="
    this->value = x.value;    // need operator "="
    return *this;
  };
};

template <typename _key_type, typename _subkey_type, typename _value_type>
struct Lead {
  _key_type key;
  _subkey_type subkey;
  long long int son_pos = 0;
  friend bool operator<(const Lead &x, const Lead &y) {
    if (x.key != y.key)
      return (x.key < y.key);
    else
      return (x.subkey < y.subkey);
  }
  friend bool operator==(const Lead &x, const Lead &y) {
    return (x.key == y.key && x.subkey == y.subkey);
  }
  friend bool operator>(const Lead &x, const Lead &y) {
    if (x.key != y.key)
      return (x.key > y.key);
    else
      return (x.subkey > y.subkey);
  }
  friend bool operator<=(const Lead &x, const Lead &y) {
    return (x < y || x == y);
  }
  friend bool operator>=(const Lead &x, const Lead &y) {
    return (x > y || x == y);
  }
  friend bool operator!=(const Lead &x, const Lead &y) {
    return !(x.key == y.key && x.subkey == y.subkey);
  }
  Lead<_key_type, _subkey_type, _value_type> &operator=(
      const Lead<_key_type, _subkey_type, _value_type> &x) {
    this->key = x.key;
    this->subkey = x.subkey;
    this->son_pos = x.son_pos;
    return *this;
  }
};

template <typename _key_type, typename _subkey_type, typename _value_type>
int ElementCmp(const Element<_key_type, _subkey_type, _value_type> &x,
               const Element<_key_type, _subkey_type, _value_type> &y) {
  if (x.key < y.key)
    return -1;
  else if (x.key > y.key)
    return 1;
  else
    return 0;
}

template <typename _key_type, typename _subkey_type, typename _value_type>
int ElementLeadCmp(const Element<_key_type, _subkey_type, _value_type> &x,
                   const Lead<_key_type, _subkey_type, _value_type> &y) {
  if (x.key != y.key) {
    if (x.key < y.key)
      return -1;
    else
      return 1;
  } else {
    if (x.subkey < y.subkey)
      return -1;
    else if (x.subkey == y.subkey)
      return 0;
    else
      return 1;
  }
}

template <typename _key_type, typename _subkey_type, typename _value_type>
struct Node {
  int node_size = 0;
  Lead<_key_type, _subkey_type, _value_type> array_lead[100 + 5];
  int tag =
      0;  // tag==0 means it points to a node, tag==1 means it points to a block
};

template <typename _key_type, typename _subkey_type, typename _value_type>
struct Block {
  int block_size = 0;
  Element<_key_type, _subkey_type, _value_type> array_element[100 + 5];
  long long int last = 0;
  long long int next = 0;
};

template <typename _key_type, typename _subkey_type, typename _value_type>
class BPlusTree {
 private:
  std::fstream file_index_;
  std::fstream file_block_;
  int count_max_node = 0;
  int count_max_block = 0;

 public:
  std::string filename_index;
  std::string filename_block;

  long long int GetValidPosition(
      int x) {  // if x, return a place for the new node in file_index_
    long long int ans;
    if (x) {
      ans = 2 * sizeof(int) + sizeof(long long int) +
            count_max_node * sizeof(Node<_key_type, _subkey_type, _value_type>);
      count_max_node++;
    } else {
      ans =
          sizeof(int) +
          count_max_block * sizeof(Block<_key_type, _subkey_type, _value_type>);
      count_max_block++;
    }
    return ans;
  }

  BPlusTree(std::string name1, std::string name2)
      : filename_index(name1), filename_block(name2) {
    file_index_.open(filename_index);
    if (!file_index_) {
      file_index_.open(filename_index, std::ofstream::out);
      file_index_.close();
      file_index_.open(filename_index);
      int count_all = 0;
      int count_node = 0;
      long long int root_pos = 2 * sizeof(int) + sizeof(long long int);
      file_index_.seekp(0);
      file_index_.write(reinterpret_cast<char *>(&count_all), sizeof(int));
      file_index_.write(reinterpret_cast<char *>(&count_node), sizeof(int));
      file_index_.write(reinterpret_cast<char *>(&root_pos),
                        sizeof(long long int));
    }
    file_index_.close();
    file_block_.open(filename_block);
    if (!file_block_) {
      file_block_.open(filename_block, std::ofstream::out);
      file_block_.close();
      file_block_.open(filename_block);
      int count_block = 0;
      file_block_.seekp(0);
      file_block_.write(reinterpret_cast<char *>(&count_block), sizeof(int));
    }
    file_block_.close();
  };

  void Initialize() {}

  bool Insert(const _key_type &key, const _subkey_type &subkey,
              const _value_type &value) {
    Element<_key_type, _subkey_type, _value_type> new_element;
    new_element.key = key;
    new_element.subkey = subkey;
    new_element.value = value;

    file_index_.open(filename_index);
    int count_all = 0;
    file_index_.seekg(0);
    file_index_.read(reinterpret_cast<char *>(&count_all), sizeof(int));
    if (!count_all) {
      Lead<_key_type, _subkey_type, _value_type> new_lead;
      new_lead.key = key;
      new_lead.subkey = subkey;
      int pos_new_block = GetValidPosition(0);
      new_lead.son_pos = pos_new_block;

      file_block_.open(filename_block);
      Block<_key_type, _subkey_type, _value_type> new_block;
      new_block.block_size = 1;
      new_block.array_element[0] = new_element;
      new_block.last = -1;
      new_block.next = -1;
      file_block_.seekp(pos_new_block);
      file_block_.write(reinterpret_cast<char *>(&new_block),
                        sizeof(Block<_key_type, _subkey_type, _value_type>));
      int count_block = 1;
      file_block_.seekp(0);
      file_block_.write(reinterpret_cast<char *>(&count_block), sizeof(int));
      file_block_.close();

      Node<_key_type, _subkey_type, _value_type> new_node;
      new_node.tag = 1;
      new_node.node_size = 1;
      new_node[0] = new_lead;
      int pos_new_node = GetValidPosition(1);
      file_index_.seekp(pos_new_node);
      file_index_.write(reinterpret_cast<char *>(&new_node),
                        sizeof(Node<_key_type, _subkey_type, _value_type>));
      count_all++;
      file_index_.seekp(0);
      file_index_.write(reinterpret_cast<char *>(&count_all), sizeof(int));
      int count_node = 1;
      file_index_.write(reinterpret_cast<char *>(&count_node), sizeof(int));
      file_index_.write(reinterpret_cast<char *>(&pos_new_node),
                        sizeof(long long int));
      file_index_.close();

      return true;
    } else {
      Lead<_key_type, _subkey_type, _value_type> new_lead;
      new_lead.key = key;
      new_lead.subkey = subkey;
      long long int pos = 0;
      file_index_.seekg(2 * sizeof(int));
      file_index_.read(reinterpret_cast<char *>(&pos), sizeof(long long int));
      Node<_key_type, _subkey_type, _value_type> test_node;
      long long int sta[1000];
      int top = -1;
      while (true) {
        sta[++top] = pos;
        file_index_.seekg(pos);
        file_index_.read(reinterpret_cast<char *>(&test_node),
                         sizeof(Node<_key_type, _subkey_type, _value_type>));
        int h = 0, t = test_node.node_size - 1, mid, ind = -1;
        while (h <= t) {  // find the first one > new_lead
          mid = h + ((t - h) >> 1);
          if (new_lead < test_node.array_lead[mid]) {
            ind = mid;
            t = mid - 1;
          } else
            h = mid + 1;
        }
        if (ind == -1) {
          int k = test_node.node_size - 1;
          pos = test_node.array_lead[k].son_pos;
        } else {
          if (ind) {
            pos = test_node.array_lead[ind - 1].son_pos;
          } else {
            pos = test_node.array_lead[0].son_pos;
          }
        }
        if (test_node.tag == 1) break;
      }
      file_index_.close();

      file_block_.open(filename_block);
      Block<_key_type, _subkey_type, _value_type> test_block;
      file_block_.seekg(pos);
      file_block_.read(reinterpret_cast<char *>(&test_block),
                       sizeof(Block<_key_type, _subkey_type, _value_type>));
      int h = 0, t = test_block.block_size - 1, mid, ind = -1;
      while (h <= t) {  // find the first one >= targeted_element
        mid = h + ((t - h) >> 1);
        if (new_element <= test_block.array_element[mid]) {
          ind = mid;
          t = mid - 1;
        } else
          h = mid + 1;
      }
      if (ind != -1 &&
          new_element ==
              test_block.array_element[ind]) {  // new one has been in the block
        test_block.array_element[ind] = new_element;
        file_block_.seekp(pos);
        file_block_.read(reinterpret_cast<char *>(&test_block),
                         sizeof(Block<_key_type, _subkey_type, _value_type>));
        file_block_.close();

        return false;
      } else {
        if (ind == -1) {
          ind = test_block.block_size;
          test_block.array_element[ind] = new_element;
          test_block.block_size++;
          if (test_block.block_size <= 100) {
            file_block_.seekp(pos);
            file_block_.read(
                reinterpret_cast<char *>(&test_block),
                sizeof(Block<_key_type, _subkey_type, _value_type>));
            file_block_.close();

            file_index_.open(filename_index);
            count_all++;
            file_index_.seekp(0);
            file_index_.write(reinterpret_cast<char *>(&count_all),
                              sizeof(int));
            file_index_.close();

            return true;
          } else {
          }
        } else {
        }
      }
    }
  }

  bool Delete(const _key_type &key, const _subkey_type &subkey) {}

  bool Modify(const _key_type &key, const _subkey_type &subkey,
              const _value_type &value) {
    file_index_.open(filename_index);
    int count_all = 0;
    file_index_.seekg(0);
    file_index_.read(reinterpret_cast<char *>(&count_all), sizeof(int));
    if (!count_all) {
      file_index_.close();

      return false;
    }
    Lead<_key_type, _subkey_type, _value_type> targeted_lead;
    targeted_lead.key = key;
    targeted_lead.subkey = subkey;
    long long int pos = 0;
    file_index_.seekg(2 * sizeof(int));
    file_index_.read(reinterpret_cast<char *>(&pos), sizeof(long long int));
    Node<_key_type, _subkey_type, _value_type> test_node;
    while (true) {
      file_index_.seekg(pos);
      file_index_.read(reinterpret_cast<char *>(&test_node),
                       sizeof(Node<_key_type, _subkey_type, _value_type>));
      int h = 0, t = test_node.node_size - 1, mid, ind = -1;
      while (h <= t) {  // find the first one > targeted_lead
        mid = h + ((t - h) >> 1);
        if (targeted_lead < test_node.array_lead[mid]) {
          ind = mid;
          t = mid - 1;
        } else
          h = mid + 1;
      }
      if (ind == -1) {
        int k = test_node.node_size - 1;
        pos = test_node.array_lead[k].son_pos;
      } else {
        if (ind) {
          pos = test_node.array_lead[ind - 1].son_pos;
        } else {
          pos = test_node.array_lead[0].son_pos;
        }
      }
      if (test_node.tag == 1) break;
    }
    file_index_.close();

    file_block_.open(filename_block);
    Block<_key_type, _subkey_type, _value_type> test_block;
    file_block_.seekg(pos);
    file_block_.read(reinterpret_cast<char *>(&test_block),
                     sizeof(Block<_key_type, _subkey_type, _value_type>));
    Element<_key_type, _subkey_type, _value_type> targeted_element;
    targeted_element.key = key;
    targeted_element.subkey = subkey;
    int h = 0, t = test_block.block_size - 1, mid, ind = -1;
    while (h <= t) {  // find the first one >= targeted_element
      mid = h + ((t - h) >> 1);
      if (targeted_element <= test_block.array_element[mid]) {
        ind = mid;
        t = mid - 1;
      } else
        h = mid + 1;
    }
    if (ind == -1 || targeted_element != test_block.array_element[ind]) {
      file_block_.close();

      return false;
    } else {
      test_block.array_element[ind].value = value;
      file_block_.seekp(pos);
      file_block_.write(reinterpret_cast<char *>(&test_block),
                        sizeof(Block<_key_type, _subkey_type, _value_type>));
      file_block_.close();

      return true;
    }
  }

  void Clear() {}

  bool Exist(const _key_type &key) {
    file_index_.open(filename_index);
    int count_all = 0;
    file_index_.seekg(0);
    file_index_.read(reinterpret_cast<char *>(&count_all), sizeof(int));
    if (!count_all) {
      file_index_.close();

      return false;
    }
    long long int pos = 0;
    file_index_.seekg(2 * sizeof(int));
    file_index_.read(reinterpret_cast<char *>(&pos), sizeof(long long int));
    Node<_key_type, _subkey_type, _value_type> test_node;
    while (true) {
      file_index_.seekg(pos);
      file_index_.read(reinterpret_cast<char *>(&test_node),
                       sizeof(Node<_key_type, _subkey_type, _value_type>));
      int h = 0, t = test_node.node_size - 1, mid, ind = -1;
      while (h <= t) {  // find the first one > targeted_lead
        mid = h + ((t - h) >> 1);
        if (key < test_node.array_lead[mid].key) {
          ind = mid;
          t = mid - 1;
        } else
          h = mid + 1;
      }
      if (ind == -1) {
        int k = test_node.node_size - 1;
        pos = test_node.array_lead[k].son_pos;
      } else {
        if (ind) {
          pos = test_node.array_lead[ind - 1].son_pos;
        } else {
          pos = test_node.array_lead[0].son_pos;
        }
      }
      if (test_node.tag == 1) break;
    }
    file_index_.close();

    file_block_.open(filename_block);
    Block<_key_type, _subkey_type, _value_type> test_block;
    file_block_.seekg(pos);
    file_block_.read(reinterpret_cast<char *>(&test_block),
                     sizeof(Block<_key_type, _subkey_type, _value_type>));
    int h = 0, t = test_block.block_size - 1, mid, ind = -1;
    while (h <= t) {  // find the first one >= targeted_element
      mid = h + ((t - h) >> 1);
      if (key <= test_block.array_element[mid].key) {
        ind = mid;
        t = mid - 1;
      } else
        h = mid + 1;
    }
    file_block_.close();

    if (ind == -1 || key != test_block.array_element[ind].key) {
      return false;
    } else {
      return true;
    }
  }

  bool Exist(const _key_type &key, const _subkey_type &subkey) {
    file_index_.open(filename_index);
    int count_all = 0;
    file_index_.seekg(0);
    file_index_.read(reinterpret_cast<char *>(&count_all), sizeof(int));
    if (!count_all) {
      file_index_.close();

      return false;
    }
    Lead<_key_type, _subkey_type, _value_type> targeted_lead;
    targeted_lead.key = key;
    targeted_lead.subkey = subkey;

    long long int pos = 0;
    file_index_.seekg(2 * sizeof(int));
    file_index_.read(reinterpret_cast<char *>(&pos), sizeof(long long int));
    Node<_key_type, _subkey_type, _value_type> test_node;
    while (true) {
      file_index_.seekg(pos);
      file_index_.read(reinterpret_cast<char *>(&test_node),
                       sizeof(Node<_key_type, _subkey_type, _value_type>));
      int h = 0, t = test_node.node_size - 1, mid, ind = -1;
      while (h <= t) {  // find the first one > targeted_lead
        mid = h + ((t - h) >> 1);
        if (targeted_lead < test_node.array_lead[mid]) {
          ind = mid;
          t = mid - 1;
        } else
          h = mid + 1;
      }
      if (ind == -1) {
        int k = test_node.node_size - 1;
        pos = test_node.array_lead[k].son_pos;
      } else {
        if (ind) {
          pos = test_node.array_lead[ind - 1].son_pos;
        } else {
          pos = test_node.array_lead[0].son_pos;
        }
      }
      if (test_node.tag == 1) break;
    }
    file_index_.close();

    file_block_.open(filename_block);
    Block<_key_type, _subkey_type, _value_type> test_block;
    file_block_.seekg(pos);
    file_block_.read(reinterpret_cast<char *>(&test_block),
                     sizeof(Block<_key_type, _subkey_type, _value_type>));
    Element<_key_type, _subkey_type, _value_type> targeted_element;
    targeted_element.key = key;
    targeted_element.subkey = subkey;
    int h = 0, t = test_block.block_size - 1, mid, ind = -1;
    while (h <= t) {  // find the first one >= targeted_element
      mid = h + ((t - h) >> 1);
      if (targeted_element <= test_block.array_element[mid]) {
        ind = mid;
        t = mid - 1;
      } else
        h = mid + 1;
    }
    file_block_.close();

    if (ind == -1 || targeted_element != test_block.array_element[ind]) {
      return false;
    } else {
      return true;
    }
  }

  std::vector<_value_type> Traverse() {}

  std::vector<_value_type> Traverse(const _key_type &key) {}

  [[nodiscard]] _value_type Get(const _key_type &key,
                                const _subkey_type &subkey) {
    file_index_.open(filename_index);
    int count_all = 0;
    file_index_.seekg(0);
    file_index_.read(reinterpret_cast<char *>(&count_all), sizeof(int));
    if (!count_all) {
      file_index_.close();

      return _value_type();
    }
    Lead<_key_type, _subkey_type, _value_type> targeted_lead;
    targeted_lead.key = key;
    targeted_lead.subkey = subkey;
    long long int pos = 0;
    file_index_.seekg(2 * sizeof(int));
    file_index_.read(reinterpret_cast<char *>(&pos), sizeof(long long int));
    Node<_key_type, _subkey_type, _value_type> test_node;
    while (true) {
      file_index_.seekg(pos);
      file_index_.read(reinterpret_cast<char *>(&test_node),
                       sizeof(Node<_key_type, _subkey_type, _value_type>));
      int h = 0, t = test_node.node_size - 1, mid, ind = -1;
      while (h <= t) {  // find the first one > targeted_lead
        mid = h + ((t - h) >> 1);
        if (targeted_lead < test_node.array_lead[mid]) {
          ind = mid;
          t = mid - 1;
        } else
          h = mid + 1;
      }
      if (ind == -1) {
        int k = test_node.node_size - 1;
        pos = test_node.array_lead[k].son_pos;
      } else {
        if (ind) {
          pos = test_node.array_lead[ind - 1].son_pos;
        } else {
          pos = test_node.array_lead[0].son_pos;
        }
      }
      if (test_node.tag == 1) break;
    }
    file_index_.close();

    file_block_.open(filename_block);
    Block<_key_type, _subkey_type, _value_type> test_block;
    file_block_.seekg(pos);
    file_block_.read(reinterpret_cast<char *>(&test_block),
                     sizeof(Block<_key_type, _subkey_type, _value_type>));
    Element<_key_type, _subkey_type, _value_type> targeted_element;
    targeted_element.key = key;
    targeted_element.subkey = subkey;
    int h = 0, t = test_block.block_size - 1, mid, ind = -1;
    while (h <= t) {  // find the first one >= targeted_element
      mid = h + ((t - h) >> 1);
      if (targeted_element <= test_block.array_element[mid]) {
        ind = mid;
        t = mid - 1;
      } else
        h = mid + 1;
    }
    file_block_.close();

    if (ind == -1 || targeted_element != test_block.array_element[ind]) {
      return _value_type();
    } else {
      return test_block.array_element[ind].value;
    }
  }
};