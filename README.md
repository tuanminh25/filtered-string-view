# **Filtered String View**  

## **Overview**  

This project implements **`filtered_string_view`**, a lightweight, non-owning string view that allows **on-the-fly filtering** using predicates. It enables efficient string manipulation **without unnecessary copies**, leveraging **C++20 features** for optimal performance.  

### **Key Features**  
- **Custom filtering with predicates**, allowing selective character visibility.  
- **Efficient internal representation** using `const char *` and `std::size_t`.  
- **Multiple Constructors**:
  - **Implicit & explicit string constructors** (`std::string`, `const char *`)  
  - **Predicate-based constructors** for filtering.  
  - **Copy & move constructors** for efficient object management.  
- **Iterator Support**:
  - **Bidirectional `const_iterator`** for safe traversal.  
  - **Full range access** with `begin()`, `end()`, `rbegin()`, `rend()`, etc.  
- **Member Functions & Operators**:
  - **Element access** (`operator[]`, `.at()`, `.data()`)  
  - **Conversion to `std::string`** (`explicit operator std::string()`)  
  - **Comparison & ordering** (`operator==`, `operator<=>`)  
  - **Stream output** (`operator<<`)  
- **Utility Functions**:
  - **String manipulation tools**: `compose`, `split`, `substr`.  
- **Exception Safety & `const`-Correctness**:
  - Designed with `noexcept` guarantees where applicable.  
- **Performance Focus**:
  - **Avoids unnecessary allocations**, ensuring minimal memory overhead.  
  - **Uses `std::function<bool(const char &)>`** for filtering logic.  

This implementation is built around idea of  **modularity, performance, and extensibility**, with hope of making it a powerful tool for **string processing** in high-performance applications.  
