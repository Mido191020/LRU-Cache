#include <iostream>
#include <unordered_map>

using namespace std;

// Node Structure
template <typename K, typename V>
struct node {
    K key;
    V value;
    node* next;
    node* prev;
    
    node(K k = K(), V v = V()) :
        key(k),
        value(v),
        next(nullptr),
        prev(nullptr)
    {}
};

// Forward declaration
template <typename K, typename V>
class LRUCache;

// Doubly Linked List with Sentinel Nodes
template <typename K, typename V>
class linked_list {
private:
    node<K, V>* head;
    node<K, V>* tail;

public:
    linked_list() {
        head = new node<K, V>();
        tail = new node<K, V>();
        head->next = tail;
        tail->prev = head;
    }
    
    ~linked_list() {
        node<K, V>* curr = head;
        while (curr != nullptr) {
            node<K, V>* next_node = curr->next;
            delete curr;
            curr = next_node;
        }
    }

    // Rule of 5: Prevent shallow copies that cause double-free on destruction
    linked_list(const linked_list&) = delete;
    linked_list& operator=(const linked_list&) = delete;
    linked_list(linked_list&&) = delete;
    linked_list& operator=(linked_list&&) = delete;

    node<K,V>* insert(K key, V value) {
        node<K, V>* newNode = new node<K, V>(key, value);
        node<K, V>* first = head->next;

        newNode->next = first;
        newNode->prev = head;
        head->next = newNode;
        first->prev = newNode;
        return newNode;
    }
    
    node<K, V>* find(K k) {
        node<K, V>* temp = head->next;
        while (temp != tail) {
            if (temp->key == k) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }
    
    void removeNode(node<K, V>* target) {
        node<K, V>* prevNode = target->prev;
        node<K, V>* nextNode = target->next;

        prevNode->next = nextNode;
        nextNode->prev = prevNode;
    }
    
    int remove(node<K, V>* target) {
        if (target == nullptr) return -1;
        removeNode(target);
        delete target;
        return 1;
    }
    
    void Detach(node<K, V>* target) {
        removeNode(target);

        node<K, V>* first = head->next;
        target->next = first;
        target->prev = head;
        head->next = target;
        first->prev = target;
    }
    
    void moveToFront(node<K, V>* target) {
        if (target == nullptr || target == head->next) return;
        Detach(target);
    }
    
    node<K,V>* getLast() {
        if (tail->prev == head) return nullptr;
        return tail->prev;
    }
    
    void removeLast() {
        if (tail->prev == head) return;
        remove(tail->prev);
    }
    
    void print() {
        node<K, V>* temp = head->next;
        while (temp != tail) {
            cout << temp->key << "->" << temp->value << "\n";
            temp = temp->next;
        }
        cout << "\n";
    }
};

template <typename K, typename V>
class LRUCache {
private:
    int capacity;
    unordered_map<K, node<K, V>*> cacheMap;
    linked_list<K, V> cacheList;

public:
    explicit LRUCache(int c) : capacity(c) {}

    // Rule of 5: Prevent shallow copies
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
    LRUCache(LRUCache&&) = delete;
    LRUCache& operator=(LRUCache&&) = delete;

    node<K,V>* get(K key) {
        auto it = cacheMap.find(key);
        if (it == cacheMap.end())
            return nullptr;
        cacheList.moveToFront(it->second);
        return it->second;
    }

    node<K,V>* put(K key, V value) {
        if (capacity <= 0)
            return nullptr;
            
        auto it = cacheMap.find(key);
        if (it != cacheMap.end()) {
            node<K,V>* existNode = it->second;
            existNode->value = value;
            cacheList.moveToFront(existNode);
            return existNode;
        }
        
        node<K,V>* newNode = cacheList.insert(key, value);
        cacheMap[key] = newNode;
        
        if (cacheMap.size() > capacity) {
            node<K,V>* lastNode = cacheList.getLast();
            if (lastNode != nullptr) {
                K get_key = lastNode->key;
                cacheMap.erase(get_key);
                cacheList.removeLast();
            }
        }
        return newNode;
    }

    void display() {
        cout << "Cache State (MRU -> LRU):\n";
        cacheList.print();
    }
};

int main() {
    cout << "========================================\n";
    cout << "       LRU CACHE TEST (Capacity = 3)    \n";
    cout << "========================================\n\n";

    LRUCache<int, int> cache(3);

    cout << "[Step 1] Adding 3 entries: (1, 10), (2, 20), (3, 30)...\n";
    cache.put(1, 10);
    cache.put(2, 20);
    cache.put(3, 30);
    cache.display();

    cout << "[Step 2] Accessing key 1 (Promoting it to MRU)...\n";
    node<int, int>* res = cache.get(1);
    if (res) {
        cout << "Hit! Key: 1, Value: " << res->value << "\n";
    } else {
        cout << "Miss! Key 1 not found.\n";
    }
    cache.display();

    cout << "[Step 3] Adding key 4 (4, 40) -> Should evict key 2 (LRU)...\n";
    cache.put(4, 40);
    cache.display();

    cout << "[Step 4] Checking if key 2 is still in cache...\n";
    if (cache.get(2) == nullptr) {
        cout << "-> SUCCESS: Key 2 is evicted (cache miss) as expected!\n\n";
    } else {
        cout << "-> ERROR: Key 2 was NOT evicted!\n\n";
    }

    return 0;
}
