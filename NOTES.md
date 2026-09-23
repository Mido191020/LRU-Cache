# Session Notes: Custom LRU Cache Implementation

## Status & Summary
> **Log:** The Doubly Linked List (`linked_list`) class is now fully implemented, debugged, and verified! All edge cases (head removal, tail eviction, single-node scenarios, use-after-free, and pointer initialization) have been tested and run cleanly. The foundation is complete.

---

## 🛠️ Completed Architecture: Production-Grade Doubly Linked List
Branch: `write-my-own-version` in [`main.cpp`](main.cpp)

### 1. Core Structures
- [`struct node<K, V>`](main.cpp#L5-L17):
  - Decoupled key (`K`) and value (`V`) types supporting heterogeneous storage.
  - Safe default constructor `node(K k = K(), V v = V())` for both real entries and sentinel nodes.
  - Initialized with modern `nullptr` pointers (`next`, `prev`).
- [`class linked_list<K, V>`](main.cpp#L18-L101):
  - **Sentinel (Dummy Nodes) Pattern**: Initializes permanently anchored `head` and `tail` sentinels (`head->next = tail; tail->prev = head;`), eliminating all `nullptr` edge-cases and conditional branching.
  - **Memory Leak Protection**: Destructor `~linked_list()` walks the chain and deallocates every node and sentinel from the heap upon exit.

### 2. Verified Branch-Free Operations
- **`insert(key, value)`**: Inserts immediately after `head` sentinel in $\mathcal{O}(1)$ with zero `if` conditions.
- **`removeNode(node<K, V>* target)`**: Direct 2-line pointer splice (`prev->next = next; next->prev = prev;`) universally applicable to any node without special casing head, tail, or single elements.
- **`remove(node<K, V>* target)`**: Unhooks the supplied node with `removeNode()` and frees its memory.
- **`Detach(node<K, V>* target)`**: Slices node from its position via `removeNode()` and promotes it directly after `head` in pure $\mathcal{O}(1)$ with zero branches.
- **`moveToFront(node<K, V>* target)`**: Promotes the supplied node to MRU position.
- **`removeLast()`**: Instantaneous $\mathcal{O}(1)$ LRU tail eviction targeting `tail->prev`.
- **`print()`**: Clean forward traversal from `head->next` to `tail` rendering `key->value`.

---

## 📓 Exhaustive Engineering Post-Mortem: All Mistakes & Bugs Faced

This section documents every error, bug, compiler/linker issue, and runtime failure encountered while building this low-level engine from scratch.

### 🔴 Category 1: Pointer & Memory Corruption (Segfaults / Access Violations)

#### Bug 1.1: Wild Uninitialized Member Pointers (`0xC0000005: STATUS_ACCESS_VIOLATION`)
* **The Buggy Code**:
  ```cpp
  template <typename T>
  class linked_list {
  private:
      node<T>* head; // Not initialized!
      node<T>* tail; // Not initialized!
  ```
* **What Happened**: In C++, primitive pointers on the stack are **not** zeroed out by default. They hold arbitrary garbage addresses from previous stack frames (e.g. `0x7ff6a2b0...`).
* **The Failure**: When calling `Node.insert(10)`, the check `if (head == nullptr)` evaluated to `false` because `head` held random garbage. The code immediately jumped to `head->prev = newNode;`, attempting to write to whatever random address was in `head`.
* **Symptom**: Instant crash with exit code `-1073741819` (`0xC0000005` hardware access violation).
* **The Fix**: Added a constructor to explicitly zero the pointers:
  ```cpp
  linked_list() {
      head = nullptr;
      tail = nullptr;
  }
  ```

---

#### Bug 1.2: Use-After-Free in `moveToFront()`
* **The Buggy Code**:
  ```cpp
  void moveToFront(T key) {
      node<T>* temp = find(key);
      remove(key);                    // 💥 delete temp; called here!
      insert(temp->key, temp->value); // 💥 Reading deallocated memory!
  }
  ```
* **What Happened**: `remove(key)` identified the node and freed it via `delete temp;`. The heap allocator returned that memory to the OS/heap manager. Immediately on the next line, `temp->key` and `temp->value` were read from the dangling pointer.
* **The Failure**: Classic **Use-After-Free (UAF)**. It may read corrupted garbage, crash, or overwrite newly allocated memory.
* **The Fix**: Stored `key` and `value` into local stack variables *before* deallocating the node:
  ```cpp
  T k = temp->key;
  T value = temp->value;
  remove(key);
  insert(k, value);
  ```

---

#### Bug 1.3: Dereferencing `nullptr` on Cache Miss in `moveToFront()`
* **The Buggy Code**:
  ```cpp
  void moveToFront(T key) {
      node<T>* temp = find(key);
      T k = temp->key; // Crashes if temp == nullptr!
  ```
* **What Happened**: If `find(key)` could not locate the key in the list, it returned `nullptr`. The code immediately tried to read `temp->key`, dereferencing address `0x0`.
* **The Failure**: Segmentation fault / crash whenever an un-cached key was promoted.
* **The Fix**: Added a safety guard at the start of the method:
  ```cpp
  if (temp == nullptr || temp == head) return;
  ```

---

### 🟠 Category 2: Topological & Invariant Logic Bugs

#### Bug 2.1: Cyclic Self-Referencing / The Infinite Loop Bug in `insert()`
* **The Buggy Code**:
  ```cpp
  void insert(T data) {
      node<T>* newNode = new node(data);
      if (head == nullptr) {
          head = newNode;
          tail = newNode;
          // 💥 Missing 'return;' or 'else' branch!
      }
      head->prev = newNode;
      newNode->next = head; // newNode->next = newNode!
      newNode->prev = nullptr;
      head = newNode;
  }
  ```
* **What Happened**: When inserting the very first node (`10`), `head` was assigned to `newNode`. Because there was no `return` or `else`, execution continued. `newNode->next = head;` set `10->next = 10` (the node pointed to itself!).
* **The Failure**: Any forward traversal (like `print()`) encountered a cycle and ran in an **infinite loop**, continuously printing `10 10 10 10...` until the process was killed.
* **The Fix**: Added an early `return;` inside the `if (head == nullptr)` block.

---

#### Bug 2.2: Premature List Truncation when Removing `head`
* **The Buggy Code**:
  ```cpp
  if (temp == head) {
      head = nullptr;
      tail = nullptr; // 💥 Wipes out the entire list!
      delete temp;
      return 1;
  }
  ```
* **What Happened**: Assumed that if `temp == head`, the list must only have 1 node.
* **The Failure**: If the list had multiple nodes (`4 -> 3 -> 2 -> 1`) and the head (`4`) was removed or moved to front, setting `tail = nullptr` and `head = nullptr` orphaned nodes `3, 2, 1`. The entire rest of the list was lost in memory.
* **The Fix**: Branched based on list size:
  ```cpp
  if (temp == head) {
      if (head == tail) { // Truly 1 node
          head = nullptr;
          tail = nullptr;
      } else {            // Multiple nodes
          head = head->next;
          head->prev = nullptr;
      }
      delete temp;
      return 1;
  }
  ```

---

#### Bug 2.3: `removeLast()` Traversal, 1-Node Crash, and $\mathcal{O}(N)$ Invariant Violation
* **The Buggy Code**:
  ```cpp
  void removeLast() {
      if (head == nullptr) return;
      node<T>* temp = head;
      node<T>* temp2 = tail;
      while (temp->next->next != nullptr) { // 💥 Crashes on 1-node list!
          temp = temp->next;
      }
      temp->next = nullptr;
      tail = temp;
      delete temp2;
  }
  ```
* **What Happened**:
  1. **Crash on 1 Node**: If the list has only 1 node, `temp->next` is `nullptr`. Evaluating `temp->next->next` attempts to read `nullptr->next` $\rightarrow$ Access Violation crash.
  2. **Dangling Pointer (Earlier Draft)**: An earlier version used `while (temp->next != nullptr)`, which made `temp` and `temp2` both point to `tail`. It then deleted `temp2` while setting `tail = temp`, leaving `tail` pointing to freed memory.
  3. **$\mathcal{O}(N)$ Invariant Violation**: In a doubly linked list, traversing from `head` to find the predecessor of `tail` violates the core $\mathcal{O}(1)$ LRU requirement.
* **The Fix**: Replaced the loop entirely with direct predecessor lookup via `tail->prev` in $\mathcal{O}(1)$:
  ```cpp
  void removeLast() {
      if (tail == nullptr) return;
      node<T>* toDelete = tail;
      if (head == tail) {
          head = nullptr;
          tail = nullptr;
      } else {
          tail = tail->prev;
          tail->next = nullptr;
      }
      delete toDelete;
  }
  ```

---

### 🟡 Category 3: Interface, Mapping & Type Inconsistencies

#### Bug 3.1: Inverted Constructor Parameters (`key` vs `value` Swap)
* **The Buggy Code**:
  ```cpp
  node(T d, T k) : value(d), key(k) {} // 1st param is value, 2nd is key
  ...
  void insert(T key, T value) {
      node<T>* newNode = new node(key, value); // Passed key first!
  ```
* **What Happened**: `key` was mapped to `value`, and `value` was mapped to `key`.
* **The Failure**: When inserting `(1, 10)`, the stored node had `key = 10` and `value = 1`. Calling `find(1)` or `moveToFront(1)` searched for key 1, failed to find it, and returned `nullptr`.
* **The Fix**: Aligned the constructor parameter order:
  ```cpp
  node(T k, T v) : key(k), value(v), next(nullptr), prev(nullptr) {}
  ```

---

#### Bug 3.2: Pass-By-Value Object vs Pointer Mismatch
* **The Buggy Code**:
  ```cpp
  node<T>* search_for_node(node<T> a_node) {
      ...
      if (temp == a_node) // 💥 Comparing pointer (node<T>*) with object (node<T>)
  }
  int remove(node<T> a_node) { ... }
  ```
* **What Happened**: Passed `node<T>` by value (copying the object) instead of by pointer `node<T>*`. Inside, tried to compare a pointer `temp` with the copied object `a_node`.
* **The Failure**: C++ has no default `operator==` between a pointer and an object. It only compiled initially because template methods are compiled lazily; as soon as called, it triggered compilation errors.
* **The Fix**: Changed signatures to take `node<T>*` or lookup by key `T key`.

---

#### Bug 3.3: Inverted Output Formatting in `print()`
* **The Buggy Code**:
  ```cpp
  cout << temp->value << "->" << temp->key;
  ```
* **The Failure**: Printed `value -> key` (e.g. `10->1`) instead of the standard `key -> value` mapping.
* **The Fix**: Corrected to `cout << temp->key << "->" << temp->value;`.

---

#### Bug 3.4: Duplicate Traversal Routines (`search_for_node` vs `find`)
* **What Happened**: `find(T k)` and `search_for_node(T key)` had identical implementations scanning the list.
* **The Fix**: Reused `find(key)` in `remove()` and eliminated redundant duplicate code.

---

### 🔵 Category 4: Build System & OS-Level Failures

#### Bug 4.1: Windows Executable File Lock (`ld.exe: Permission denied`)
* **The Compiler Error**:
  ```text
  D:\clion\CLion 2023.3.4\bin\mingw\bin/ld.exe: cannot open output file LRU_Cache.exe: Permission denied
  collect2.exe: error: ld returned 1 exit status
  ninja: build stopped: subcommand failed.
  ```
* **What Happened**: When a program crashes with an unhandled exception (such as Bug 1.1 or Bug 1.3), Windows Error Reporting (WER) or an orphaned debugger handle can hold the crashed process in a zombie kernel state (PID 1580).
* **OS Mechanic**: Windows NTFS places an exclusive write lock on active executable binaries. The compiler finished compilation, but the linker `ld.exe` could not overwrite `LRU_Cache.exe`.
* **The Fix**:
  1. Identified the zombie lock holding `LRU_Cache.exe`.
  2. Renamed the locked file to `LRU_Cache_old.exe` (NTFS allows renaming open files, freeing the original path).
  3. Fixed the underlying code crashes so subsequent builds and runs exit cleanly with code `0`.

---

## 🧠 Core Low-Level Systems Takeaways
1. **Always Zero Pointers in C++**: Never rely on default initialization for member pointers. Always use `head = nullptr;` or in-class member initializers.
2. **Never Read Memory After Deleting It**: Save needed fields on the stack before calling `delete` (or better yet, rewire pointers without deleting when moving nodes).
3. **Guard All Lookups for `nullptr`**: Never assume a key exists in a cache. Always guard before accessing `node->member`.
4. **Leverage Doubly Linked Pointers**: If a node has a `prev` pointer, you never need a `while` loop to find predecessors. Eviction must strictly be $\mathcal{O}(1)$.

---

## ✅ Completed: `LRUCache<K, V>` Wrapper
The cache coordinator is fully implemented in [`main.cpp`](main.cpp#L104-L149):
1. Embeds `unordered_map<K, node<K, V>*> cacheMap` and `linked_list<K, V> cacheList`.
2. `get(key)`:
   - Checks map in $\mathcal{O}(1)$.
   - If present, `moveToFront(node*)` and returns the node pointer.
   - If not found, returns `nullptr`.
3. `put(key, value)`:
   - If key exists: updates value and `moveToFront(node*)`.
   - If key is new:
     - Inserts new node at head and records in map.
     - If at `capacity`, evicts `getLast()` via `removeLast()` and erases from map.
4. `display()`: Prints cache state (MRU → LRU) via `cacheList.print()`.
