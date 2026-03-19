# 📊 Memory-Efficient Versioned File Indexer

## 🚀 Overview

This project is a **C++-based memory-efficient text indexing system** that processes large files using fixed-size buffers and supports multiple versions of documents.

It allows:

* 📌 Word frequency queries
* 🔄 Version comparison (diff)
* 🔝 Top-K frequent words

---

## 🧠 Key Features

* **Memory Efficient Processing**

  * Uses fixed-size buffers (256–1024 KB)
  * Avoids loading entire file into memory

* **Chunk-Aware Tokenization**

  * Handles words split across buffer boundaries

* **Version Control**

  * Maintains separate word-frequency maps for each version

* **Object-Oriented Design**

  * Encapsulation, Inheritance, Polymorphism
  * Abstract query interface

* **Template Programming**

  * Generic `getTopK()` for reusable sorting logic

---

## 🏗️ System Design

### Core Components:

* `filereader` → Buffered file reading
* `tokenizer` → Converts text into tokens
* `versionindex` → Stores word frequencies
* `indexbuilder` → Builds index from file
* `query` → Abstract base class

  * `word_count_query`
  * `diff_query`
  * `topk_query`

---

## ⚙️ How It Works

1. File is read in **fixed-size chunks**
2. Characters are processed into **tokens**
3. Tokens are stored in **hash maps**
4. Queries operate on in-memory indices

---

## 🧪 Example Usage

### 🔹 Word Count

```bash
./analyzer --file test_logs.txt --version v1 --buffer 256 --query word --word error
```

### 🔹 Difference Between Versions

```bash
./analyzer --file1 test_logs.txt --version1 v1 --file2 verbose_logs.txt --version2 v2 --buffer 256 --query diff --word error
```

### 🔹 Top-K Words

```bash
./analyzer --file test_logs.txt --version v1 --buffer 256 --query top --top 10
```

---

## ⚡ Compilation

```bash
g++ -O3 230865_Rohan.cpp -o indexer
```

---

## 📊 Complexity

* Time Complexity:

  * Indexing: **O(N)**
  * Top-K: **O(U log U)**

* Space Complexity:

  * **O(V + buffer size)**

---

## 📌 Key Highlights

* Efficient handling of **large files**
* Clean **OOP architecture**
* Demonstrates **real-world system design concepts**
* Optimized I/O using **binary mode + buffering**

---


## 👨‍💻 Author

**Rohan Kumar**
IIT Kanpur

---
