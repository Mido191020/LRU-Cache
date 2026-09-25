#ifndef LRU_CACHE_HPP
#define LRU_CACHE_HPP

#include <iostream>
#include <unordered_map>

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
            std::cout << temp->key << "->" << temp->value << "\n";
            temp = temp->next;
        }
        std::cout << "\n";
    }
};

template <typename K, typename V>
class LRUCache {
private:
    int capacity;
    std::unordered_map<K, node<K, V>*> cacheMap;
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
        std::cout << "Cache State (MRU -> LRU):\n";
        cacheList.print();
    }
};

#endif // LRU_CACHE_HPP
