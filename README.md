# Skateboard Physics Engine 🛹

A lightweight, high-performance 2D physics simulation built from scratch to model custom skateboarding physics, procedural mechanics, and structural object interactions.

## 🛠️ Tech Stack & Architecture

* **Language Standard**: C++26 (Bleeding-edge ISO standard features)
* **Graphics Library**: SFML 3 (Modernized event loops & native vertex rendering arrays)
* **Build System**: CMake 3.28+ (Cross-platform portability)
* **Compiler & Environment**: GCC (UCRT64 via MSYS2)
* **Debugging Tools**: GDB (GNU Debugger) natively integrated
* **Architecture**: Modular, data-driven system (Data-driven scene initialization, decoupled physics solvers, and binary state serialization)

---

## 📈 Dev Log & Progression

### Day 10: Data-Driven Architecture & Binary Persistence
* **Data-Driven Scene Transition**: Shifted the engine from hardcoded entity spawning to a file-based architecture. The `Engine` now utilizes `MapParser` to initialize scenes dynamically from text files, enabling rapid level prototyping without recompilation.
* **Binary Persistence Layer**: Engineered the `Serializer` utility to handle high-performance binary save/load operations. This system maps physical constraints and object states to a binary format, ensuring state persistence for complex physics configurations.
* **Static Geometry Batching**: Integrated `MapRenderer` to handle the batching of static track geometry into single vertex arrays. This drastically reduces draw calls, ensuring high frame stability regardless of track complexity.
* **Ignition-Only Entry Layer**: Refactored `main.cpp` to strictly serve as an ignition entry point. All system initialization, resource allocation, and scene management are now encapsulated within the `Engine` constructor and run loop, enforcing a clean separation of concerns.

### Day 8: High-Fidelity Texturing, Rotational Correction & Pipeline Automation
* **Rotational Dynamics Alignment**: Corrected the relative surface velocity equations to align with SFML's native Y-down coordinate space.
* **Visual-Physical Origin Alignment**: Refactored positioning bindings to map directly to the geometric center of mass, resolving orbital pivot artifacts.
* **CMake Asset Pipeline**: Upgraded build automation to synchronize the `assets/` directory directly to the workspace.

### Day 7: Batch Mesh Baking & Physics Isolation
* **Batch Vertex Architecture**: Implemented a mesh baking utility to compile static map segments into global `sf::VertexArray` primitives.
* **Dedicated Physics Pipeline**: Decoupled collision mechanics into a dedicated `PhysicsSystem` class, handling SAT projections and sliding friction.

---

## 🚀 Repository Structure

```text
sfml-cpp-game/
├── CMakeLists.txt             # Universal build configuration
├── README.md                  # Project documentation
├── assets/maps/               # Text-based level definitions (default.txt)
└── src/
    ├── main.cpp               # Engine ignition point (Process lifetime)
    ├── Engine.hpp/.cpp        # System orchestration & hardware clock
    ├── LevelManager.hpp/.cpp  # Master entity/constraint ownership
    ├── MapParser.hpp/.cpp     # Data-driven level instantiation
    ├── MapRenderer.hpp/.cpp   # Static geometry batching & rendering
    ├── Serializer.hpp/.cpp    # Binary state persistence
    ├── PhysicsSystem.hpp/.cpp # SAT collision & impulse resolution
    ├── RigidBody.hpp/.cpp     # Physics entity definitions
    ├── SoftBody.hpp/.cpp      # Deformable mass-spring lattices
    ├── SpatialGrid.hpp/.cpp   # Broadphase collision optimization
    ├── Spring.hpp             # Constraint definitions
    └── VectorMath.hpp         # Standalone vector utilities