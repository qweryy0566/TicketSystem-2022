#ifndef TICKETSYSTEM_BPLUSTREE_HPP_
#define TICKETSYSTEM_BPLUSTREE_HPP_

#include <fstream>
#include <limits>
#include <iostream>

#include "vector.hpp"
using fqj::vector;

static const int Capacity=300;
template<typename _key_type, typename _subkey_type, typename _value_type>
struct Element{
    _key_type key;
    _subkey_type subkey;
    _value_type value;
    long long int son_pos=0;
    friend bool operator<(const Element &x,const Element &y){if(x.key!=y.key) return (x.key<y.key);else return (x.subkey<y.subkey);}
    friend bool operator==(const Element &x,const Element &y){return (x.key==y.key&&x.subkey==y.subkey);}
    friend bool operator>(const Element &x,const Element &y){if(x.key!=y.key) return (x.key>y.key);else return (x.subkey>y.subkey);}
    friend bool operator<=(const Element &x,const Element &y){return (x<y||x==y);}
    friend bool operator>=(const Element &x,const Element &y){return (x>y||x==y);}
    friend bool operator!=(const Element &x,const Element &y){return !(x.key==y.key&&x.subkey==y.subkey);}
    Element<_key_type, _subkey_type, _value_type>& operator=(const Element<_key_type, _subkey_type, _value_type> &x){
        this->key=x.key;// need operator "="
        this->subkey=x.subkey;// need operator "="
        this->value=x.value;// need operator "="
        this->son_pos=x.son_pos;
        return *this;
    };
};

template<typename _key_type, typename _subkey_type, typename _value_type>
int ElementCmp(const Element<_key_type, _subkey_type, _value_type> &x,const Element<_key_type, _subkey_type, _value_type> &y){
    if(x.key<y.key) return -1;
    else if(x.key>y.key) return 1;
    else return 0;
}

template<typename _key_type, typename _subkey_type, typename _value_type>
struct Node{
    int node_size=0;
    Element<_key_type, _subkey_type, _value_type> array_element[Capacity+5];
    long long int last=0;
    long long int next=0;
    int tag=0;//if tag==1, it points to leaves(block) 
};

template <typename _key_type,typename _subkey_type,typename _value_type>
class BPlusTree{
private:
    std::fstream file_content_;
    std::fstream file_deleted_;
    
public:
    std::string filename_content;
    std::string filename_deleted;

    long long int GetValidPosition(){
        long long int ans=0;
        file_deleted_.open(filename_deleted);
        int count_deleted=0;
        file_deleted_.seekg(sizeof(int));
        file_deleted_.read(reinterpret_cast<char*>(&count_deleted),sizeof(int));
        int count_node=0;
        file_deleted_.seekg(0);
        file_deleted_.read(reinterpret_cast<char*>(&count_node),sizeof(int));
        if(!count_deleted){
            ans=sizeof(int)+sizeof(long long int)+(long long)count_node*sizeof(Node<_key_type, _subkey_type, _value_type>);
        }
        else{
            file_deleted_.seekg(2*sizeof(int)+(long long)(count_deleted-1)*sizeof(long long int));
            file_deleted_.read(reinterpret_cast<char*>(&ans),sizeof(long long int));
            count_deleted--;
            file_deleted_.seekp(sizeof(int));
            file_deleted_.write(reinterpret_cast<char*>(&count_deleted),sizeof(int));
        }
        count_node++;
        file_deleted_.seekp(0);
        file_deleted_.write(reinterpret_cast<char*>(&count_node),sizeof(int));
        file_deleted_.close();
        return ans;
    }

    void PushDeleted(long long int deleted_pos){
        file_deleted_.open(filename_deleted);
        int count_deleted=0;
        file_deleted_.seekg(sizeof(int));
        file_deleted_.read(reinterpret_cast<char*>(&count_deleted),sizeof(int));
        file_deleted_.seekp(2*sizeof(int)+count_deleted*sizeof(long long int));
        file_deleted_.write(reinterpret_cast<char*>(&deleted_pos),sizeof(long long int));
        count_deleted++;
        file_deleted_.seekp(sizeof(int));
        file_deleted_.write(reinterpret_cast<char*>(&count_deleted),sizeof(int));
        int count_node=0;
        file_deleted_.seekg(0);
        file_deleted_.read(reinterpret_cast<char*>(&count_node),sizeof(int));
        count_node--;
        file_deleted_.seekp(0);
        file_deleted_.write(reinterpret_cast<char*>(&count_node),sizeof(int));
        file_deleted_.close();
    }
    
    BPlusTree(std::string name1,std::string name2):filename_content(name1),filename_deleted(name2){
        file_content_.open(filename_content);
        if(!file_content_){
            file_content_.open(filename_content,std::ofstream::out);
            file_content_.close();
            file_content_.open(filename_content);
            int count_all=0;
            long long int root_pos=sizeof(int)+sizeof(long long int);
            file_content_.seekp(0);
            file_content_.write(reinterpret_cast<char *>(&count_all), sizeof(int));
            file_content_.seekp(sizeof(int));
            file_content_.write(reinterpret_cast<char*>(&root_pos),sizeof(long long int));
        }
        file_content_.close();

        file_deleted_.open(filename_deleted);
        if(!file_deleted_){
            file_deleted_.open(filename_deleted,std::ofstream::out);
            file_deleted_.close();
            file_deleted_.open(filename_deleted);
            int count_node=0;
            file_deleted_.seekp(0);
            file_deleted_.write(reinterpret_cast<char*>(&count_node),sizeof(int));
            int count_deleted=0;
            file_deleted_.seekp(sizeof(int));
            file_deleted_.write(reinterpret_cast<char*>(&count_deleted),sizeof(int));
        }
        file_deleted_.close();
    };

    int Size(){
        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char*>(&count_all),sizeof(int));
        file_content_.close();
        return count_all;
    }

    void Initialize(){
        file_content_.open(filename_content);
        int count_all=0;
        long long int root_pos=sizeof(int)+sizeof(long long int);
        file_content_.seekp(0);
        file_content_.write(reinterpret_cast<char *>(&count_all), sizeof(int));
        file_content_.seekp(sizeof(int));
        file_content_.write(reinterpret_cast<char*>(&root_pos),sizeof(long long int));
        file_content_.close();

        file_deleted_.open(filename_deleted);
        int count_node=0;
        file_deleted_.seekp(0);
        file_deleted_.write(reinterpret_cast<char*>(&count_node),sizeof(int));
        int count_deleted=0;
        file_deleted_.seekp(sizeof(int));
        file_deleted_.write(reinterpret_cast<char*>(&count_deleted),sizeof(int));
        file_deleted_.close();
    }

    bool Insert(const _key_type& key, const _subkey_type& subkey, const _value_type& value) {
        Element<_key_type, _subkey_type, _value_type> new_element;
        new_element.key=key;
        new_element.subkey=subkey;
        new_element.value=value;

        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char *>(&count_all),sizeof(int));
        if(!count_all){
            Node<_key_type, _subkey_type, _value_type> new_block;
            new_block.array_element[0]=new_element;
            new_block.node_size=1;
            new_block.next=new_block.last=0;
            long long int pos_new_block= GetValidPosition();
            file_content_.seekp(pos_new_block);
            file_content_.write(reinterpret_cast<char*>(&new_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
            new_element.son_pos=pos_new_block;
            Node<_key_type, _subkey_type, _value_type> new_node;
            new_node.array_element[0]=new_element;
            new_node.node_size=1;
            new_node.tag=1;
            long long int pos_new_node= GetValidPosition();
            file_content_.seekp(pos_new_node);
            file_content_.write(reinterpret_cast<char*>(&new_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
            count_all=1;
            file_content_.seekp(0);
            file_content_.write(reinterpret_cast<char*>(&count_all),sizeof(int));
            file_content_.seekp(sizeof(int));
            file_content_.write(reinterpret_cast<char*>(&pos_new_node),sizeof(long long int));
            file_content_.close();

            return true;
        }
        else{
            long long int pos=0;
            file_content_.seekg(sizeof(int));
            file_content_.read(reinterpret_cast<char*>(&pos),sizeof(long long int));
            Node<_key_type, _subkey_type, _value_type> test_node;
            long long int stack_pos[25];
            int stack_ind[25];
            int top=-1;
            while(true) {
                stack_pos[++top]=pos;
                file_content_.seekg(pos);
                file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
                int h = 0, t = test_node.node_size - 1, mid, ind=-1;
                while (h <= t) {// find the first one > targeted
                    mid = h + ((t - h) >> 1);
                    if (new_element < test_node.array_element[mid]) {
                        ind = mid;
                        t = mid - 1;
                    } else h = mid + 1;
                }
                if(ind==-1){//new lead >= all of them
                    ind=test_node.node_size-1;
                    pos=test_node.array_element[ind].son_pos;
                }
                else{
                    if(ind){
                        ind--;
                        pos=test_node.array_element[ind].son_pos;
                    }
                    else pos=test_node.array_element[0].son_pos;
                }
                stack_ind[top]=ind;
                if(test_node.tag==1) break;//now pos points to a block in file_content_
                //all "pos" in stack_ point to nodes in file_content_
                //and the ind points to where to get son_pos
            }
            Node<_key_type, _subkey_type, _value_type> test_block;
            file_content_.seekg(pos);
            file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
            int h=0,t=test_block.node_size-1,mid,ind=-1;
            while(h<=t) {//find the first one >= new_element
                mid = h + ((t - h) >> 1);
                if (new_element <= test_block.array_element[mid]) {
                    ind = mid;
                    t = mid - 1;
                } else h = mid + 1;
            }
            if(ind!=-1&&new_element==test_block.array_element[ind]){//new one has been in the block
                //test_block.array_element[ind]=new_element;
                file_content_.seekp(pos);
                file_content_.write(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                file_content_.close();

                return false;
            }
            else{
                if(ind==-1){//all elements in the block < the targeted
                    ind=test_block.node_size;
                    test_block.array_element[ind]=new_element;
                    test_block.node_size++;
                    if(test_block.node_size<=Capacity){
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        count_all++;
                        file_content_.seekp(0);
                        file_content_.write(reinterpret_cast<char*>(&count_all),sizeof(int));
                        file_content_.close();

                        return true;
                    }
                }
                else{//ind now points to the first element > targeted element
                    for(int i=test_block.node_size-1;i>=ind;i--){
                        test_block.array_element[i+1]=test_block.array_element[i];
                    }
                    test_block.array_element[ind]=new_element;
                    test_block.node_size++;
                    if(!ind){//lead must be updated
                        Node<_key_type, _subkey_type, _value_type> update_node;
                        for(int i=top;i>=0;i--){
                            file_content_.seekg(stack_pos[i]);
                            file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            update_node.array_element[stack_ind[i]].key=test_block.array_element[ind].key;
                            update_node.array_element[stack_ind[i]].subkey=test_block.array_element[ind].subkey;
                            file_content_.seekp(stack_pos[i]);
                            file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            if(stack_ind[i]) break;//don't need to update more
                        }
                    }
                    if(test_block.node_size<=Capacity){
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        count_all++;
                        file_content_.seekp(0);
                        file_content_.write(reinterpret_cast<char*>(&count_all),sizeof(int));
                        file_content_.close();

                        return true;
                    }
                }
                long long int new_block_pos= GetValidPosition();
                Node<_key_type, _subkey_type, _value_type> new_block;
                mid=test_block.node_size>>1;//0 to mid-1 will be left in the test_block
                for(int i=mid;i<=test_block.node_size-1;i++){
                    new_block.array_element[i-mid]=test_block.array_element[i];
                }
                new_block.node_size=test_block.node_size-mid;
                test_block.node_size=mid;
                if(test_block.next){
                    Node<_key_type, _subkey_type, _value_type> next_block;
                    file_content_.seekg(test_block.next);
                    file_content_.read(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    next_block.last=new_block_pos;
                    file_content_.seekp(test_block.next);
                    file_content_.write(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                }
                new_block.next=test_block.next;
                new_block.last=pos;
                test_block.next=new_block_pos;
                file_content_.seekp(pos);
                file_content_.write(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                file_content_.seekp(new_block_pos);
                file_content_.write(reinterpret_cast<char*>(&new_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                Element<_key_type, _subkey_type, _value_type> new_lead;//new_lead leads the new_block
                new_lead.key=new_block.array_element[0].key;
                new_lead.subkey=new_block.array_element[0].subkey;
                new_lead.value=new_block.array_element[0].value;
                new_lead.son_pos=new_block_pos;
                //pos and new_block_pos
                //stack_pos and stack_ind
                //new_lead
                Node<_key_type, _subkey_type, _value_type> new_node;
                Element<_key_type, _subkey_type, _value_type> tmp_lead;
                while(top>=0){
                    file_content_.seekg(stack_pos[top]);
                    file_content_.read(reinterpret_cast<char*>(&test_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    //ind and ind + 1
                    for(int i=test_node.node_size-1;i>=stack_ind[top]+1;i--){
                        test_node.array_element[i+1]=test_node.array_element[i];
                    }
                    test_node.array_element[stack_ind[top]+1]=new_lead;//assignment is finished
                    test_node.node_size++;
                    if(test_node.node_size<=Capacity){
                        file_content_.seekp(stack_pos[top]);
                        file_content_.write(reinterpret_cast<char*>(&test_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        break;
                    }
                    mid=test_node.node_size>>1;
                    for(int i=mid;i<=test_node.node_size-1;i++){
                        new_node.array_element[i-mid]=test_node.array_element[i];
                    }
                    new_node.node_size=test_node.node_size-mid;
                    test_node.node_size=mid;
                    new_node.tag=test_node.tag;
                    file_content_.seekp(stack_pos[top]);
                    file_content_.write(reinterpret_cast<char*>(&test_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    tmp_lead.key=test_node.array_element[0].key;
                    tmp_lead.subkey=test_node.array_element[0].subkey;
                    tmp_lead.value=test_node.array_element[0].value;
                    tmp_lead.son_pos=stack_pos[top];
                    long long int new_node_pos= GetValidPosition();
                    new_lead.key=new_node.array_element[0].key;
                    new_lead.subkey=new_node.array_element[0].subkey;
                    new_lead.value=new_node.array_element[0].value;
                    new_lead.son_pos=new_node_pos;//new_lead leads the new_node
                    file_content_.seekp(new_node_pos);
                    file_content_.write(reinterpret_cast<char*>(&new_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    top--;
                }
                if(top==-1){//need a new root
                    long long int new_node_pos= GetValidPosition();
                    new_node.array_element[0]=tmp_lead;
                    new_node.array_element[1]=new_lead;
                    new_node.tag=0;
                    new_node.node_size=2;
                    file_content_.seekp(new_node_pos);
                    file_content_.write(reinterpret_cast<char*>(&new_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    file_content_.seekp(sizeof(int));
                    file_content_.write(reinterpret_cast<char*>(&new_node_pos),sizeof(long long int));//new root_pos
                }
                count_all++;
                file_content_.seekp(0);
                file_content_.write(reinterpret_cast<char*>(&count_all),sizeof(int));
                file_content_.close();

                return true;
            }
        }
    }

    bool Delete(const _key_type& key, const _subkey_type& subkey) {
        Element<_key_type, _subkey_type, _value_type> targeted_element;
        targeted_element.key = key;
        targeted_element.subkey = subkey;

        file_content_.open(filename_content);
        int count_all = 0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char *>(&count_all), sizeof(int));
        if (!count_all) {
            file_content_.close();

            return false;
        }
        long long int pos = 0;
        file_content_.seekg(sizeof(int));
        file_content_.read(reinterpret_cast<char *>(&pos), sizeof(long long int));
        Node<_key_type, _subkey_type, _value_type> test_node;
        int top = -1;
        long long int stack_pos[25];
        long long int stack_last_pos[25]={-1};
        long long int stack_next_pos[25]={-1};
        int stack_ind[25];
        while (true) {
            stack_pos[++top] = pos;
            file_content_.seekg(pos);
            file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
            int h = 0, t = test_node.node_size - 1, mid, ind = -1;
            while (h <= t) {// find the first one > targeted_lead
                mid = h + ((t - h) >> 1);
                if (targeted_element < test_node.array_element[mid]) {
                    ind = mid;
                    t = mid - 1;
                } else h = mid + 1;
            }
            if (ind == -1) {//targeted_lead >= all of them
                ind = test_node.node_size - 1;
                pos = test_node.array_element[ind].son_pos;

                stack_next_pos[top+1]=-1;
                if(ind) stack_last_pos[top+1]=test_node.array_element[ind-1].son_pos;
                else stack_last_pos[top+1]=-1;
            }
            else {
                if(ind){
                    ind--;
                    pos = test_node.array_element[ind].son_pos;

                    stack_next_pos[top+1]=test_node.array_element[ind+1].son_pos;
                    if(ind) stack_last_pos[top+1]=test_node.array_element[ind-1].son_pos;
                    else stack_last_pos[top+1]=-1;
                }
                else {//ind == 0, target_lead < all of them
                    pos = test_node.array_element[0].son_pos;
                    file_content_.close();

                    return false;
                }
            }
            stack_ind[top] = ind;
            if (test_node.tag == 1) break;//now pos points to a block in file_content_
            //all "pos" in stack_ point to nodes in file_content_
            //and the ind points to where to get son_pos
        }
        Node<_key_type, _subkey_type, _value_type> test_block;
        file_content_.seekg(pos);
        file_content_.read(reinterpret_cast<char *>(&test_block), sizeof(Node<_key_type, _subkey_type, _value_type>));
        int h = 0, t = test_block.node_size - 1, mid, ind = -1;
        while (h <= t) {// find the first one >= targeted_element
            mid = h + ((t - h) >> 1);
            if (targeted_element <= test_block.array_element[mid]) {
                ind = mid;
                t = mid - 1;
            } else h = mid + 1;
        }
        if (ind == -1 || targeted_element != test_block.array_element[ind]) {//targeted_element cannot be found
            file_content_.close();

            return false;
        }
        for(int i=ind+1;i<=test_block.node_size-1;i++){//ind will be deleted
            test_block.array_element[i-1]=test_block.array_element[i];
        }
        test_block.node_size--;
        if(!ind){//lead must be updated
            Node<_key_type, _subkey_type, _value_type> update_node;
            for(int i=top;i>=0;i--){
                file_content_.seekg(stack_pos[i]);
                file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                update_node.array_element[stack_ind[i]].key=test_block.array_element[0].key;
                update_node.array_element[stack_ind[i]].subkey=test_block.array_element[0].subkey;
                file_content_.seekp(stack_pos[i]);
                file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                if(stack_ind[i]) break;
            }
        }
        if (test_block.node_size >= Capacity >> 1) {
            file_content_.seekp(pos);
            file_content_.write(reinterpret_cast<char *>(&test_block), sizeof(Node<_key_type, _subkey_type, _value_type>));
        }
        else{//test_block.node_size < Capacity >> 1
            Node<_key_type, _subkey_type, _value_type> adjacent_block;
            if(top==0){//merge happens in the root node
                if(stack_next_pos[top+1]!=-1){
                    file_content_.seekg(test_block.next);
                    file_content_.read(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    if(adjacent_block.node_size > Capacity >> 1){//borrow its son
                        int k=test_block.node_size;
                        test_block.array_element[k]=adjacent_block.array_element[0];
                        test_block.node_size++;
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char *>(&test_block), sizeof(Node<_key_type, _subkey_type, _value_type>));
                        for(int i=1;i<=adjacent_block.node_size-1;i++){
                            adjacent_block.array_element[i-1]=adjacent_block.array_element[i];
                        }
                        adjacent_block.node_size--;
                        file_content_.seekp(test_block.next);
                        file_content_.write(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        Node<_key_type, _subkey_type, _value_type> update_node;
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        update_node.array_element[stack_ind[top]+1].key=adjacent_block.array_element[0].key;
                        update_node.array_element[stack_ind[top]+1].subkey=adjacent_block.array_element[0].subkey;
                        file_content_.seekp(stack_pos[top]);
                        file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    }
                    else{//the two one: test_block and adjacent_block need to be merged
                        PushDeleted(test_block.next);
                        test_block.next=adjacent_block.next;
                        if(test_block.next){
                            Node<_key_type, _subkey_type, _value_type> next_block;
                            file_content_.seekg(test_block.next);
                            file_content_.read(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            next_block.last=pos;
                            file_content_.seekp(test_block.next);
                            file_content_.write(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        }
                        int k=test_block.node_size;
                        for(int i=0;i<=adjacent_block.node_size-1;i++){
                            test_block.array_element[k+i]=adjacent_block.array_element[i];
                        }
                        test_block.node_size+=adjacent_block.node_size;
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char *>(&test_block), sizeof(Node<_key_type, _subkey_type, _value_type>));
                        Node<_key_type, _subkey_type, _value_type> update_node;
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        for(int i=stack_ind[top]+2;i<=update_node.node_size-1;i++){
                            update_node.array_element[i-1]=update_node.array_element[i];
                        }
                        update_node.node_size--;
                        file_content_.seekp(stack_pos[top]);
                        file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    }
                }
                else if(stack_last_pos[top+1]!=-1){
                    file_content_.seekg(test_block.last);
                    file_content_.read(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    if(adjacent_block.node_size > Capacity >> 1){//borrow its son
                        for(int i=test_block.node_size-1;i>=0;i--){
                            test_block.array_element[i+1]=test_block.array_element[i];
                        }
                        int k=adjacent_block.node_size-1;
                        test_block.array_element[0]=adjacent_block.array_element[k];
                        test_block.node_size++;
                        adjacent_block.node_size--;
                        file_content_.seekp(test_block.last);
                        file_content_.write(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char *>(&test_block), sizeof(Node<_key_type, _subkey_type, _value_type>));
                        Node<_key_type, _subkey_type, _value_type> update_node;
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        update_node.array_element[stack_ind[top]].key=test_block.array_element[0].key;
                        update_node.array_element[stack_ind[top]].subkey=test_block.array_element[0].subkey;
                        file_content_.seekp(stack_pos[top]);
                        file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    }
                    else{//the two one: adjacent_block and test_block need to be merged
                        PushDeleted(pos);
                        int k=adjacent_block.node_size;
                        for(int i=0;i<=test_block.node_size-1;i++){
                            adjacent_block.array_element[k+i]=test_block.array_element[i];
                        }
                        adjacent_block.node_size+=test_block.node_size;
                        adjacent_block.next=test_block.next;
                        if(adjacent_block.next){
                            Node<_key_type, _subkey_type, _value_type> next_block;
                            file_content_.seekg(adjacent_block.next);
                            file_content_.read(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            next_block.last=test_block.last;
                            file_content_.seekp(adjacent_block.next);
                            file_content_.write(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        }
                        file_content_.seekp(test_block.last);
                        file_content_.write(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        Node<_key_type, _subkey_type, _value_type> update_node;
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        for(int i=stack_ind[top]+1;i<=update_node.node_size-1;i++){
                            update_node.array_element[i-1]=update_node.array_element[i];
                        }
                        update_node.node_size--;
                        file_content_.seekp(stack_pos[top]);
                        file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    }
                }
                else{//do nothing
                    file_content_.seekp(pos);
                    file_content_.write(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                }
            }
            else{//not the root, so its size cannot be too small
                bool check_=false;
                int ind1=0,ind2=0;
                if(stack_next_pos[top+1]!=-1){
                    file_content_.seekg(test_block.next);
                    file_content_.read(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    if(adjacent_block.node_size > Capacity >> 1){//borrow its son
                        int k=test_block.node_size;
                        test_block.array_element[k]=adjacent_block.array_element[0];
                        test_block.node_size++;
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char *>(&test_block), sizeof(Node<_key_type, _subkey_type, _value_type>));
                        for(int i=1;i<=adjacent_block.node_size-1;i++){
                            adjacent_block.array_element[i-1]=adjacent_block.array_element[i];
                        }
                        adjacent_block.node_size--;
                        file_content_.seekp(test_block.next);
                        file_content_.write(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        Node<_key_type, _subkey_type, _value_type> update_node;
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        update_node.array_element[stack_ind[top]+1].key=adjacent_block.array_element[0].key;
                        update_node.array_element[stack_ind[top]+1].subkey=adjacent_block.array_element[0].subkey;
                        file_content_.seekp(stack_pos[top]);
                        file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    }
                    else{//the two one: test_block and adjacent_block need to be merged
                        PushDeleted(test_block.next);
                        test_block.next=adjacent_block.next;
                        if(test_block.next){
                            Node<_key_type, _subkey_type, _value_type> next_block;
                            file_content_.seekg(test_block.next);
                            file_content_.read(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            next_block.last=pos;
                            file_content_.seekp(test_block.next);
                            file_content_.write(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        }
                        int k=test_block.node_size;
                        for(int i=0;i<=adjacent_block.node_size-1;i++){
                            test_block.array_element[k+i]=adjacent_block.array_element[i];
                        }
                        test_block.node_size+=adjacent_block.node_size;
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char *>(&test_block), sizeof(Node<_key_type, _subkey_type, _value_type>));
                        check_=true;
                        ind1=stack_ind[top];ind2=ind1+1;
                    }
                }
                else{
                    file_content_.seekg(test_block.last);
                    file_content_.read(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    if(adjacent_block.node_size > Capacity >> 1){//borrow its son
                        for(int i=test_block.node_size-1;i>=0;i--){
                            test_block.array_element[i+1]=test_block.array_element[i];
                        }
                        int k=adjacent_block.node_size-1;
                        test_block.array_element[0]=adjacent_block.array_element[k];
                        test_block.node_size++;
                        adjacent_block.node_size--;
                        file_content_.seekp(pos);
                        file_content_.write(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        file_content_.seekp(test_block.last);
                        file_content_.write(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        Node<_key_type, _subkey_type, _value_type> update_node;
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        update_node.array_element[stack_ind[top]].key=test_block.array_element[0].key;
                        update_node.array_element[stack_ind[top]].subkey=test_block.array_element[0].subkey;
                        file_content_.seekp(stack_pos[top]);
                        file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                    }
                    else{//adjacent_block and test_block wil be merged
                        PushDeleted(pos);
                        int k=adjacent_block.node_size;
                        for(int i=0;i<=test_block.node_size-1;i++){
                            adjacent_block.array_element[i+k]=test_block.array_element[i];
                        }
                        adjacent_block.node_size+=test_block.node_size;
                        adjacent_block.next=test_block.next;
                        if(adjacent_block.next){
                            Node<_key_type, _subkey_type, _value_type> next_block;
                            file_content_.seekg(adjacent_block.next);
                            file_content_.read(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            next_block.last=test_block.last;
                            file_content_.seekp(adjacent_block.next);
                            file_content_.write(reinterpret_cast<char*>(&next_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        }
                        file_content_.seekp(test_block.last);
                        file_content_.write(reinterpret_cast<char*>(&adjacent_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        check_=true;
                        ind1=stack_ind[top]-1;ind2=ind1+1;
                    }
                }
                if(check_){
                    Node<_key_type, _subkey_type, _value_type> update_node;
                    Node<_key_type, _subkey_type, _value_type> adjacent_node;
                    while(top>=1){
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        for(int i=ind2+1;i<=update_node.node_size-1;i++){
                            update_node.array_element[i-1]=update_node.array_element[i];
                        }
                        update_node.node_size--;
                        if(update_node.node_size>= Capacity>>1){
                            file_content_.seekp(stack_pos[top]);
                            file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            break;
                        }
                        if(stack_next_pos[top]!=-1){//has next_node
                            file_content_.seekg(stack_next_pos[top]);
                            file_content_.read(reinterpret_cast<char*>(&adjacent_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            if(adjacent_node.node_size > Capacity>>1){//borrow its son
                                int k=update_node.node_size;
                                update_node.array_element[k]=adjacent_node.array_element[0];
                                update_node.node_size++;
                                for(int i=1;i<=adjacent_node.node_size-1;i++){
                                    adjacent_node.array_element[i-1]=adjacent_node.array_element[i];
                                }
                                adjacent_node.node_size--;
                                file_content_.seekp(stack_next_pos[top]);
                                file_content_.write(reinterpret_cast<char*>(&adjacent_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                file_content_.seekp(stack_pos[top]);
                                file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                file_content_.seekg(stack_pos[top-1]);
                                file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                update_node.array_element[stack_ind[top-1]+1].key=adjacent_node.array_element[0].key;
                                update_node.array_element[stack_ind[top-1]+1].subkey=adjacent_node.array_element[0].subkey;
                                file_content_.seekp(stack_pos[top-1]);
                                file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                break;
                            }
                            else{//update_node and adjacent_node will be merged
                                PushDeleted(stack_next_pos[top]);
                                int k=update_node.node_size;
                                for(int i=0;i<=adjacent_node.node_size-1;i++){
                                    update_node.array_element[k+i]=adjacent_node.array_element[i];
                                }
                                update_node.node_size+=adjacent_node.node_size;
                                file_content_.seekp(stack_pos[top]);
                                file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                ind1=stack_ind[top-1];ind2=ind1+1;
                            }
                        }
                        else{
                            file_content_.seekg(stack_last_pos[top]);
                            file_content_.read(reinterpret_cast<char*>(&adjacent_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                            if(adjacent_node.node_size > Capacity>>1){//borrow its son
                                for(int i=update_node.node_size-1;i>=0;i--){
                                    update_node.array_element[i+1]=update_node.array_element[i];
                                }
                                int k=adjacent_node.node_size-1;
                                update_node.array_element[0]=adjacent_node.array_element[k];
                                update_node.node_size++;
                                adjacent_node.node_size--;
                                file_content_.seekp(stack_last_pos[top]);
                                file_content_.write(reinterpret_cast<char*>(&adjacent_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                file_content_.seekp(stack_pos[top]);
                                file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                file_content_.seekg(stack_pos[top-1]);
                                file_content_.read(reinterpret_cast<char*>(&adjacent_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                adjacent_node.array_element[stack_ind[top-1]].key=update_node.array_element[0].key;
                                adjacent_node.array_element[stack_ind[top-1]].subkey=update_node.array_element[0].subkey;
                                file_content_.seekp(stack_pos[top-1]);
                                file_content_.write(reinterpret_cast<char*>(&adjacent_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                break;
                            }
                            else{
                                PushDeleted(stack_pos[top]);
                                int k=adjacent_node.node_size;
                                for(int i=0;i<=update_node.node_size-1;i++){
                                    adjacent_node.array_element[k+i]=update_node.array_element[i];
                                }
                                adjacent_node.node_size+=update_node.node_size;
                                file_content_.seekp(stack_last_pos[top]);
                                file_content_.write(reinterpret_cast<char*>(&adjacent_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                ind1=stack_ind[top-1]-1;ind2=ind1+1;
                            }
                        }
                        top--;
                    }
                    if(top==0){
                        file_content_.seekg(stack_pos[top]);
                        file_content_.read(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        if(update_node.node_size>=3){
                            for(int i=ind2+1;i<=update_node.node_size-1;i++){
                                update_node.array_element[i-1]=update_node.array_element[i];
                            }
                            update_node.node_size--;
                            file_content_.seekp(stack_pos[top]);
                            file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                        }
                        else{//update_node.node_size <=2 and it will be <=1
                            if(update_node.tag==0){//merge and a new root
                                long long int new_root_pos=update_node.array_element[0].son_pos;
                                file_content_.seekp(sizeof(int));
                                file_content_.write(reinterpret_cast<char*>(&new_root_pos),sizeof(long long int));
                                PushDeleted(stack_pos[top]);
                            }
                            else{//the root
                                for(int i=ind2+1;i<=update_node.node_size-1;i++){
                                    update_node.array_element[i-1]=update_node.array_element[i];
                                }
                                update_node.node_size--;
                                if(update_node.node_size>0){
                                    file_content_.seekp(stack_pos[top]);
                                    file_content_.write(reinterpret_cast<char*>(&update_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                                }
                                else{
                                    Initialize();
                                }
                            }
                        }
                    }
                }
            }
        }
        count_all--;
        file_content_.seekp(0);
        file_content_.write(reinterpret_cast<char*>(&count_all),sizeof(int));
        file_content_.close();

        //if(!count_all) Initialize();
        return true;
    }

    bool Modify(const _key_type& key, const _subkey_type& subkey, const _value_type& value){
        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char*>(&count_all),sizeof(int));
        if(!count_all){
            file_content_.close();

            return false;
        }
        Element<_key_type, _subkey_type, _value_type> targeted_lead;
        targeted_lead.key=key;
        targeted_lead.subkey=subkey;
        long long int pos=0;
        file_content_.seekg(sizeof(int));
        file_content_.read(reinterpret_cast<char*>(&pos),sizeof(long long int));
        Node<_key_type, _subkey_type, _value_type> test_node;
        while(true) {
            file_content_.seekg(pos);
            file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
            int h = 0, t = test_node.node_size - 1, mid, ind=-1;
            while (h <= t) {// find the first one > targeted_lead
                mid = h + ((t - h) >> 1);
                if (targeted_lead < test_node.array_element[mid]) {
                    ind = mid;
                    t = mid - 1;
                } else h = mid + 1;
            }
            if(ind==-1){
                int k=test_node.node_size-1;
                pos=test_node.array_element[k].son_pos;
            }
            else{
                if(ind){
                    pos=test_node.array_element[ind-1].son_pos;
                }
                else{
                    pos=test_node.array_element[0].son_pos;
                }
            }
            if(test_node.tag==1) break;
        }
        Node<_key_type, _subkey_type, _value_type> test_block;
        file_content_.seekg(pos);
        file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
        Element<_key_type, _subkey_type, _value_type> targeted_element;
        targeted_element.key=key;
        targeted_element.subkey=subkey;
        int h=0,t=test_block.node_size-1,mid,ind=-1;
        while(h<=t){//find the first one >= targeted_element
            mid=h+((t-h)>>1);
            if(targeted_element<=test_block.array_element[mid]){
                ind=mid;
                t=mid-1;
            }
            else h=mid+1;
        }
        if(ind==-1||targeted_element!=test_block.array_element[ind]){
            file_content_.close();

            return false;
        }
        else{
            test_block.array_element[ind].value=value;
            file_content_.seekp(pos);
            file_content_.write(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
            file_content_.close();

            return true;
        }
    }

    void Clear(){
        Initialize();
    }

    bool Exist(const _key_type& key){
        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char*>(&count_all),sizeof(int));
        if(!count_all){
            file_content_.close();

            return false;
        }
        long long int pos=0;
        file_content_.seekg(sizeof(int));
        file_content_.read(reinterpret_cast<char*>(&pos),sizeof(long long int));
        Node<_key_type, _subkey_type, _value_type> test_node;
        while(true) {
            file_content_.seekg(pos);
            file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
            int h = 0, t = test_node.node_size - 1, mid, ind=-1;
            while (h <= t) {// find the first one > targeted_lead
                mid = h + ((t - h) >> 1);
                if (key < test_node.array_element[mid].key) {
                    ind = mid;
                    t = mid - 1;
                } else h = mid + 1;
            }
            if(ind==-1){
                int k=test_node.node_size-1;
                pos=test_node.array_element[k].son_pos;
            }
            else{
                if(ind){
                    pos=test_node.array_element[ind-1].son_pos;
                }
                else{
                    pos=test_node.array_element[0].son_pos;
                }
            }
            if(test_node.tag==1) break;
        }
        Node<_key_type, _subkey_type, _value_type> test_block;
        file_content_.seekg(pos);
        file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
        int h=0,t=test_block.node_size-1,mid,ind=-1;
        while(h<=t){//find the first one >= targeted_element
            mid=h+((t-h)>>1);
            if(key<=test_block.array_element[mid].key){
                ind=mid;
                t=mid-1;
            }
            else h=mid+1;
        }
        file_content_.close();
        if(ind==-1||key!=test_block.array_element[ind].key){
            return false;
        }
        else{
            return true;
        }
    }

    bool Exist(const _key_type& key, const _subkey_type& subkey){
        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char*>(&count_all),sizeof(int));
        if(!count_all){
            file_content_.close();

            return false;
        }
        Element<_key_type, _subkey_type, _value_type> targeted_lead;
        targeted_lead.key=key;
        targeted_lead.subkey=subkey;

        long long int pos=0;
        file_content_.seekg(sizeof(int));
        file_content_.read(reinterpret_cast<char*>(&pos),sizeof(long long int));
        Node<_key_type, _subkey_type, _value_type> test_node;
        while(true) {
            file_content_.seekg(pos);
            file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
            int h = 0, t = test_node.node_size - 1, mid, ind=-1;
            while (h <= t) {// find the first one > targeted_lead
                mid = h + ((t - h) >> 1);
                if (targeted_lead < test_node.array_element[mid]) {
                    ind = mid;
                    t = mid - 1;
                } else h = mid + 1;
            }
            if(ind==-1){
                int k=test_node.node_size-1;
                pos=test_node.array_element[k].son_pos;
            }
            else{
                if(ind){
                    pos=test_node.array_element[ind-1].son_pos;
                }
                else{
                    pos=test_node.array_element[0].son_pos;
                }
            }
            if(test_node.tag==1) break;
        }
        Node<_key_type, _subkey_type, _value_type> test_block;
        file_content_.seekg(pos);
        file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
        Element<_key_type, _subkey_type, _value_type> targeted_element;
        targeted_element.key=key;
        targeted_element.subkey=subkey;
        int h=0,t=test_block.node_size-1,mid,ind=-1;
        while(h<=t){//find the first one >= targeted_element
            mid=h+((t-h)>>1);
            if(targeted_element<=test_block.array_element[mid]){
                ind=mid;
                t=mid-1;
            }
            else h=mid+1;
        }
        file_content_.close();

        if(ind==-1||targeted_element!=test_block.array_element[ind]){
            return false;
        }
        else{
            return true;
        }
    }

    vector<_value_type> Traverse(){
        vector<_value_type> v_all;
        v_all.clear();
        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char*>(&count_all),sizeof(int));
        if(!count_all){
            file_content_.close();

            return v_all;
        }
        else{
            long long int pos=0;
            file_content_.seekg(sizeof(int));
            file_content_.read(reinterpret_cast<char*>(&pos),sizeof(long long int));
            Node<_key_type, _subkey_type, _value_type> test_node;
            Node<_key_type, _subkey_type, _value_type> test_block;
            while(true){
                file_content_.seekg(pos);
                file_content_.read(reinterpret_cast<char*>(&test_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                pos=test_node.array_element[0].son_pos;
                if(test_node.tag==1) break;
            }
            while(true){
                file_content_.seekg(pos);
                file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                for(int i=0;i<=test_block.node_size-1;i++){
                    v_all.push_back(test_block.array_element[i].value);
                }
                pos=test_block.next;
                if(!pos) break;
            }
            file_content_.close();

            return v_all;
        }
    }

    vector<_value_type> Traverse(const _key_type& key){
        vector<_value_type> v_all;
        v_all.clear();
        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char*>(&count_all),sizeof(int));
        file_content_.close();

        if(!count_all) return v_all;
        else{
            int h , t , mid, ind;
            file_content_.open(filename_content);
            Element<_key_type, _subkey_type, _value_type> min_lead;
            min_lead.key=key;
            min_lead.subkey=std::numeric_limits<_subkey_type>::min();
            long long int first_pos=0;
            file_content_.seekg(sizeof(int));
            file_content_.read(reinterpret_cast<char*>(&first_pos),sizeof(long long int));
            Node<_key_type, _subkey_type, _value_type> test_node;
            while(true) {
                file_content_.seekg(first_pos);
                file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
                h = 0, t = test_node.node_size - 1,ind=-1;
                while (h <= t) {// find the first one > targeted_lead
                    mid = h + ((t - h) >> 1);
                    if (min_lead < test_node.array_element[mid]) {
                        ind = mid;
                        t = mid - 1;
                    } else h = mid + 1;
                }
                if(ind==-1){
                    int k=test_node.node_size-1;
                    first_pos=test_node.array_element[k].son_pos;
                }
                else{
                    if(ind){
                        first_pos=test_node.array_element[ind-1].son_pos;
                    }
                    else{
                        first_pos=test_node.array_element[0].son_pos;
                    }
                }
                if(test_node.tag==1) break;
            }
            Node<_key_type, _subkey_type, _value_type> test_block;
            file_content_.seekg(first_pos);
            file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
            Element<_key_type, _subkey_type, _value_type> min_element;
            min_element.key=key;
            min_element.subkey=std::numeric_limits<_subkey_type>::min();
            int first_ind=-1;
            h=0;t=test_block.node_size-1;
            while(h<=t){//find the first one >= min_element
                mid=h+((t-h)>>1);
                if(min_element<=test_block.array_element[mid]){
                    first_ind=mid;
                    t=mid-1;
                }
                else h=mid+1;
            }
            file_content_.close();

            if(first_ind==-1){//all of them < the targeted
                first_ind=test_block.node_size;
            }
            file_content_.open(filename_content);
            Element<_key_type, _subkey_type, _value_type> max_lead;
            max_lead.key=key;
            max_lead.subkey=std::numeric_limits<_subkey_type>::max();
            long long int last_pos=0;
            file_content_.seekg(sizeof(int));
            file_content_.read(reinterpret_cast<char*>(&last_pos),sizeof(long long int));
            while(true) {
                file_content_.seekg(last_pos);
                file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
                h = 0, t = test_node.node_size - 1, ind=-1;
                while (h <= t) {// find the first one > targeted_lead
                    mid = h + ((t - h) >> 1);
                    if (max_lead < test_node.array_element[mid]) {
                        ind = mid;
                        t = mid - 1;
                    } else h = mid + 1;
                }
                if(ind==-1){
                    int k=test_node.node_size-1;
                    last_pos=test_node.array_element[k].son_pos;
                }
                else{
                    if(ind){
                        last_pos=test_node.array_element[ind-1].son_pos;
                    }
                    else{
                        last_pos=test_node.array_element[0].son_pos;
                    }
                }
                if(test_node.tag==1) break;
            }
            file_content_.seekg(last_pos);
            file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
            Element<_key_type, _subkey_type, _value_type> max_element;
            max_element.key=key;
            max_element.subkey=std::numeric_limits<_subkey_type>::max();
            h=0;t=test_block.node_size-1;
            int last_ind=-1;
            while(h<=t){//find the first one >= targeted_element
                mid=h+((t-h)>>1);
                if(max_element<=test_block.array_element[mid]){
                    last_ind=mid;
                    t=mid-1;
                }
                else h=mid+1;
            }
            if(last_ind==-1){//not found
                last_ind=test_block.node_size-1;
            }
            else if(test_block.array_element[last_ind].key>max_element.key){
                last_ind--;
            }
            long long int pos=first_pos;
            while(pos!=last_pos){
                file_content_.seekg(pos);
                file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
                for(int i=first_ind;i<=test_block.node_size-1;i++){
                    v_all.push_back(test_block.array_element[i].value);
                }
                first_ind=0;
                pos=test_block.next;
            }
            file_content_.seekg(pos);
            file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
            for(int i=first_ind;i<=last_ind;i++){
                v_all.push_back(test_block.array_element[i].value);
            }
            file_content_.close();

            return v_all;
        }
    }

    // qweryy : Get 是不是没有测试过。
    [[nodiscard]] _value_type Get(const _key_type& key, const _subkey_type& subkey){
        file_content_.open(filename_content);
        int count_all=0;
        file_content_.seekg(0);
        file_content_.read(reinterpret_cast<char*>(&count_all),sizeof(int));
        if(!count_all){
            file_content_.close();

            return _value_type();
        }
        // qweryy : Node -> Element
        Element<_key_type, _subkey_type, _value_type> targeted_lead;
        targeted_lead.key=key;
        targeted_lead.subkey=subkey;
        long long int pos=0;
        file_content_.seekg(sizeof(int));
        file_content_.read(reinterpret_cast<char*>(&pos),sizeof(long long int));
        Node<_key_type, _subkey_type, _value_type> test_node;
        while(true) {
            file_content_.seekg(pos);
            file_content_.read(reinterpret_cast<char *>(&test_node), sizeof(Node<_key_type, _subkey_type, _value_type>));
            int h = 0, t = test_node.node_size - 1, mid, ind=-1;
            while (h <= t) {// find the first one > targeted_lead
                mid = h + ((t - h) >> 1);
                if (targeted_lead < test_node.array_element[mid]) {
                    ind = mid;
                    t = mid - 1;
                } else h = mid + 1;
            }
            if(ind==-1){
                int k=test_node.node_size-1;
                pos=test_node.array_element[k].son_pos;
            }
            else{
                if(ind){
                    pos=test_node.array_element[ind-1].son_pos;
                }
                else{
                    pos=test_node.array_element[0].son_pos;
                }
            }
            if(test_node.tag==1) break;
        }
        Node<_key_type, _subkey_type, _value_type> test_block;
        file_content_.seekg(pos);
        file_content_.read(reinterpret_cast<char*>(&test_block),sizeof(Node<_key_type, _subkey_type, _value_type>));
        Element<_key_type, _subkey_type, _value_type> targeted_element;
        targeted_element.key=key;
        targeted_element.subkey=subkey;
        int h=0,t=test_block.node_size-1,mid,ind=-1;
        while(h<=t){//find the first one >= targeted_element
            mid=h+((t-h)>>1);
            if(targeted_element<=test_block.array_element[mid]){
                ind=mid;
                t=mid-1;
            }
            else h=mid+1;
        }
        file_content_.close();

        if(ind==-1||targeted_element!=test_block.array_element[ind]){
            return _value_type();
        }
        else{
            return test_block.array_element[ind].value;
        }
    }

    void CheckPrintAll(){
        file_content_.open(filename_content);
        long long int pos=0;
        file_content_.seekg(sizeof(int));
        file_content_.read(reinterpret_cast<char*>(&pos),sizeof(long long int));
        int top=-1;
        long long int stack_pos[10000];
        Node<_key_type, _subkey_type, _value_type> test_node;
        stack_pos[++top]=pos;
        long long int stack_pos_tmp[10000];
        int top_tmp=-1;
        while(true){
            while(top>=0){
                file_content_.seekg(stack_pos[top]);
                file_content_.read(reinterpret_cast<char*>(&test_node),sizeof(Node<_key_type, _subkey_type, _value_type>));
                for(int i=0;i<=test_node.node_size-1;i++){
                    std::cout << test_node.array_element[i].key << "-" << test_node.array_element[i].subkey << " ";
                    stack_pos_tmp[++top_tmp]=test_node.array_element[i].son_pos;
                }
                std::cout << "|| ";
                top--;
            }
            std::cout << std::endl;
            if(test_node.tag==1) break;
            while(top_tmp>=0){
                stack_pos[++top]=stack_pos_tmp[top_tmp--];
            }
        }
        file_content_.close();
    }
};

#endif  // TICKETSYSTEM_BPLUSTREE_HPP_