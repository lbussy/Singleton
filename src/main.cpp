/**
 * @file main.cpp
 * @brief A test application for Singleton, a C++ header-only class to enforce
 *        a Singleton condition for an application.
 *
 * This software is distributed under the MIT License. See LICENSE.md for
 * details.
 *
 * Copyright (C) 2025 Lee C. Bussy (@LBussy). All rights reserved.
 */

#include <iostream>
#include <cstdlib>
#include "singleton.hpp"

/**
 * @brief Tests the SingletonProcess class with a given port.
 *
 * Verifies whether the SingletonProcess instance can be created and whether
 * binding to the same port is prevented as expected.
 *
 * @param port The port number to use for testing.
 */
void test_singleton(uint16_t port)
{
    try
    {
        std::cout << "\nTesting SingletonProcess with port " << port << ".\n";

        SingletonProcess singleton(port);

        if (singleton())
        {
            std::cout << "Singleton instance created successfully on " << port << ".\n";
        }
        else
        {
            std::cerr << "Singleton instance creation failed.\n";
        }

        std::cout << "Testing binding to the same port (should fail)...\n";
        try
        {
            SingletonProcess singleton_fail(port);
            if (singleton_fail())
            {
                std::cerr << "Error: Should not be able to bind to the same port!\n";
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Expected failure: " << e.what() << "\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

/**
 * @brief Simulates a permission error by trying to bind to a restricted port.
 *
 * Attempts to create a SingletonProcess instance on a port that typically
 * requires root privileges (e.g., port 80) to ensure the proper error is raised.
 */
void simulate_permission_error()
{
    try
    {
        constexpr uint16_t restricted_port = 23; // Telnet, usually restricted
        std::cout << "\nTesting SingletonProcess on a restricted port (" << restricted_port << ")...\n";

        SingletonProcess singleton(restricted_port);

        if (singleton())
        {
            std::cerr << "Error: Should not be able to bind to a restricted port without root privileges!\n";
        }
    }
    catch (const SingletonException &e)
    {
        std::cerr << "Expected failure (singleton error): " << e.what() << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Expected failure (generic error): " << e.what() << "\n";
    }
}

/**
 * @brief Main function to run SingletonProcess tests.
 *
 * Executes tests for SingletonProcess class functionality, including creating
 * an instance on a user-specified port and simulating a permission error on a
 * restricted port.
 *
 * @param argc The argument count.
 * @param argv The argument values.
 * @return Exit status (0 for success, non-zero for error).
 */
int main(int argc, char *argv[])
{
    uint16_t test_port = 8080; // Default test port
    if (argc > 1)
    {
        test_port = static_cast<uint16_t>(std::atoi(argv[1]));
        if (test_port == 0)
        {
            std::cerr << "Invalid port specified. Defaulting to port 8080.\n";
            test_port = 8080;
        }
    }

    std::cout << "===========================\n";
    std::cout << "Testing SingletonProcess...\n";
    std::cout << "===========================\n";

    test_singleton(test_port);
    simulate_permission_error();

    std::cout << "\nSingletonProcess test completed.\n";
    return 0;
}
