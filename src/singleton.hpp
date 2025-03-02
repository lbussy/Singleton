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

#include <unistd.h>      // close()
#include <cerrno>        // errno
#include <string>        // std::string, std::to_string()
#include <stdexcept>     // std::runtime_error
#include <system_error>  // std::system_error
#include <memory>        // std::unique_ptr

#include <netinet/in.h>  // sockaddr_in, INADDR_LOOPBACK, htons(), htonl()
#include <string.h>      // strerror

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
        : socket_fd_(-1), port_(port) {}

    [[nodiscard]] bool operator()()
    {
        if (socket_fd_ == -1)  // Only bind once
        {
            socket_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
            if (socket_fd_ < 0)
            {
                return false;
            }

            struct sockaddr_in name{};
            name.sin_family = AF_INET;
            name.sin_port = htons(port_);
            name.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

            if (bind(socket_fd_, reinterpret_cast<struct sockaddr *>(&name), sizeof(name)) != 0)
            {
                close(socket_fd_);
                socket_fd_ = -1;
                return false;
            }
        }
        return socket_fd_ != -1;
    }

    ~SingletonProcess()
    {
        if (socket_fd_ != -1)
        {
            close(socket_fd_);
            socket_fd_ = -1;
        }
    }

private:
    int socket_fd_;  // Store the socket for the lifetime of the process
    uint16_t port_;
};

#endif // SINGLETON_H
