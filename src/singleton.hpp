/**
 * @file singleton.hpp
 * @brief A C++ header-only class to enforce a Singleton condition for
 *        an application.
 *
 * This software is distributed under the MIT License. See LICENSE.md for
 * details.
 *
 * Copyright (C) 2025 Lee C. Bussy (@LBussy). All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SINGLETON_H
#define SINGLETON_H

#include <netinet/in.h>  // sockaddr_in, INADDR_LOOPBACK, htons(), htonl()
#include <unistd.h>      // close()
#include <cerrno>        // errno
#include <string>        // std::string, std::to_string()
#include <stdexcept>     // std::runtime_error
#include <system_error>  // std::system_error
#include <memory>        // std::unique_ptr

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
        : socket_fd_(nullptr, SocketDeleter{}), rc_(1), port_(port) {}

    [[nodiscard]] bool operator()()
    {
        if (!socket_fd_)
        {
            int sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock < 0)
            {
                throw std::system_error(errno, std::generic_category(),
                                        "Could not create socket on port " + std::to_string(port_));
            }

            struct sockaddr_in name{};
            name.sin_family = AF_INET;
            name.sin_port = htons(port_);
            name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

            if (bind(sock, reinterpret_cast<struct sockaddr *>(&name), sizeof(name)) != 0)
            {
                if (errno == EADDRINUSE)
                {
                    throw SingletonException("Another instance is already running on port " +
                                             std::to_string(port_));
                }
                throw std::system_error(errno, std::generic_category(),
                                        "Could not bind to port " + std::to_string(port_));
            }

            socket_fd_.reset(new int(sock));
        }
        return socket_fd_ != nullptr;
    }

private:
    struct SocketDeleter
    {
        void operator()(int *fd) const
        {
            if (fd && *fd != -1)
            {
                close(*fd);
                delete fd;
            }
        }
    };

    std::unique_ptr<int, SocketDeleter> socket_fd_; // Smart pointer for socket cleanup
    int rc_;
    uint16_t port_;
};
#endif // SINGLETON_H
