---
applyTo: "**/*.c,**/*.cpp,**/*.h,**/*.hpp"
---

# C++ Coding Guidelines

This document defines the standard conventions for writing and reviewing C++ code in the DigitShowDST project. Based on Modern C++ (C++17 and later) best practices, we aim for safe and maintainable code.

## Core Principles

### Function Qualifiers
- **Apply `constexpr` and `noexcept` to functions by default**
  - If `constexpr` is not possible, consider making it a `const` member function
  - Omit `noexcept` only when the function may throw exceptions
- **Mark function parameters as `const` whenever possible**
  - Even for pass-by-value parameters, `const` prevents accidental modifications within the function
  - This catches bugs where parameters are unintentionally modified

```cpp
// Good
constexpr int add(const int a, const int b) noexcept {
    return a + b;
}

// Runtime computation required
const std::string& getName() const noexcept {
    return name_;
}

// Parameters marked const prevent bugs
void processValue(const int value) noexcept {
    // value = 10;  // Compile error - prevented by const
    doSomething(value);
}
```

### Inline Functions
- **Aggressively inline non-complex functions**
  - Short functions defined in headers
  - Template functions (implicitly inline)
  - `constexpr` functions (implicitly inline)

```cpp
// Good - in header file
inline bool isValid(const int value) noexcept {
    return value >= 0 && value <= 100;
}
```

## Variable Definitions

### Constness Priority
1. **`constexpr`** - When the value can be a compile-time constant
2. **`const`** - When initialized at runtime but never modified
3. **mutable** - Only when modification is absolutely necessary

```cpp
// Good
constexpr int kMaxSize = 1024;           // Compile-time constant
const auto timestamp = getCurrentTime(); // Runtime constant
int counter = 0;                          // Only when necessary
```

### Scope Priority
1. **Local scope variables** - Define immediately before use
2. **`static` variables** - When sharing within a function is needed
3. **`inline` variables** - When sharing across translation units is needed
4. **Never use `extern`**

```cpp
// Good - prefer local scope
void processData() {
    const auto data = loadData();
    // Use data...
}

// static variable (shared within function)
int getNextId() noexcept {
    static int id = 0;
    return id++;
}

// inline variable (in header file)
namespace config {
    inline constexpr int kDefaultTimeout = 30;
}
```

### Namespace Usage
- **Always wrap `inline` variables in a namespace**

```cpp
// Good
namespace constants {
    inline constexpr double kPi = 3.14159265358979323846;
}

// Bad - inline variable in global scope
inline constexpr double kPi = 3.14159265358979323846;
```

## Functional Programming Principles

### Pure Functions and Side Effect Separation
- **Define functions as pure whenever possible, separating side effects**
- Depend only on inputs; return the same output for the same input

```cpp
// Good - pure function
constexpr double calculateArea(const double radius) noexcept {
    return 3.14159 * radius * radius;
}

// Clearly separate side effects
void logAndCalculate(const double radius) {
    const auto area = calculateArea(radius);  // Pure part
    std::println("Area: {}", area);            // Side effect
}
```

## Error Handling (Rust-Inspired Approach)

### Core Strategy
1. **Normal errors**  Include in return value using `std::optional` or `std::expected`
2. **Unexpected behavior (bugs)**  Throw exceptions
3. **Exception-throwing library functions**  Wrap with `try-catch` and convert to `std::expected`

```cpp
// Good - normal errors use optional
std::optional<int> parseInt(const std::string_view str) noexcept {
    try {
        return std::stoi(std::string(str));
    } catch (...) {
        return std::nullopt;
    }
}

// Good - expected for detailed error information (C++23)
std::expected<User, Error> loadUser(const int id) noexcept {
    try {
        const auto data = database.query(id);
        return User{data};
    } catch (const DatabaseError& e) {
        return std::unexpected(Error::DatabaseFailure);
    }
}

// Bad - throwing for normal errors
int parseInt(const std::string_view str) {
    return std::stoi(std::string(str));  // Exception leaks out
}
```

### Exceptions Indicate "Bugs"
```cpp
// Assertion-like usage
void processArray(const std::vector<int>& arr) {
    if (arr.empty()) {
        throw std::logic_error("Array must not be empty - this is a bug");
    }
    // Process...
}
```

## Leveraging Modern C++ Standard Library

### Prefer Standard Features
- **Use modern C++ features over old C APIs**

```cpp
// Good - Modern C++
std::string message = std::format("Value: {}", value);
auto file = std::ofstream("output.txt");
auto numbers = std::vector{1, 2, 3, 4, 5};

// Bad - old C API
char buffer[256];
sprintf(buffer, "Value: %d", value);
FILE* file = fopen("output.txt", "w");
```

### Containers and Ranges
```cpp
// Good - range-based for loop
for (const auto& item : collection) {
    process(item);
}

// Good - algorithms
std::ranges::sort(numbers);
const auto result = std::ranges::find_if(items, predicate);
```

## String Handling

### Priority Order
1. **`const char[N]`** - Fixed-length literals
2. **`std::string_view`** - Read-only, no ownership
3. **`std::string`** - Ownership needed, mutation required
4. **Never use `CString`** (minimize MFC dependency)

```cpp
// Good
void processName(const std::string_view name) noexcept {
    std::println("Hello, {}", name);
}

std::string buildMessage(const std::string_view prefix,
                         const std::string_view suffix) {
    return std::format("{}{}", prefix, suffix);
}

constexpr char kDefaultName[128] = "Unknown";

// Bad - using CString
void processName(const CString& name) {  // Avoid
    // ...
}
```

### String Literals
```cpp
// Good - using namespace for literals
using namespace std::string_literals;
auto str = "Hello"s;           // std::string
auto sv = "Hello"sv;           // std::string_view
```

## Memory Management

### Smart Pointer Usage
- **Avoid raw pointers; use smart pointers**

```cpp
// Good
auto ptr = std::make_unique<Widget>();
auto shared = std::make_shared<Resource>();

// Bad
Widget* ptr = new Widget();  // Avoid
```

### RAII Principle
- **Initialize resources upon acquisition; automatically release at scope end**

```cpp
// Good
{
    auto file = std::ofstream("data.txt");
    file << "content";
    // Automatically closed
}

// Bad
FILE* file = fopen("data.txt", "w");
fprintf(file, "content");
fclose(file);  // May forget
```

## Type Safety

### Type Deduction
```cpp
// Good - auto reduces verbosity
auto value = calculateValue();
const auto& item = container.front();

// Explicit type when clarity is important
std::string name = getName();  // auto is fine, but explicit is clearer
```

### Enumerations
```cpp
// Good - enum class
enum class Status {
    Success,
    Failure,
    Pending
};

Status status = Status::Success;

// Bad - old-style enum
enum Status {  // Pollutes scope
    SUCCESS,
    FAILURE
};
```

## Code Style

### Naming Conventions
- **Constants**: `kCamelCase` or `UPPER_SNAKE_CASE`
- **Variables/Functions**: `camelCase` or `snake_case` (project-consistent)
- **Types/Classes**: `PascalCase`
- **Member variables**: Trailing `_` (e.g., `value_`)
- **Namespaces**: `lowercase`

```cpp
// Constants
constexpr int kMaxRetries = 3;
constexpr int MAX_RETRIES = 3;  // Either is acceptable

// Class
class DataProcessor {
private:
    int count_;
    std::string name_;

public:
    constexpr int getCount() const noexcept { return count_; }
};
```

### Include Order
1. Corresponding header file (for .cpp files)
2. C++ standard library
3. Third-party libraries
4. Project headers

```cpp
// Corresponding header
#include "Widget.h"

// Standard library
#include <string>
#include <vector>

// Third-party
#include <nlohmann/json.hpp>

// Project header
#include "Utils.h"
```

## Handling Legacy Code

This project contains MFC-based legacy code. Refactoring policy:

1. **New code must follow these guidelines**
2. **When modifying existing code, modernize where feasible**
3. **Large refactorings should be done incrementally**
4. **Minimize MFC dependencies and migrate toward standard C++**

```cpp
// Refactoring example
// Before (MFC-dependent)
CString GetMessage() {
    CString msg = _T("Hello");
    return msg;
}

// After (standard C++)
std::string getMessage() noexcept {
    return "Hello";
}
```

## References

- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- [Modern C++ Features](https://github.com/AnthonyCalandra/modern-cpp-features)
- [C++17/20/23 Features (Japanese)](https://cpprefjp.github.io/)
- [Original Article (Japanese)](https://qiita.com/elipmoc101/items/01003c82dbd2e464a071)
