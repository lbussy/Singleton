# Singleton

## Overview

**Singleton** is a header-only C++ library that enforces a singleton condition for an application using UDP port binding. This ensures that only one instance of a program can run at a time by attempting to bind to a specific port.

## Features

- **Header-only**: No separate compilation is required.
- **Uses UDP sockets** to ensure a single instance.
- **Exception-based error handling** for better debugging.
- **Lightweight and easy to integrate** into existing C++ projects.

## Requirements

- C++17 or later
- A POSIX-compliant system (Linux, macOS, or WSL on Windows)
- `g++` or `clang++`

## Installation

This library does not require installation. Simply include `singleton.hpp` in your project:

```cpp
#include "singleton.hpp"
```

## Usage

### Example: Ensuring a Single Instance

```cpp
#include <iostream>
#include "singleton.hpp"

int main() {
    try {
        uint16_t port = 8080; // Port to enforce singleton
        SingletonProcess singleton(port);

        if (singleton()) {
            std::cout << "Application running with singleton enforcement on port "
                      << singleton.GetLockFileName() << std::endl;
        } else {
            std::cerr << "Another instance is already running." << std::endl;
        }
    } catch (const SingletonException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

## Compilation

To compile and test the `Singleton` library, use the provided `Makefile`:

```sh
make test
```

### Cleaning Build Artifacts

```sh
make clean
```

### Running Static Analysis

```sh
make lint
```

## Error Handling

- If another instance is already running on the specified port, an exception is thrown.
- If the socket cannot be created or bound, a `std::system_error` is raised.
- For restricted ports (e.g., ports < 1024), a failure message will be logged.

## License

This project is licensed under the **MIT License**. See [LICENSE.md](LICENSE.md) for details.

## Author

Lee C. Bussy (@LBussy)
