#include <iostream>
#include <unordered_map>

using namespace std;
template <typename K, typename V>
struct node{
K key;
V value;
node*next;
node*prev;
node(K k = K(), V v = V()):
        key(k),
        value(v),
        next(nullptr),
        prev(nullptr)
    {}
};
template <typename K, typename V>
class linked_list{
private:
node<K, V>*head;
node<K, V>*tail;
public:
    linked_list(){
        head = new node<K, V>();
        tail = new node<K, V>();
        head->next = tail;
        tail->prev = head;
    }
    ~linked_list(){
        node<K, V>* curr = head;
        while (curr != nullptr){
            node<K, V>* next_node = curr->next;
            delete curr;
            curr = next_node;
        }
    }
    void insert(K key, V value){
        node<K, V>* newNode=new node<K, V>(key,value);
        node<K, V>* first=head->next;

        newNode->next=first;
        newNode->prev=head;
        head->next=newNode;
        first->prev=newNode;
    }
    node<K, V>* find(K k){
        node<K, V>*temp=head->next;
        while (temp!= tail){
            if (temp->key==k){
                return temp;
            }
            temp=temp->next;
        }
        return nullptr;
    }
    void removeNode(node<K, V>* target){
        node<K, V>* prevNode = target->prev;
        node<K, V>* nextNode = target->next;

        prevNode->next = nextNode;
        nextNode->prev = prevNode;
    }
   int remove(K key){
        node<K, V>*temp= find(key);
        if (temp== nullptr)return -1;
        removeNode(temp);
        delete(temp);
        return 1;
    }
    void Detach(node<K, V>*target){
        removeNode(target);

        node<K, V>* first = head->next;
        target->next = first;
        target->prev = head;
        head->next = target;
        first->prev = target;
    }
    void moveToFront(K key){
        node<K, V>*temp= find(key);
        if (temp== nullptr||temp== head->next)return;
        Detach(temp);
    }
    void removeLast(){
        if (tail->prev== head)return;
        node<K, V>*toDelete=tail->prev;
        removeNode(toDelete);
        delete(toDelete);
    }
    void print(){
        node<K, V>*temp=head->next;
        while (temp!= tail){
            cout<<temp->key;
            cout<< "->" << temp->value;
            cout<<"\n";
            temp=temp->next;
        }
        cout<<"\n";
    }
};

int main() {
    linked_list<int, int>Node;
    Node.insert(1,10);
    Node.insert(2,20);
    Node.insert(3,30);
    Node.insert(4,40);
    Node.print();
    Node.moveToFront(1);
    Node.print();
    return 0;
}