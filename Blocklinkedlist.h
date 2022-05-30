#include <fstream>

template<typename _key_type, typename _subkey_type, typename _value_type>
struct element{
    _key_type key;
    _subkey_type subkey;
    _value_type value;
    friend bool operator<(const element &x,const element &y){if(x.key!=y.key) return (x.key<y.key);else return (x.subkey<y.subkey);}
    friend bool operator==(const element &x,const element &y){return (x.key==y.key&&x.subkey==y.subkey);}
    friend bool operator>(const element &x,const element &y){if(x.key!=y.key) return (x.key>y.key);else return (x.subkey>y.subkey);}
    friend bool operator<=(const element &x,const element &y){return (x<y||x==y);}
    friend bool operator>=(const element &x,const element &y){return (x>y||x==y);}
    friend bool operator!=(const element &x,const element &y){return !(x.key==y.key&&x.subkey==y.subkey);}
    element<_key_type, _subkey_type, _value_type>& operator=(const element<_key_type, _subkey_type, _value_type> &x){
        this->key=x.key;
        this->subkey=x.subkey;
        this->value=x.value;
        return (*this);
    };
};

template<typename _key_type, typename _subkey_type, typename _value_type>
struct block{element<_key_type, _subkey_type, _value_type> element_array[960+5];};

template<typename _key_type, typename _subkey_type, typename _value_type>
struct lead{
    element<_key_type, _subkey_type, _value_type> this_element;
    long long int last;
    long long int next;
    int block_size;
    long long int location;
};

template<typename _key_type, typename _subkey_type, typename _value_type>
int Element_Lead_cmp(const element<_key_type, _subkey_type, _value_type> &x,const lead<_key_type, _subkey_type, _value_type> &y){
    if(x<y.this_element) return -1;
    else if(x>y.this_element) return 1;
    else return 0;
}

template<typename _key_type, typename _subkey_type, typename _value_type>
int Element_cmp(const element<_key_type, _subkey_type, _value_type> &x,const element<_key_type, _subkey_type, _value_type> &y){
    if(x.key<y.key) return -1;
    else if(x.key>y.key) return 1;
    else return 0;
}

template<typename _key_type, typename _subkey_type, typename _value_type>
class BlockLinkedList {
private:
    std::fstream index_;
    std::fstream lead_search;

public:
    std::string file_name_lead;
    std::string file_name_all;

    BlockLinkedList(std::string name1,std::string name2):file_name_all(name1),file_name_lead(name2){
        element<_key_type, _subkey_type, _value_type> min_element;
        min_element.key=std::numeric_limits<_key_type>::min();
        min_element.subkey=std::numeric_limits<_subkey_type>::min();
        min_element.value=std::numeric_limits<_value_type>::min();
        element<_key_type, _subkey_type, _value_type> max_element;
        max_element.key=std::numeric_limits<_key_type>::max();
        max_element.subkey=std::numeric_limits<_subkey_type>::max();
        max_element.value=std::numeric_limits<_value_type>::max();
        index_.clear();
        index_.open(file_name_all);
        if(!index_){
            index_.open(file_name_all,std::ofstream::out);
            index_.close();
            index_.open(file_name_all);
            block<_key_type, _subkey_type, _value_type> first_block;
            first_block.element_array[0]=min_element;
            index_.seekp(0,std::ios::beg);
            index_.write(reinterpret_cast<char *>(&first_block), sizeof(block<_key_type, _subkey_type, _value_type>));
            first_block.element_array[0]=max_element;
            index_.seekp(sizeof(block<_key_type, _subkey_type, _value_type>),std::ios::beg);
            index_.write(reinterpret_cast<char *>(&first_block), sizeof(block<_key_type, _subkey_type, _value_type>));
        }
        index_.close();
        lead_search.clear();
        lead_search.open(file_name_lead);
        if(!lead_search){
            lead_search.open(file_name_lead,std::ofstream::out);
            lead_search.close();
            lead_search.open(file_name_lead);
            int m = 2;
            lead_search.seekp(0,std::ios::beg);
            lead_search.write(reinterpret_cast<char *>(&m), sizeof(int));
            lead_search.seekp(sizeof(int),std::ios::beg);
            lead_search.write(reinterpret_cast<char *>(&m), sizeof(int));
            long long int head_location = 2 * sizeof(int) + sizeof(long long int);
            lead_search.seekp(2 * sizeof(int),std::ios::beg);
            lead_search.write(reinterpret_cast<char *>(&head_location), sizeof(long long int));
            lead<_key_type, _subkey_type, _value_type> min_lead;
            min_lead.block_size = 1;
            min_lead.location = 0;
            min_lead.last = head_location+sizeof(lead<_key_type, _subkey_type, _value_type>);
            min_lead.next = min_lead.last;
            min_lead.this_element = min_element;
            lead_search.seekp(head_location,std::ios::beg);
            lead_search.write(reinterpret_cast<char *>(&min_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
            lead<_key_type, _subkey_type, _value_type> max_lead;
            max_lead.block_size = 1;
            max_lead.location = sizeof(block<_key_type, _subkey_type, _value_type>);
            max_lead.last = head_location;
            max_lead.next = max_lead.last;
            max_lead.this_element = max_element;
            lead_search.seekp(min_lead.next,std::ios::beg);
            lead_search.write(reinterpret_cast<char *>(&max_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
        }
        lead_search.close();
    }

    bool Insert(const _key_type& key, const _subkey_type& subkey, const _value_type& value) {
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        element<_key_type, _subkey_type, _value_type> new_ele;
        new_ele.value = value;
        new_ele.subkey = subkey;
        new_ele.key = key;
        int lead_count;
        lead_search.seekg(sizeof(int));
        lead_search.read(reinterpret_cast<char *>(&lead_count), sizeof(int));
        long long int loc;
        lead_search.seekg(2 * sizeof(int));
        lead_search.read(reinterpret_cast<char *>(&loc), sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        for (int i = 0; i < lead_count; ++i) {//search the lead
            lead_search.seekg(loc, std::ios::beg);
            lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
            int cmp = Element_Lead_cmp(new_ele, test_lead);
            if (cmp == 0) {
                lead_search.close();
                index_.close();
                return false;
            }
            else if (cmp < 0 || (cmp > 0 && i == lead_count - 1)) {
                if (i == 0 && cmp < 0) {
                    lead_search.close();
                    index_.close();
                    return false;
                } else {
                    if (cmp < 0) {
                        loc = test_lead.last;
                        lead_search.seekg(loc, std::ios::beg);
                        lead_search.read(reinterpret_cast<char *>(&test_lead),
                                         sizeof(lead<_key_type, _subkey_type, _value_type>));
                    }
                    const long long int all_loc = test_lead.location;
                    const int block_size = test_lead.block_size;
                    block<_key_type, _subkey_type, _value_type> test_block;
                    index_.clear();
                    index_.seekg(all_loc);
                    index_.read(reinterpret_cast<char *>(&test_block),
                                sizeof(block<_key_type, _subkey_type, _value_type>));
                    element<_key_type, _subkey_type, _value_type> test_element;
                    if (block_size == 1) {
                        test_element = test_block.element_array[0];
                        if (test_element < new_ele) {
                            test_lead.block_size++;
                            lead_search.seekp(loc, std::ios::beg);
                            lead_search.write(reinterpret_cast<char *>(&test_lead),
                                              sizeof(lead<_key_type, _subkey_type, _value_type>));
                            test_block.element_array[1] = new_ele;
                        }
                        index_.clear();
                        index_.seekp(all_loc, std::ios::beg);
                        index_.write(reinterpret_cast<char *>(&test_block),
                                     sizeof(block<_key_type, _subkey_type, _value_type>));
                        index_.close();
                        lead_search.close();
                        return true;
                    }
                    int h = 0, t = block_size - 1, j = 0;
                    while (h < t) {
                        j = (h + t + 1) / 2;
                        if (new_ele < test_block.element_array[j]) t = j - 1;
                        else h = j;
                    }
                    j = (h + t) / 2;
                    if (test_block.element_array[j] == new_ele) {
                        index_.close();
                        lead_search.close();
                        return false;
                    }
                    ++j;
                    for (int k = block_size - 1; k >= j; --k)
                        test_block.element_array[k + 1] = test_block.element_array[k];
                    test_block.element_array[j] = new_ele;
                    test_lead.block_size++;//change here
                    if (test_lead.block_size < 960) {
                        lead_search.seekp(loc, std::ios::beg);
                        lead_search.write(reinterpret_cast<char *>(&test_lead),
                                          sizeof(lead<_key_type, _subkey_type, _value_type>));
                        index_.seekp(all_loc);
                        index_.write(reinterpret_cast<char *>(&test_block),
                                     sizeof(block<_key_type, _subkey_type, _value_type>));
                        lead_search.close();
                        index_.close();
                        return true;
                    } else {
                        const int mid = 960 / 2;
                        int max_count = 0;
                        lead_search.seekg(0);
                        lead_search.read(reinterpret_cast<char *>(&max_count), sizeof(int));
                        const long long int new_block_loc =
                                max_count * sizeof(block<_key_type, _subkey_type, _value_type>);
                        block<_key_type, _subkey_type, _value_type> new_block;
                        for (int k = mid; k < test_lead.block_size; k++)
                            new_block.element_array[k - mid] = test_block.element_array[k];
                        index_.seekp(new_block_loc, std::ios::beg);
                        index_.write(reinterpret_cast<char *>(&new_block),
                                     sizeof(block<_key_type, _subkey_type, _value_type>));
                        lead<_key_type, _subkey_type, _value_type> new_lead;
                        new_lead.block_size = test_lead.block_size - mid;
                        new_lead.location = new_block_loc;
                        new_lead.this_element = new_block.element_array[0];
                        new_lead.last = loc;
                        new_lead.next = test_lead.next;
                        const long long int new_lead_loc =
                                2 * sizeof(int) + sizeof(long long int) +
                                max_count * sizeof(lead<_key_type, _subkey_type, _value_type>);
                        lead_search.seekp(new_lead_loc, std::ios::beg);
                        lead_search.write(reinterpret_cast<char *>(&new_lead),
                                          sizeof(lead<_key_type, _subkey_type, _value_type>));
                        lead_search.seekg(test_lead.next, std::ios::beg);
                        lead_search.read(reinterpret_cast<char *>(&new_lead),
                                         sizeof(lead<_key_type, _subkey_type, _value_type>));
                        new_lead.last = new_lead_loc;
                        lead_search.seekp(test_lead.next, std::ios::beg);
                        lead_search.write(reinterpret_cast<char *>(&new_lead),
                                          sizeof(lead<_key_type, _subkey_type, _value_type>));
                        test_lead.block_size = mid;
                        test_lead.next = new_lead_loc;
                        lead_search.seekp(loc, std::ios::beg);
                        lead_search.write(reinterpret_cast<char *>(&test_lead),
                                          sizeof(lead<_key_type, _subkey_type, _value_type>));
                        int lead_count=0;
                        lead_search.seekg(sizeof(int));
                        lead_search.read(reinterpret_cast<char *>(&lead_count), sizeof(int));
                        lead_count++;
                        lead_search.seekp(sizeof(int), std::ios::beg);
                        lead_search.write(reinterpret_cast<char *>(&lead_count), sizeof(int));
                        index_.clear();
                        index_.seekp(all_loc);
                        index_.write(reinterpret_cast<char *>(&test_block),
                                     sizeof(block<_key_type, _subkey_type, _value_type>));
                        index_.close();
                        max_count++;
                        lead_search.seekp(0);
                        lead_search.write(reinterpret_cast<char *>(&max_count), sizeof(int));
                        lead_search.close();
                        return true;
                    }
                }
            } else {
                loc = test_lead.next;
                continue;
            }
        }
        lead_search.close();
        index_.close();
        return false;
    }

    bool Modify(const _key_type& key, const _subkey_type& subkey, const _value_type& value){
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        element<_key_type, _subkey_type, _value_type> new_ele;
        new_ele.subkey=subkey;
        new_ele.key=key;
        int lead_count;
        lead_search.seekg(sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_count),sizeof(int));
        long long int loc;
        lead_search.seekg(2*sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&loc),sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        for(int i=0;i<lead_count;++i){//search the lead
            lead_search.seekg(loc,std::ios::beg);
            lead_search.read(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
            int cmp=Element_Lead_cmp(new_ele,test_lead);
            if(cmp==0) {
                test_lead.this_element.value=value;
                lead_search.seekp(loc,std::ios::beg);
                lead_search.write(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
                lead_search.close();
                long long int all_loc=test_lead.location;
                block<_key_type, _subkey_type, _value_type> test_block;
                index_.clear();
                index_.seekg(all_loc);
                index_.read(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                test_block.element_array[0].value=value;
                index_.seekp(all_loc);
                index_.write(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                index_.close();
                return true;
            }
            else if(cmp<0||(cmp>0&&i==lead_count-1)) {
                if (i == 0 && cmp < 0) {
                    lead_search.close();
                    index_.close();
                    return false;
                }
                else {
                    if (cmp < 0) {
                        loc = test_lead.last;
                        lead_search.seekg(loc, std::ios::beg);
                        lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                    }
                    const long long int all_loc = test_lead.location;
                    const int block_size = test_lead.block_size;
                    block<_key_type, _subkey_type, _value_type> test_block;
                    index_.clear();
                    index_.seekg(all_loc);
                    index_.read(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                    if (block_size == 1) {
                        if (test_block.element_array[0] != new_ele) {
                            index_.close();
                            lead_search.close();
                            return false;
                        } else {
                            test_lead.this_element.value=value;
                            lead_search.seekp(loc,std::ios::beg);
                            lead_search.write(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
                            lead_search.close();
                            test_block.element_array[0].value=value;
                            index_.seekp(all_loc);
                            index_.write(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                            index_.close();
                            return true;
                        }
                    }
                    int h = 0, t = block_size - 1, j = 0;
                    while (h < t) {
                        j = (h + t + 1) / 2;
                        if (new_ele<test_block.element_array[j]) t = j - 1;
                        else h = j;
                    }
                    j = (h + t) / 2;
                    if (test_block.element_array[j] == new_ele){
                        lead_search.close();
                        test_block.element_array[j].value=value;
                        index_.seekp(all_loc);
                        index_.write(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                        index_.close();
                        return true;
                    }
                    else{
                        index_.close();
                        lead_search.close();
                        return false;
                    }
                }
            }
            else{
                loc=test_lead.next;
                continue;
            }
        }
        lead_search.close();
        index_.close();
        return false;
    }

    bool Erase(const _key_type& key, const _subkey_type& subkey){
        element<_key_type, _subkey_type, _value_type> targeted_element;
        targeted_element.key=key;
        targeted_element.subkey=subkey;
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        int lead_count;
        lead_search.seekg(sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_count),sizeof(int));
        long long int lead_loc;
        lead_search.seekg(2*sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_loc),sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        int lead_cmp=0;
        for(int i=0;i<lead_count;++i) {
            lead_search.seekg(lead_loc,std::ios::beg);
            lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
            lead_cmp = Element_Lead_cmp(targeted_element, test_lead);
            if (i==0&&lead_cmp < 0) {
                index_.close();
                lead_search.close();
                return false;
            } else if (lead_cmp > 0) {
                const long long int all_loc = test_lead.location;
                const int block_size = test_lead.block_size;
                if(block_size==1){
                    lead_loc = test_lead.next;
                    continue;
                }
                block<_key_type, _subkey_type, _value_type> test_block;
                index_.clear();
                index_.seekg(all_loc);
                index_.read(reinterpret_cast<char*>(&test_block),sizeof(block<_key_type, _subkey_type, _value_type>));
                int h = 0, t = block_size - 1, j = 0;
                while (h < t) {
                    j = (h + t + 1) / 2;
                    if (test_block.element_array[j] > targeted_element) t = j - 1;
                    else h = j;
                }
                j = (h + t) / 2;
                if(test_block.element_array[j]!=targeted_element){
                    lead_loc = test_lead.next;
                    continue;
                }
                else{
                    for(int k=j+1;k<block_size;k++) test_block.element_array[k-1]=test_block.element_array[k];
                    index_.seekp(all_loc,std::ios::beg);
                    index_.write(reinterpret_cast<char*>(&test_block),sizeof(block<_key_type, _subkey_type, _value_type>));
                    test_lead.block_size--;
                    lead_search.seekp(lead_loc,std::ios::beg);
                    lead_search.write(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
                    index_.close();
                    lead_search.close();
                    return true;
                }
            } else {
                long long int all_loc = test_lead.location;
                const int block_size = test_lead.block_size;
                if(block_size==1){
                    lead_count--;
                    lead_search.seekp(sizeof(int),std::ios::beg);
                    lead_search.write(reinterpret_cast<char*>(&lead_count),sizeof(int));
                    long long int next=test_lead.next;
                    long long int last=test_lead.last;
                    lead_search.seekg(next,std::ios::beg);
                    lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                    test_lead.last=last;
                    lead_search.seekp(next,std::ios::beg);
                    lead_search.write(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                    lead_search.seekg(last,std::ios::beg);
                    lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                    test_lead.next=next;
                    lead_search.seekp(last,std::ios::beg);
                    lead_search.write(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                }
                else{
                    block<_key_type, _subkey_type, _value_type> test_block;
                    index_.clear();
                    index_.seekg(all_loc,std::ios::beg);
                    index_.read(reinterpret_cast<char*>(&test_block),sizeof(block<_key_type, _subkey_type, _value_type>));
                    for(int j = 1; j < block_size; j++) test_block.element_array[j-1]=test_block.element_array[j];
                    index_.clear();
                    index_.seekp(all_loc,std::ios::beg);
                    index_.write(reinterpret_cast<char*>(&test_block),sizeof(block<_key_type, _subkey_type, _value_type>));
                    test_lead.this_element=test_block.element_array[0];
                    test_lead.block_size--;
                    lead_search.seekp(lead_loc,std::ios::beg);
                    lead_search.write(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
                }
                index_.close();
                lead_search.close();
                return true;
            }
        }
        index_.close();
        lead_search.close();
        return false;
    }

    void Clear(){

    }

    bool Exist(const _key_type& key){
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        element<_key_type, _subkey_type, _value_type> new_ele;
        new_ele.key=key;
        int lead_count;
        lead_search.seekg(sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_count),sizeof(int));
        long long int loc;
        lead_search.seekg(2*sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&loc),sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        for(int i=0;i<lead_count;++i){//search the lead
            lead_search.seekg(loc,std::ios::beg);
            lead_search.read(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
            if(new_ele.key==test_lead.this_element.key){lead_search.close();index_.close();return true;}
            else if(new_ele.key<test_lead.this_element.key||(new_ele.key>test_lead.this_element.key&&i==lead_count-1)) {
                if (i == 0 && new_ele.key < test_lead.this_element.key) {
                    lead_search.close();
                    index_.close();
                    return false;
                }
                else {
                    if (new_ele.key < test_lead.this_element.key) {
                        loc = test_lead.last;
                        lead_search.seekg(loc, std::ios::beg);
                        lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                    }
                    const long long int all_loc = test_lead.location;
                    const int block_size = test_lead.block_size;
                    block<_key_type, _subkey_type, _value_type> test_block;
                    index_.clear();
                    index_.seekg(all_loc);
                    index_.read(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                    int element_cmp=0;
                    if (block_size == 1) {
                        index_.close();
                        lead_search.close();
                        element_cmp= Element_cmp(test_block.element_array[0],new_ele);
                        if (element_cmp==0) return true;
                        else return false;
                    }
                    int h = 0, t = block_size - 1, j = 0;
                    while (h < t) {
                        j = (h + t + 1) / 2;
                        if (new_ele.key<test_block.element_array[j].key) t = j - 1;
                        else h = j;
                    }
                    j = (h + t) / 2;
                    index_.close();
                    lead_search.close();
                    element_cmp= Element_cmp(test_block.element_array[j],new_ele);
                    if (element_cmp==0) return true;
                    else return false;
                }
            }
            else{
                loc=test_lead.next;
                continue;
            }
        }
        lead_search.close();
        index_.close();
        return false;
    }

    bool Exist(const _key_type& key, const _subkey_type& subkey){
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        element<_key_type, _subkey_type, _value_type> new_ele;
        new_ele.subkey=subkey;
        new_ele.key=key;
        int lead_count;
        lead_search.seekg(sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_count),sizeof(int));
        long long int loc;
        lead_search.seekg(2*sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&loc),sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        for(int i=0;i<lead_count;++i){//search the lead
            lead_search.seekg(loc,std::ios::beg);
            lead_search.read(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
            int cmp=Element_Lead_cmp(new_ele,test_lead);
            if(cmp==0){lead_search.close();index_.close();return true;}
            else if(cmp<0||(cmp>0&&i==lead_count-1)) {
                if (i == 0 && cmp < 0) {
                    lead_search.close();
                    index_.close();
                    return false;
                }
                else {
                    if (cmp < 0) {
                        loc = test_lead.last;
                        lead_search.seekg(loc, std::ios::beg);
                        lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                    }
                    const long long int all_loc = test_lead.location;
                    const int block_size = test_lead.block_size;
                    block<_key_type, _subkey_type, _value_type> test_block;
                    index_.clear();
                    index_.seekg(all_loc);
                    index_.read(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                    element<_key_type, _subkey_type, _value_type> test_element;
                    if (block_size == 1) {
                        test_element=test_block.element_array[0];
                        index_.close();
                        lead_search.close();
                        if (test_element!=new_ele) return false;
                        else return true;
                    }
                    int h = 0, t = block_size - 1, j = 0;
                    while (h < t) {
                        j = (h + t + 1) / 2;
                        if (new_ele<test_block.element_array[j]) t = j - 1;
                        else h = j;
                    }
                    j = (h + t) / 2;
                    index_.close();
                    lead_search.close();
                    if (test_block.element_array[j] == new_ele) return true;
                    else return false;
                }
            }
            else{
                loc=test_lead.next;
                continue;
            }
        }
        lead_search.close();
        index_.close();
        return false;
    }

    std::vector<_value_type> Traverse(){
        std::vector<_value_type> v_real;
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        int lead_count;
        lead_search.seekg(sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_count),sizeof(int));
        long long int lead_loc;
        lead_search.seekg(2*sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_loc),sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        block<_key_type, _subkey_type, _value_type> test_block;
        for(int i=0;i<lead_count;++i){
            lead_search.seekg(lead_loc);
            lead_search.read(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
            int block_size=test_lead.block_size;
            long long int location=test_lead.location;
            index_.seekg(location,std::ios::beg);
            index_.read(reinterpret_cast<char*>(&test_block),sizeof(block<_key_type, _subkey_type, _value_type>));
            if(i==0||i==lead_count-1) for(int j=1;j<block_size;++j) v_real.push_back(test_block.element_array[j].value);
            else for(int j=0;j<block_size;++j) v_real.push_back(test_block.element_array[j].value);
            lead_loc=test_lead.next;
        }
        index_.close();
        lead_search.close();
        return v_real;
    } // 此函数返回一个遍历所有值的有序数组，请用 new 新建 std::vector，并且 std::vector::reserve() 足够多的空间减少浪费，调用该函数需要最后 delete 此指针（使用指针是为了减少不必要的复制）

    std::vector<_value_type> Traverse(const _key_type& key){
        std::vector<_value_type> v_real;
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        int lead_count;
        lead_search.seekg(sizeof(int),std::ios::beg);
        lead_search.read(reinterpret_cast<char*>(&lead_count),sizeof(int));
        long long int loc;
        lead_search.seekg(2*sizeof(int),std::ios::beg);
        lead_search.read(reinterpret_cast<char*>(&loc),sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        for(int i=0;i<lead_count;++i){
            lead_search.seekg(loc,std::ios::beg);
            lead_search.read(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
            if(test_lead.this_element.key<=key) {
                long long int loc_all = test_lead.location;
                int block_size = test_lead.block_size;
                block<_key_type, _subkey_type, _value_type> test_block;
                index_.clear();
                index_.seekg(loc_all);
                index_.read(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                for (int j = 0; j < block_size; ++j) {
                    if (test_block.element_array[j].key == key) v_real.push_back(test_block.element_array[j].value);
                }
                loc = test_lead.next;
                continue;
            }
            else break;
        }
        index_.close();
        lead_search.close();
        return v_real;
    } // 此函数返回一个遍历所有值的有序数组，请用 new 新建 std::vector，并且 std::vector::reserve() 足够多的空间减少浪费，调用该函数需要最后 delete 此指针（使用指针是为了减少不必要的复制）

    [[nodiscard]] _value_type Get(const _key_type& key, const _subkey_type& subkey){
        lead_search.clear();
        lead_search.open(file_name_lead);
        index_.clear();
        index_.open(file_name_all);
        element<_key_type, _subkey_type, _value_type> new_ele;
        new_ele.subkey=subkey;
        new_ele.key=key;
        int lead_count;
        lead_search.seekg(sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&lead_count),sizeof(int));
        long long int loc;
        lead_search.seekg(2*sizeof(int));
        lead_search.read(reinterpret_cast<char*>(&loc),sizeof(long long int));
        lead<_key_type, _subkey_type, _value_type> test_lead;
        for(int i=0;i<lead_count;++i){//search the lead
            lead_search.seekg(loc,std::ios::beg);
            lead_search.read(reinterpret_cast<char*>(&test_lead),sizeof(lead<_key_type, _subkey_type, _value_type>));
            int cmp=Element_Lead_cmp(new_ele,test_lead);
            if(cmp==0){lead_search.close();index_.close();return test_lead.this_element.value;}
            else if(cmp<0||(cmp>0&&i==lead_count-1)) {
                if (i == 0 && cmp < 0) {
                    lead_search.close();
                    index_.close();
                    throw 1;
                }
                else {
                    if (cmp < 0) {
                        loc = test_lead.last;
                        lead_search.seekg(loc, std::ios::beg);
                        lead_search.read(reinterpret_cast<char *>(&test_lead), sizeof(lead<_key_type, _subkey_type, _value_type>));
                    }
                    const long long int all_loc = test_lead.location;
                    const int block_size = test_lead.block_size;
                    block<_key_type, _subkey_type, _value_type> test_block;
                    index_.clear();
                    index_.seekg(all_loc);
                    index_.read(reinterpret_cast<char *>(&test_block), sizeof(block<_key_type, _subkey_type, _value_type>));
                    element<_key_type, _subkey_type, _value_type> test_element;
                    if (block_size == 1) {
                        test_element=test_block.element_array[0];
                        index_.close();
                        lead_search.close();
                        if (test_element!=new_ele) throw 1;
                        else return test_element.value;
                    }
                    int h = 0, t = block_size - 1, j = 0;
                    while (h < t) {
                        j = (h + t + 1) / 2;
                        if (new_ele<test_block.element_array[j]) t = j - 1;
                        else h = j;
                    }
                    j = (h + t) / 2;
                    index_.close();
                    lead_search.close();
                    if (test_block.element_array[j] == new_ele) return test_block.element_array[j].value;
                    else throw 1;
                }
            }
            else{
                loc=test_lead.next;
                continue;
            }
        }
        lead_search.close();
        index_.close();
        throw 1;
    }

};