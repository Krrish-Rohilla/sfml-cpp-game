# Skateboard Physics Engine 🛹

A lightweight, high-performance 2D physics simulation built from scratch to model custom skateboarding physics, procedural mechanics, and structural object interactions.

## 🛠️ Tech Stack & Architecture

* **Language Standard:** C++26 (Bleeding-edge ISO standard features)
* **Graphics Library:** SFML 3 (Modernized event loops & native vertex rendering arrays)
* **Build System:** CMake 3.28+ (Cross-platform portability)
* **Compiler & Environment:** GCC (UCRT64 via MSYS2)
* **Debugging Tools:** GDB (GNU Debugger) natively integrated
* **Architecture:** Modular, decoupled Class System (Engine, Ball, Line, and Map Generation components)

---

## 📈 Dev Log & Progression

### Day 6: Custom Vertex Geometry & Map Factories
* **Custom Geometric Pipeline:** Engineered a custom `Line` class to bypass the hardware limitation of standard 1-pixel line primitives. Rebuilt the pipeline using vector math to calculate a track line's direction, compute its perpendicular normal vector, and dynamically expand it into solid, thick geometric platforms utilizing GPU `sf::PrimitiveType::TriangleStrip` vertex buffers.
* **Decoupled Level Architecture:** Architected a static `MapGenerator` factory system to encapsulate level layout data. This isolates map parsing from the core `Engine` runtime loop, enforcing single-responsibility software design principles.
* **Trigonometric Procedural Generation:** Designed a mathematical arc generator (`addCurve`) utilizing trigonometry to dynamically stitch flat tracking segments together with smooth, custom-angled mathematical curves.

### Day 5: Environment Complete & Debugger Integration
* **Native Debugging Suite:** Integrated GDB (GNU Debugger) via MSYS2 (UCRT64) cleanly into the VS Code runtime environment for precise vector tracing and stack monitoring.
* **SFML 3 Migration Handling:** Resolved window creation API breaking changes between version branches by migrating to the explicit `sf::State` initialization format.
* **Multi-Threaded Parallel Compilation:** Confirmed a 100% zero-error executable target link across 16 logic threads natively.

### Day 4: Architectural Refactoring & Build Management
* **Modularized the Codebase:** Completely split a monolithic `main.cpp` into isolated, scalable header (`.hpp`) and source (`.cpp`) modules.
* **Implemented CMake:** Dropped localized editor tasks to establish a standard `CMakeLists.txt` build pipeline, rendering the project universally buildable.
* **Upgraded to C++26:** Enforced modern development paradigms, utilizing clean explicit casting and `std::optional` event pipelines.

### Day 3: Custom Physics Implementation
* **Collision Optimization:** Engineered a look-ahead state split collision algorithm to eliminate ground-clipping issues.
* **Kinematics Engine:** Hand-coded velocity dampening for linear kinetic friction and gravitational acceleration vectors.

---

## 🚀 Repository Structure

```text
sfml-cpp-game/
├── CMakeLists.txt         # Universal build configuration
├── README.md              # Project documentation
└── src/
    ├── main.cpp           # Application entry point
    ├── Engine.hpp/.cpp    # Central game loop execution & renderer dispatch
    ├── Ball.hpp/.cpp      # Rigid body simulation profiles & physics vectors
    ├── Line.hpp/.cpp      # Procedural normal-extruded TriangleStrip surfaces
    └── MapGenerator.hpp/.cpp # Decoupled level synthesis factory & curve toolsets