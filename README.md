# LRU Cache (HashMap + Doubly Linked List)

## 📌 Overview
This project implements a **Least Recently Used (LRU) Cache** using a combination of a **HashMap** and a **Doubly Linked List**, achieving **O(1)** time complexity for both `get` and `put` operations.

The implementation focuses on **clean systems design**, pointer manipulation, and edge-case safety, making it suitable for **low-level design (LLD)** and **system design interview preparation**.

---

## 🧠 Design Approach

The cache is built using two core data structures:

### 1. HashMap (`std::unordered_map`)
- Maps keys to nodes in the doubly linked list
- Enables **O(1)** lookup, insertion, and deletion

### 2. Doubly Linked List
- Maintains usage order:
  - **Head** → Most Recently Used (MRU)
  - **Tail** → Least Recently Used (LRU)
- Implemented using the **Dummy Node (Sentinel) pattern** to:
  - Eliminate null checks
  - Simplify insertion and deletion logic
  - Avoid edge-case bugs

---

## ⚙️ Key Features

- O(1) `get` and `put`
- Automatic eviction of Least Recently Used item
- Clean separation of concerns
- Pointer-safe list manipulation
- Interview-ready architecture

---

## 🧱 Core Components

- **Node**
  - Stores `key`, `value`
  - Contains `prev` and `next` pointers

- **Doubly Linked List**
  - Handles insertion, removal, and eviction
  - Uses sentinel head and tail nodes

- **LRU Cache**
  - Coordinates between the HashMap and the list
  - Enforces capacity constraints

---

## 📄 Documentation (Design Notes)

This repository includes a **visual design document** created using **Obsidian Canvas**:

