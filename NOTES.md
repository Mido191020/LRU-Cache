# Session Notes: Custom LRU Cache Implementation

## Status & Summary
> **Log:** We wrote the first version of the project for today, but we need to sleep now. There was some small confusion on a few edge cases, but everything is fine—we understand the core project mechanics and can complete the remaining implementation in a short amount of time.

---

## 🛠️ What We Did Today (Branch: `write-my-own-version`)
1. **Initialized Custom Implementation**:
   - Began rewriting the LRU Cache architecture from scratch in [`main.cpp`](file:///D:/clion/LRU%20Cache/main.cpp) using C++ templates.
   - Built the generic [`struct node<T>`](file:///D:/clion/LRU%20Cache/main.cpp#L5-L16) to hold cache elements with prev/next pointers.
   - Created the generic [`class linked_list<T>`](file:///D:/clion/LRU%20Cache/main.cpp#L17-L64) to manage the recency-ordered doubly linked list.
2. **Operations Drafted**:
   - `insert(T data)`: Implemented head-insertion logic.
   - `find(T k)`: Added a node traversal lookup.
   - Outlined stubs for `remove()`, `moveToFront()`, and `removeLast()`.

---

## 🔍 Points of Clarification for the Next Session
When resuming fresh tomorrow, address these specific design refinements:
1. **Decouple Key & Value**:
   - A cache entry needs both `K key` and `V value` (e.g. `template <typename K, typename V>`).
   - When evicting the least recently used node from the tail, we need its `key` to delete the entry from `unordered_map` in $\mathcal{O}(1)$.
2. **Sentinel Nodes Pattern**:
   - Initialize list with dummy `head` and `tail` sentinels (`head->next = tail`, `tail->prev = head`).
   - This eliminates all `nullptr` checks and makes `moveToFront()` and `removeLast()` clean 3-line pointer updates.
3. **Strict $\mathcal{O}(1)$ Operations**:
   - Remove the `while (temp != nullptr)` tail-search loop inside `insert()`; maintain `tail` directly or use sentinels.
   - Delegate key lookup strictly to `std::unordered_map` instead of linear list traversal.
4. **LRUCache Wrapper**:
   - Wire `unordered_map<K, node<K, V>*>` with the list to expose the public `get(key)` and `put(key, value)` APIs.
