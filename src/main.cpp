#include <iostream>
#include "../include/LRUCache.hpp"

using namespace std;

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

    cout << "[Step 5] Updating key 3 to new value 300 (Promoting 3 to MRU)...\n";
    cache.put(3, 300);
    cache.display();

    cout << "[Step 6] Adding key 5 (5, 50) -> Should evict key 1 (LRU)...\n";
    cache.put(5, 50);
    cache.display();

    cout << "[Step 7] Checking if key 1 is still in cache...\n";
    if (cache.get(1) == nullptr) {
        cout << "-> SUCCESS: Key 1 is evicted (cache miss) as expected!\n\n";
    } else {
        cout << "-> ERROR: Key 1 was NOT evicted!\n\n";
    }

    return 0;
}
