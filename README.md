# sfml-cpp-game
# 2D Physics Simulation in C++

A lightweight, from-scratch 2D physics sandbox built using C++ and the SFML graphics library. 

This project focuses on translating pure physics equations into discrete computer logic without relying on pre-built game engines.

## 🛠️ Current Features (Day 3 Status)
* **Semi-Implicit Euler Integration:** Real-time gravitational acceleration calculations.
* **Kinetic Energy Loss:** Symmetrical boundary collision handler utilizing a coefficient of restitution (`e`).
* **Ground Friction:** Velocity dampening that smoothly brings objects to a dead stop when rolling.

## 🚀 Upcoming Features / To-Do List
- [ ] Fix collision loop calculation sequence for perfect edge-case stability.
- [ ] Implement left and right wall collision boundaries.
- [ ] Add user keyboard inputs (e.g., Spacebar to apply an upward impulse/jump force).

## 📦 How to Build
* Requires **SFML 3.0+** and a C++20 compatible compiler.
* Clone the repo and link SFML graphics/window/system modules.