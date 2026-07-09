# Skateboard Physics Engine 🛹

A lightweight, high-performance 2D physics simulation built from scratch to model custom skateboarding physics, procedural mechanics, and structural object interactions.

## 🛠️ Tech Stack & Architecture

* **Language Standard**: C++26 (Bleeding-edge ISO standard features)
* **Graphics Library**: SFML 3 (Modernized event loops & native vertex rendering arrays)
* **Build System**: CMake 3.28+ (Cross-platform portability)
* **Compiler & Environment**: GCC (UCRT64 via MSYS2)
* **Debugging Tools**: GDB (GNU Debugger) natively integrated
* **Architecture**: Modular, decoupled Class System (Engine, Ball, Line, Physics, and Map Generation components)

---

## 📈 Dev Log & Progression

### Day 7: Batch Mesh Baking & Physics Isolation
* **Batch Vertex Architecture**: Implemented a mesh baking utility (`MapGenerator::bakeVisualMesh`) to compile all static map line segments and rounded end-caps into a single global `sf::VertexArray` using `sf::PrimitiveType::Triangles`[cite: 7, 8]. This optimizes rendering performance by reducing the entire track draw step to a single high-speed GPU graphics call[cite: 3].
* **Dedicated Physics Pipeline**: Decoupled collision mechanics from core execution blocks into a dedicated `PhysicsSystem` class[cite: 10]. This handles vector projection math, penetration depth correction, and localized sliding friction applied directly across tangent vectors[cite: 9].
* **Selective Collision Flagging**: Added a structural `collisionFree` toggle to line profiles[cite: 6], allowing specific tracking segments to bypass physics passes entirely to accommodate upcoming decorative or functional trigger setups[cite: 9].

### Day 6: Custom Vertex Geometry & Map Factories
* **Custom Geometric Pipeline**: Engineered a custom `Line` class to bypass the hardware limitation of standard 1-pixel line primitives[cite: 5, 6]. Rebuilt the pipeline using vector math to calculate a track line's direction, compute its perpendicular normal vector, and dynamically expand it into solid, thick geometric platforms utilizing GPU vertex buffers[cite: 5, 6].
* **Decoupled Level Architecture**: Architected a static `MapGenerator` factory system to encapsulate level layout data[cite: 8]. This isolates map parsing from the core `Engine` runtime loop, enforcing single-responsibility software design principles[cite: 3, 4].
* **Trigonometric Procedural Generation**: Designed a mathematical arc generator (`addCurve`) utilizing trigonometry to dynamically stitch flat tracking segments together with smooth, custom-angled mathematical curves[cite: 7, 8].

### Day 5: Environment Complete & Debugger Integration
* **Native Debugging Suite**: Integrated GDB (GNU Debugger) via MSYS2 (UCRT64) cleanly into the VS Code runtime environment for precise vector tracing and stack monitoring.
* **SFML 3 Migration Handling**: Resolved window creation API breaking changes between version branches by migrating to the explicit `sf::State::Windowed` initialization format[cite: 3].
* **Multi-Threaded Parallel Compilation**: Confirmed a 100% zero-error executable target link across 16 logic threads natively.

### Day 4: Architectural Refactoring & Build Management
* **Modularized the Codebase**: Completely split a monolithic `main.cpp` into isolated, scalable header (`.hpp`) and source (`.cpp`) modules.
* **Implemented CMake**: Dropped localized editor tasks to establish a standard `CMakeLists.txt` build pipeline, rendering the project universally buildable.
* **Upgraded to C++26**: Enforced modern development paradigms, utilizing clean explicit casting and `std::optional` event pipelines[cite: 3].

### Day 3: Custom Physics Implementation
* **Collision Optimization**: Engineered a look-ahead state split collision algorithm to eliminate ground-clipping issues.
* **Kinematics Engine**: Hand-coded velocity dampening for linear kinetic friction and gravitational acceleration vectors[cite: 1].

---

## 🚀 Repository Structure

```text
sfml-cpp-game/
├── CMakeLists.txt             # Universal build configuration
├── README.md                  # Project documentation
└── src/
    ├── main.cpp               # Application entry point
    ├── Engine.hpp/.cpp        # Central game loop execution & renderer dispatch
    ├── Ball.hpp/.cpp          # Rigid body simulation profiles & physics vectors
    ├── Line.hpp/.cpp          # Procedural normal-extruded vector lines & bounding state
    ├── PhysicsSystem.hpp/.cpp # Vector projection math, bounding resolution, & impulse bounce handling
    ├── MapGenerator.hpp/.cpp  # Decoupled level synthesis factory & vertex baking assembly
    └── VectorMath.hpp         # Standalone vector dot, normal, and length math utilities