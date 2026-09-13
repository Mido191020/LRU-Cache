# Session Notes: Custom LRU Cache Implementation

## Status & Summary
> **Log:** The Doubly Linked List (`linked_list`) class is now fully implemented, debugged, and verified! All edge cases (head removal, tail eviction, single-node scenarios, use-after-free, and pointer initialization) have been tested and run cleanly. The foundation is complete.

---

## 🛠️ Completed: Doubly Linked List (`linked_list<T>`)
Branch: `write-my-own-version` in [`main.cpp`](file:///D:/clion/LRU%20Cache/main.cpp)

### 1. Core Structures
- [`struct node<T>`](file:///D:/clion/LRU%20Cache/main.cpp#L6-L18):
  - Stores `key`, `value`, `next`, and `prev`.
  - Constructor initializes `key(k)`, `value(v)`, and zeroes pointers.
- [`class linked_list<T>`](file:///D:/clion/LRU%20Cache/main.cpp#L20-L127):
  - Explicit default constructor initializes `head = nullptr;` and `tail = nullptr;` (preventing garbage pointer access violations).

### 2. Verified Operations
- **`insert(key, value)`**: $\mathcal{O}(1)$ prepend to head. Handles empty-list vs multi-node without circular loops.
- **`find(key)`**: Returns pointer to the matching node.
- **`remove(key)`**: Robust deletion handling all topologies:
  - Single-node list (`head == tail`): cleanly sets `head = nullptr; tail = nullptr;`.
  - Head removal (`temp == head`): advances `head = head->next; head->prev = nullptr;`.
  - Tail removal (`temp == tail`): pulls back `tail = tail->prev; tail->next = nullptr;`.
  - Middle node removal: splices neighbors (`prev->next = next; next->prev = prev;`).
  - Frees heap memory via `delete temp;`.
- **`moveToFront(key)`**:
  - Safe guards: `if (temp == head || temp == nullptr) return;`.
  - Re-promotes target node to head without memory leaks or use-after-free.
- **`removeLast()`**:
  - Pure $\mathcal{O}(1)$ tail eviction using `tail->prev`.
  - Zero loops, safely evicts LRU item for cache eviction.
- **`print()`**:
  - Formatted visualization of the chain (`key->value`).

---

## 🚀 Next Milestone: `LRU_Cache` Wrapper
With the doubly linked list verified, the final phase is wiring the cache coordinator:
1. Embed `unordered_map<int, node<int>*> cacheMap;` and `linked_list<int> cacheList;`.
2. Implement `get(key)`:
   - Check map in $\mathcal{O}(1)$.
   - If present, `moveToFront(key)` and return value.
   - If not found, return `-1`.
3. Implement `put(key, value)`:
   - If key exists: update value and `moveToFront(key)`.
   - If key is new:
     - If at `capacity`, evict tail via `removeLast()` and erase from map.
     - Insert new node to head and record in map.
