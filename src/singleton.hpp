/**
 * @file singleton.hpp
 * @brief A C++ header-only class to enforce a Singleton condition for
 *        an application.
 *
 * This software is distributed under the MIT License. See LICENSE.md for
 * details.
 *
 * Copyright (C) 2025 Lee C. Bussy (@LBussy). All rights reserved.
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <netinet/in.h>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <iostream>

/**
 * @class SingletonException
 * @brief A custom exception for errors in the SingletonProcess class.
 */
class SingletonException : public std::runtime_error
{
public:
    explicit SingletonException(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @class SingletonProcess
 * @brief Ensures a single instance by binding to a specific port.
 */
class SingletonProcess
{
public:
    explicit SingletonProcess(uint16_t port)
        : socket_fd_(-1), rc_(1), port_(port) {}

    ~SingletonProcess()
    {
        if (socket_fd_ != -1)
        {
            if (close(socket_fd_) < 0)
            {
                std::cerr << "Warning: Failed to close socket, error: "
                          << strerror(errno) << std::endl;
            }
        }
    }

    [[nodiscard]] bool operator()()
    {
        if (socket_fd_ == -1 || rc_)
        {
            socket_fd_ = -1;
            rc_ = 1;

            if ((socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
            {
                throw std::system_error(errno, std::generic_category(),
                                        "Could not create socket on port " + std::to_string(port_));
            }

            struct sockaddr_in name{};
            name.sin_family = AF_INET;
            name.sin_port = htons(port_);
            name.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Restrict to local

            rc_ = bind(socket_fd_, reinterpret_cast<struct sockaddr *>(&name), sizeof(name));
            if (rc_ != 0)
            {
                if (errno == EADDRINUSE)
                {
                    throw SingletonException("Another instance is already running on port " +
                                             std::to_string(port_));
                }
                else
                {
                    throw std::system_error(errno, std::generic_category(),
                                            "Could not bind to port " + std::to_string(port_));
                }
            }
        }
        return (socket_fd_ != -1 && rc_ == 0);
    }

    [[nodiscard]] inline std::string GetLockFileName() const
    {
        return "port " + std::to_string(port_);
    }

private:
    int socket_fd_;
    int rc_;
    uint16_t port_;
};

#endif // SINGLETON_H
