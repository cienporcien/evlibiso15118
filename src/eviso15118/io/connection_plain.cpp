// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/io/connection_plain.hpp>

#include <cassert>
#include <cstring>

#include <unistd.h>

#include <thread>
#include <net/if.h>

#include <eviso15118/detail/helper.hpp>
#include <eviso15118/detail/io/socket_helper.hpp>

namespace eviso15118::io {

ConnectionPlain::ConnectionPlain(PollManager& poll_manager_, const std::string& interface_name, const io::Ipv6EndPoint& end_point_) :
    poll_manager(poll_manager_), end_point(end_point_) {

    // setup end point information - on EV side, this was given in the SDP response.
    //Convert the address and port to a sockaddr_in6
    sockaddr_in6 address;
    bzero((char *) &address, sizeof(address));
    address.sin6_family=AF_INET6;
    //copy was suggested instead of memcpy
    std::copy(std::begin(end_point.address), std::end(end_point.address), std::begin(address.sin6_addr.__in6_u.__u6_addr16));
    //Port is already in reverse, don't need to switch bytes.
    address.sin6_port=end_point.port;
    //need the scope_id of the interface name.
    address.sin6_scope_id=if_nametoindex(interface_name.c_str());

    fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd == -1) {
        log_and_throw("Failed to create an ipv6 socket");
    }

    //On the ev side we are the client, and use connect instead of bind.
    const auto connect_result = connect(fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address));
    if (connect_result == -1) {
        const auto error = "Failed to connect ipv6 socket to interface " + interface_name + " Error= " + strerror(errno);
        log_and_throw(error.c_str());
    }

    //Not sure what to do here, 
    poll_manager.register_fd(fd, [this]() { this->handle_connect(); });
}

ConnectionPlain::~ConnectionPlain() = default;

void ConnectionPlain::set_event_callback(const ConnectionEventCallback& callback) {
    this->event_callback = callback;
}

Ipv6EndPoint ConnectionPlain::get_public_endpoint() const {
    return end_point;
}

void ConnectionPlain::set_public_endpoint(const Ipv6EndPoint& ep) {
    end_point=ep;
}

void ConnectionPlain::write(const uint8_t* buf, size_t len) {
    assert(connection_open);

    const auto write_result = ::write(fd, buf, len);

    if (write_result == -1) {
        log_and_throw("Failed to write()");
    } else if (write_result != len) {
        log_and_throw("Could not complete write");
    }
}

ReadResult ConnectionPlain::read(uint8_t* buf, size_t len) {
    assert(connection_open);

    const auto read_result = ::read(fd, buf, len);
    if (read_result > 0) {
        size_t bytes_read = read_result;
        const auto would_block = (bytes_read < len);
        return {would_block, bytes_read};
    }

    if (read_result == -1) {
        // handle blocking read case
        if (errno == EAGAIN) {
            return {true, 0};
        }

        logf("ConnectionPlain::read failed with error code: %d", errno);
    }

    return {false, 0};
}

void ConnectionPlain::handle_connect() {
    // NOTE(aw): we could also determine the remote party here
    const auto accept_fd = accept4(fd, nullptr, nullptr, SOCK_NONBLOCK);
    if (accept_fd == -1) {
        log_and_throw("Failed to accept4");
    }

    poll_manager.unregister_fd(fd);
    ::close(fd);

    call_if_available(event_callback, ConnectionEvent::ACCEPTED);

    connection_open = true;
    call_if_available(event_callback, ConnectionEvent::OPEN);

    fd = accept_fd;
    poll_manager.register_fd(fd, [this]() { this->handle_data(); });
}

void ConnectionPlain::handle_data() {
    assert(connection_open);

    call_if_available(event_callback, ConnectionEvent::NEW_DATA);
}

void ConnectionPlain::close() {

    /* tear down TCP connection gracefully */
    logf("Closing TCP connection\n");

    // Wait for 5 seconds [V2G20-1643]
    std::this_thread::sleep_for(std::chrono::seconds(5));

    const auto shutdown_result = shutdown(fd, SHUT_RDWR);

    if (shutdown_result == -1) {
        log_and_throw("shutdown() failed");
    }

    // Waiting for client closing the connection
    std::this_thread::sleep_for(std::chrono::seconds(2));

    poll_manager.unregister_fd(fd);

    const auto close_shutdown = ::close(fd);

    if (close_shutdown == -1) {
        log_and_throw("close() failed");
    }

    logf("TCP connection closed gracefully");

    connection_open = false;
    call_if_available(event_callback, ConnectionEvent::CLOSED);
}

} // namespace eviso15118::io
