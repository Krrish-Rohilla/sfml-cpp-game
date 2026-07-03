# Skateboard Physics Engine 🛹

A lightweight, high-performance 2D physics simulation built from scratch to model custom skateboarding physics, mechanics, and object interactions.

## 🛠️ Tech Stack & Architecture
* **Language Standard:** C++26 (Bleeding-edge ISO standard features)
* **Graphics Library:** SFML 3 (Modernized event loops & native rendering)
* **Build System:** CMake 3.28+ (Cross-platform portability)
* **Architecture:** Modular, decoupled class system (`Engine` and `Ball` components)

---

## 📈 Dev Log & Progression

### Day 4: Architectural Refactoring & Build Management
* **Modularized the Codebase:** Completely split a monolithic `main.cpp` into isolated, scalable header (`.hpp`) and source (`.cpp`) modules.
* **Implemented CMake:** Dropped localized editor tasks to establish a standard `CMakeLists.txt` build pipeline, rendering the project universally buildable.
* **Upgraded to C++26:** Enforced modern development paradigms, utilizing clean explicit casting and `std::optional` event pipelines.

### Day 3: Custom Physics Implementation
* Engineered a flawless look-ahead state split collision algorithm to eliminate ground-clipping issues.
* Hand-coded velocity dampening for linear kinetic friction and gravitational acceleration vectors.

---

## 🚀 Repository Structure
```text
sfml-cpp-game/
├── CMakeLists.txt        # Universal build configuration
├── README.md             # Project documentation
└── src/
    ├── main.cpp          # Application entry point
    ├── Engine.hpp/.cpp   # Central game loop management & scene setup
    └── Ball.hpp/.cpp     # Custom physics bodies & collision bounds