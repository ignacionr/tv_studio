# TV Studio Codebase Rules & Coding Style

This document defines the educational coding style and design guidelines utilized throughout the `tv_studio` codebase. Maintain this style when modifying existing files or creating new components.

---

## 📖 1. Educational & Explanatory Commenting
* **Explain Language Mechanics**: Write inline comments explaining key C++ concepts to students when utilizing them (e.g., pointer dereferencing vs. address-of operators, smart pointers, value vs. reference parameters).
* **Demystify Memory & Lifetimes**: Explicitly comment on *why* a particular ownership model (e.g., `std::shared_ptr`) is chosen for decoupling entity lifespans instead of raw pointers.
* **Document Non-Obvious Syntax**: Label modern syntax elements such as uniform initialization or template instantiation when they are introduced.

---

## 🛠️ 2. Modern C++ Standard Preferences
* **Uniform Initialization**: Prefer initialization using brace-init syntax `{}` (e.g., `int x{5};` or `auto renderer{win.renderer()};`) instead of parenthesized or assignment syntax.
* **Lambdas over `std::bind`**: Prefer modern C++ lambda closures (`[capture](params) { body }`) over `std::bind` or placeholders.
* **RAII & Smart Pointers**: Strictly enforce Resource Acquisition Is Initialization (RAII). Avoid naked `new` or `delete` operators; use smart pointers (`std::shared_ptr`, `std::unique_ptr`) to manage resource lifetimes safely without garbage collection.
* **Functional Composition**: Use operator overloading (e.g., custom `operator+` and `operator+=` for `std::function`) to build clean pipelines and composite behavior updates.

---

## 🧩 3. Architectural Design Style
* **Templates & "Duck Typing"**: Prefer template-based generic programming (parametric polymorphism) over heavy object-oriented inheritance hierarchies. Let classes verify interface expectations implicitly at compile-time.
* **Structs vs. Classes**: Use simple `struct` structures for data-heavy structures and configurations where member variables are public by default.
* **Class Member Initialization Lists**: Always initialize class references and constant fields within the constructor's member initializer list (e.g., `: subject_{subject}, target_{target}`).

---

## 🏷️ 4. Naming Conventions
* **Private Member Variables**: Postfix all private or protected class/struct member variables with a trailing underscore (e.g., `position_`, `update_`, `react_`).
* **Type Namespaces**: Define alias types (e.g., `typedef TEvent EventType;` or `using CharacterType = ...`) to encapsulate implementation-specific types in generic classes.
