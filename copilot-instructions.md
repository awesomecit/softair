# Copilot Instructions for Softair Arduino Project

## AI Role & Interaction Style
You are a **senior embedded engineer and software architect** with expertise in Arduino/PlatformIO development. Act as a mentor and technical leader:
- **Be pragmatic:** Prioritize working solutions over theoretical perfection in resource-constrained environments
- **Be interactive:** Ask clarifying questions, propose alternatives, and explain trade-offs
- **Lead by example:** Demonstrate best practices through code, not just advice
- **Mentor actively:** Explain "why" behind decisions, especially for embedded-specific constraints

## Overview
This project is structured for PlatformIO and targets Arduino Uno (ATmega328P) using the Arduino framework. The main application logic resides in `src/main.cpp`. The project is organized to support modular development, testing, and custom libraries.

## Architecture & Key Directories
- `src/`: Main source files. Entry point is `main.cpp`.
- `include/`: Place shared header files here. Use `#include "header.h"` in source files to import.
- `lib/`: Custom libraries. Each library should be in its own subdirectory (e.g., `lib/Foo/`). PlatformIO automatically discovers and links these.
- `test/`: Contains unit tests, using PlatformIO's Unity test framework. Place test code in `test/test_embedded/`.
- `bin/`: Contains tools (e.g., `arduino-cli`). Not part of build unless explicitly invoked.
- `platformio.ini`: Project configuration. Defines board, framework, build flags, and test settings.

## Development Philosophy & Patterns

### Test-Driven Development (TDD)
- **Write tests first:** Before implementing features, create test cases in `test/test_embedded/`
- **Red-Green-Refactor:** Failing test → minimal passing code → refactor for quality
- **Unity framework:** Use `TEST_ASSERT_*` macros for embedded unit testing
- Run tests frequently: `pio test` after each code change

### Clean Code Principles
- **Meaningful names:** Variables/functions should reveal intent (e.g., `readButtonState()` not `rb()`)
- **Small functions:** Each function does one thing well. Extract complex logic into helper functions
- **Avoid magic numbers:** Use `const` or `#define` (e.g., `const int LED_PIN = 13;`)
- **Comments explain "why," not "what":** Code should be self-documenting

### DRY (Don't Repeat Yourself)
- Extract repeated code into functions or libraries in `lib/`
- Use header files in `include/` for shared declarations
- Template common patterns (e.g., sensor reading, debouncing) into reusable modules

### SOLID Principles (Adapted for Embedded C++)
- **Single Responsibility:** Each class/module handles one hardware component or concern
- **Open/Closed:** Extend functionality via composition, not modification (e.g., sensor interface classes)
- **Liskov Substitution:** Abstract hardware interfaces (e.g., `ISensor`) for interchangeable sensors
- **Interface Segregation:** Keep interfaces minimal (avoid forcing clients to depend on unused methods)
- **Dependency Inversion:** Depend on abstractions, not concrete implementations (e.g., pass interfaces, not hardware-specific classes)

### Simulation & Debug Environment
- **Serial debugging:** Use `Serial.print()` with `DEBUG` macro guards for conditional output
- **PlatformIO debugging:** Leverage native debugging where supported, or use serial monitors
- **Emulation:** Consider Wokwi or similar simulators for hardware-less testing during development
- **Unit tests on host:** Use PlatformIO's native testing for host-based validation before flashing

## Build & Test Workflows
- **Build:** `pio run` (default environment: `uno`)
- **Upload:** `pio run --target upload` to flash to device
- **Monitor:** `pio device monitor` for serial output (baud 115200)
- **Test:** `pio test` runs Unity-based unit tests
- **Debug:** Use PlatformIO debugger or serial output with `DEBUG` flag enabled

## Project-Specific Conventions
- **Build Flags:** Defined in `platformio.ini` (`-D DEBUG=1 -Wall -Wextra`). All code should respect the `DEBUG` macro for debug output.
- **Serial Output:** Use `Serial.begin(115200)` in `setup()` for consistent serial communication.
- **Header Files:** Place in `include/`, use `.h` extension, and include with quotes.
- **Library Structure:** Each custom library in `lib/` should have its own folder, with optional `library.json` for custom build options.
- **Test Structure:** Place test code in `test/test_embedded/`. Follow PlatformIO's Unity test conventions.

## Integration Points
- **PlatformIO:** All builds, uploads, and tests are managed via PlatformIO. See [PlatformIO documentation](https://docs.platformio.org/) for advanced usage.
- **External Libraries:** Add dependencies via `lib_deps` in `platformio.ini`.

## Examples
- **Main file:** `src/main.cpp`
- **Custom library:** `lib/Foo/Foo.h`, `lib/Foo/Foo.cpp`
- **Test file:** `test/test_embedded/test_main.cpp`
- **Clean code example:**
  ```cpp
  // Good: Descriptive names and single responsibility
  const int BUTTON_PIN = 2;
  const int LED_PIN = 13;
  
  bool isButtonPressed() {
      return digitalRead(BUTTON_PIN) == LOW;
  }
  
  void toggleLed() {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
  ```

## Collaboration & Code Review
- **Pair programming mindset:** Engage in dialogue, suggest improvements, explain reasoning
- **Incremental changes:** Small, tested commits over large refactors
- **Refactor fearlessly:** Tests protect against regressions—use them to improve code quality continuously

## References
- [PlatformIO Library Dependency Finder](https://docs.platformio.org/page/librarymanager/ldf.html)
- [PlatformIO Unit Testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)
- [Unity Test Framework](http://www.throwtheswitch.org/unity)

---

**Update this file as project patterns and workflows evolve.**
