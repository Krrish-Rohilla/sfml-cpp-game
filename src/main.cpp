#include "Engine.hpp"
#include <iostream>
#include <exception>

/**
 * @brief Application entry point.
 * Instantiates the core Engine container framework and boots the master execution loop.
 * 
 * @return Int execution status code (0 for clean termination, 1 for fatal errors).
 */
int main() {
    // Advanced Defensive Architecture: Wrap the root runtime container in a comprehensive 
    // try-catch boundary to ensure any unexpected system or OS-level allocation driver 
    // exceptions are cleanly caught and reported instead of causing silent desktop crashes.
    try {
        // Instantiate the authoritative engine runtime anchor on the stack
        Engine engine("2D Custom Physics Engine", 1280, 720);

        // Block the primary execution thread and run the continuous 144Hz simulation loop
        engine.run();
    } 
    catch (const std::exception& error) {
        std::cerr << "\n[FATAL ENGINE CRASH]: An unhandled standard exception occurred!\n"
                  << "Details: " << error.what() << "\n\n";
        return 1;
    } 
    catch (...) {
        std::cerr << "\n[FATAL ENGINE CRASH]: An unidentified, non-standard exception occurred!\n\n";
        return 1;
    }

    std::cout << "\n[ENGINE SYSTEM]: Simulation terminated cleanly. Handing control back to OS.\n";
    return 0;
}