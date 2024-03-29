// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "connection_abstract.hpp"

#include <eviso15118/config.hpp>
#include <eviso15118/io/poll_manager.hpp>

namespace eviso15118::io {

class ConnectionPlain : public IConnection {
public:
    ConnectionPlain(PollManager&, const std::string& interface_name, const io::Ipv6EndPoint& end_point);

    void set_event_callback(const ConnectionEventCallback&) final;
    Ipv6EndPoint get_public_endpoint() const final;
    void set_public_endpoint(const Ipv6EndPoint&) final;

    void write(const uint8_t* buf, size_t len) final;
    ReadResult read(uint8_t* buf, size_t len) final;

    void close() final;

    ~ConnectionPlain();

private:
    PollManager& poll_manager;

    Ipv6EndPoint end_point;

    int fd{-1};

    bool connection_open{false};

    ConnectionEventCallback event_callback{nullptr};

    void handle_connect();
    void handle_data();
};
} // namespace eviso15118::io
