// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <netinet/in.h>

#include "ipv6_endpoint.hpp"
#include "sdp.hpp"

namespace eviso15118::io {

struct PeerRequestContext {
    explicit PeerRequestContext(bool valid_) : valid(valid_){};
    v2gtp::Security security;
    v2gtp::TransportProtocol transport_protocol;
    struct sockaddr_in6 address;

    operator bool() const {
        return valid;
    }

private:
    const bool valid;
};

class SdpServer {
public:
    SdpServer();
    ~SdpServer();
    PeerRequestContext get_peer_request();

    //RDB new for getting the SDP response from the EVSE
    PeerRequestContext get_peer_response();

    void send_response(const PeerRequestContext&, const Ipv6EndPoint&);

    // RDB - This is always the same (or set from config), so the request and endpoint don't need to be passed in.
    void send_request();

    auto get_fd() const {
        return fd;
    }

private:
    int fd{-1};
    uint8_t udp_buffer[2048];
};

} // namespace eviso15118::io
