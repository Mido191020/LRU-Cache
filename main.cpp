#include <iostream>
#include <unordered_map>

using namespace std;
template <typename T>
struct node{
T key;
T data;
node*next;
node*prev;
node(T d):
    data(d),
    next(NULL),
    prev(NULL)
    {}
};
template <typename T>
class linked_list{
private:
node<T>*head;
node<T>*tail;
public:
    void insert(T data){
        if (head== nullptr){
            node<T>*newNode=new node(data);
            newNode->next= nullptr;
            newNode->prev= nullptr;
            head=newNode;
            tail=newNode;
            return;
        } else{
            node<T>*newNode=new node(data);
            node<T>*temp=head;
            temp->prev=newNode;
            newNode->next=temp;
            newNode->prev= nullptr;
            head=newNode;
        }
        node<T>*temp=head;
        while (temp!= nullptr){
            if (temp->next== nullptr){
                tail=temp;
            }
            temp=temp->next;
        }
    }
    node<T>* find(T k){
        node<T>*temp=head;
        while (temp!= nullptr){
            if (temp->key==k){
                return temp;
            }
            temp=temp->next;
        }
        return nullptr;
    }
    void remove(node<T>temp){}
    void moveToFront(){
        //TODO:
    }
    void removeLast(){
        //TODO:
    }
};

int main() {

    return 0;
}