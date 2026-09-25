# LRU Cache — Engineered from First Principles

<div align="center">

[![C++](https://img.shields.io/badge/C++-17%2B-blue.svg?style=flat&logo=c%2B%2B)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

**A high-performance, purely $\mathcal{O}(1)$ Least Recently Used (LRU) Cache built from the ground up in Modern C++ using an `unordered_map` and a heavily optimized doubly linked list.**

[Architecture](#️-architecture) • [The 12-Bug Post-Mortem](#-the-12-bug-engineering-post-mortem) • [Usage](#-usage)

</div>

---

## 🎯 Overview

This repository isn't just an implementation of an LRU Cache; it is an **engineering case study** in building low-level data structures from first principles. 

Instead of hiding behind standard library lists, this project implements a **custom, memory-safe, templated Doubly Linked List**. It utilizes the **Sentinel (Dummy) Node pattern**, achieving zero-branch constant-time pointer surgeries ($\mathcal{O}(1)$) and eradicating the typical null-pointer edge cases that plague cache designs.

### Core Features
- ✅ **Pure $\mathcal{O}(1)$ operations:** Constant-time `get()`, `put()`, and LRU eviction.
- ✅ **Sentinel (Dummy) Node Architecture:** Permanently anchored `head` and `tail` nodes eliminate `if (nullptr)` edge-casing entirely.
- ✅ **Zero-Allocation Node Promotion:** Uses an in-place `Detach()` method to rewire pointers without triggering the `new`/`delete` heap allocator, solving Use-After-Free bugs.
- ✅ **Template-driven:** Type-safe for any `K` (Key) and `V` (Value) pairings.
- ✅ **Rule of 5 Compliant:** Prevents shallow-copy double-free memory corruption.

---

## 🏗️ Architecture

The cache operates by coordinating two structures:
1. **`std::unordered_map<K, node<K, V>*>`**: Provides $\mathcal{O}(1)$ lookup from a Key directly to the node's memory address in the linked list.
2. **`linked_list<K, V>`**: A custom doubly-linked list maintaining strict MRU (Most Recently Used) to LRU (Least Recently Used) chronological order.

### The Sentinel Node Invariant
By initializing the list with two dummy nodes (`head` and `tail`), the list is *never* empty from a pointer perspective. Every real node is guaranteed to have a `prev` and a `next`.

This turns complex conditional logic:
```cpp
// Bad: Traditional list removal
if (node->prev) node->prev->next = node->next;
else head = node->next;
if (node->next) node->next->prev = node->prev;
else tail = node->prev;
```
Into a universally true, branch-free, $\mathcal{O}(1)$ operation:
```cpp
// Good: Sentinel node removal
node->prev->next = node->next;
node->next->prev = node->prev;
```

### Visual Flow
```mermaid
flowchart LR
    Map[HashMap] -.->|O(1) Lookup| N2
    
    subgraph Doubly Linked List (MRU to LRU)
        H[Head Sentinel] <--> N1[Node 1]
        N1 <--> N2[Node 2]
        N2 <--> N3[Node 3]
        N3 <--> T[Tail Sentinel]
    end

    style H fill:#333,color:#fff
    style T fill:#333,color:#fff
    style Map fill:#0052cc,color:#fff
```

---

## 📓 The 12-Bug Engineering Post-Mortem

Building a linked data structure from scratch requires confronting memory corruption, segfaults, and topological bugs. 

I meticulously documented the entire engineering journey—including the root causes and mechanical fixes for 12 distinct low-level bugs—in **[`docs/NOTES.md`](docs/NOTES.md)**. 

If you want to understand how C++ handles stack garbage, Use-After-Free (UAF) vulnerabilities, template redefinitions, and NTFS process locking, read the post-mortem.

---

## 💻 Usage

### Quick Start

The core cache is a header-only, templated class. Drop [`include/LRUCache.hpp`](include/LRUCache.hpp) into your project.

```cpp
#include "LRUCache.hpp"
#include <iostream>

int main() {
    // Create an LRU cache with a strict capacity of 3
    LRUCache<int, std::string> cache(3);

    // 1. Insert values
    cache.put(1, "A");
    cache.put(2, "B");
    cache.put(3, "C");
    
    // Cache State: [3:"C" (MRU)] -> [2:"B"] -> [1:"A" (LRU)]

    // 2. O(1) Access and MRU Promotion
    node<int, std::string>* result = cache.get(1); // "A" moves to MRU
    
    // Cache State: [1:"A" (MRU)] -> [3:"C"] -> [2:"B" (LRU)]

    // 3. Exceed Capacity & Evict
    cache.put(4, "D"); // Cache is full. Key 2 ("B") is evicted in O(1).
    
    cache.display(); 
    // Output:
    // Cache State (MRU -> LRU):
    // 4->D
    // 1->A
    // 3->C
    
    return 0;
}
```

### API Reference

| Method | Complexity | Description |
|--------|------------|-------------|
| `LRUCache(int capacity)` | $\mathcal{O}(1)$ | Initializes cache and instantiates the underlying sentinel nodes. |
| `get(K key)` | $\mathcal{O}(1)$ | Looks up key in the hash map. If found, detaches and promotes the node to MRU. Returns `node*` or `nullptr`. |
| `put(K key, V value)` | $\mathcal{O}(1)$ | Inserts or updates a key. If capacity is exceeded, evicts the `tail->prev` node instantly. |
| `display()` | $\mathcal{O}(N)$ | Safely traverses the linked list from `head->next` to print the chronological state. |

---

## 🚀 Build & Test

To compile and run the interactive test suite located in [`src/main.cpp`](src/main.cpp):

**Using CMake (Recommended):**
```bash
mkdir build && cd build
cmake ..
cmake --build .
./LRU_Cache
```

**Using pure `g++`:**
```bash
g++ -std=c++17 src/main.cpp -I include -o LRU_Cache
./LRU_Cache
```

---

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
