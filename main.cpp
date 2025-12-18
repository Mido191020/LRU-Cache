#include <iostream>
#include <unordered_map>

using namespace std;

class node{
public:
    int key;
    int value;
    node*prev;
    node*next;

    node(int k,int v){
        key=k;
        value=v;
        prev= nullptr;
        next= nullptr;
    }
};

class double_linked_list{
public:
    node* head;
    node* tail;

    double_linked_list(){
        head = new node(-1,-1);
        tail = new node(-1,-1);
        head->next = tail;
        tail->prev = head;
    }

    void remove_node(node* temp){
        node* before = temp->prev;
        node* after = temp->next;

        before->next = after;
        after->prev = before;
    }

    void add_first(node* temp){
        node* after_head = head->next;

        temp->prev = head;
        temp->next = after_head;

        head->next = temp;
        after_head->prev = temp;
    }

    void move_to_front(node* temp){
        remove_node(temp);
        add_first(temp);
    }

    // Returns the node pointer so we can delete it from memory later
    node* remove_last(){
        if(head->next == tail) return nullptr;

        node* last = tail->prev;
        remove_node(last);
        return last;
    }
};

class LRU_CACHE{
    int capacity;
    unordered_map<int, node*> cacheMap;
    double_linked_list cacheList;

public:
    LRU_CACHE(int cap) {
        capacity = cap;
    }

    int get(int key) {
        // 1. Check if key exists in the map
        if (cacheMap.find(key) == cacheMap.end()) {
            return -1; // Key not found
        }

        // 2. Access the node
        node* targetNode = cacheMap[key];

        // 3. This key was just used, so move it to the front (Most Recently Used)
        cacheList.move_to_front(targetNode);

        return targetNode->value;
    }

    void put(int key, int value) {
        // Case 1: Key already exists
        if (cacheMap.find(key) != cacheMap.end()) {
            node* existingNode = cacheMap[key];
            existingNode->value = value; // Update value
            cacheList.move_to_front(existingNode); // Bring to front
        }
            // Case 2: Key is new
        else {
            // Check if we are at full capacity
            if (cacheMap.size() >= capacity) {
                // We need to evict the Least Recently Used (Tail)
                node* lruNode = cacheList.remove_last();

                // Remove from Map
                cacheMap.erase(lruNode->key);

                // Free memory
                delete lruNode;
            }

            // Create new node
            node* newNode = new node(key, value);

            // Add to List (Front) and Map
            cacheList.add_first(newNode);
            cacheMap[key] = newNode;
        }
    }
};

int main() {
    // Testing the Cache
    cout << "--- Starting LRU Cache Test ---" << endl;

    // Capacity = 2
    LRU_CACHE lru(2);

    lru.put(1, 10); // Cache: {1=10}
    lru.put(2, 20); // Cache: {2=20, 1=10} (2 is most recent)

    cout << "Get 1: " << lru.get(1) << endl; // Returns 10. Cache: {1=10, 2=20} (1 moved to front)

    lru.put(3, 30); // Cache Full! Evicts 2 (LRU). Cache: {3=30, 1=10}

    cout << "Get 2: " << lru.get(2) << endl; // Returns -1 (was evicted)

    lru.put(4, 40); // Cache Full! Evicts 1 (LRU). Cache: {4=40, 3=30}

    cout << "Get 1: " << lru.get(1) << endl; // Returns -1 (was evicted)
    cout << "Get 3: " << lru.get(3) << endl; // Returns 30
    cout << "Get 4: " << lru.get(4) << endl; // Returns 40

    return 0;
}