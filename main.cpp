#include <iostream>
#include <unordered_map>

using namespace std;
template <typename T>
struct node{
T key;
T value;
node*next;
node*prev;
node(T k,T v):

        key(k),
        value(v),
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
    linked_list(){
        head= nullptr;
        tail= nullptr;
    }
    void insert(T key,T value){
        node<T>* newNode=new node(key,value);
        if (head== nullptr){
            head=newNode;
            tail=newNode;
            return;
        }
        newNode->next=head;
        head->prev=newNode;
        head=newNode;

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

   int remove(T key){
        //to remove node from a list
        //find the node
        //change the conections of it
        //if tail we need to make new tail
        //if it head we need to remove it and make the node empty
        node<T>*temp= find(key);
        if (temp== nullptr)return -1;
        node<T>*prev=temp->prev;
        node<T>*next_node=temp->next;

        if (temp==head){
            if (head==tail){
                head= nullptr;
                tail= nullptr;
            } else{
                head=head->next;
                head->prev= nullptr;
            }


            delete(temp);
            return 1;
        }
        if (temp==tail){
            prev->next=temp->next;
            tail=prev;
            delete(temp);
            return 1;
        }
        prev->next=temp->next;
        next_node->prev=temp->prev;
        delete(temp);
       return 1;
    }
    void moveToFront(T key){

        node<T>*temp= find(key);
        if (temp== nullptr||temp== head)return;
        T k=temp->key;
        T value=temp->value;
        remove(key);
        insert(k,value);
    }
    void removeLast(){
        if (tail== nullptr)return;
      node<T>*toDelete=tail;
      if (head==tail){
          head= nullptr;
          tail= nullptr;
      } else{
          tail=tail->prev;
          tail->next= nullptr;
      }
      delete(toDelete);
    }
    void print(){
        node<T>*temp=head;
        while (temp!= nullptr){
            cout<<temp->key;
            cout<< "->" << temp->value;
            cout<<"\n";
            temp=temp->next;
        }
        cout<<"\n";
    }
};

int main() {
    linked_list<int>Node;
    Node.insert(1,10);
    Node.insert(2,20);
    Node.insert(3,30);
    Node.insert(4,40);
    Node.print();
    Node.moveToFront(1);
    Node.print();
    return 0;
}