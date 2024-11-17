// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <netinet/in.h>
#include <string>

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

    //RDB add the additional properties for Wireless SDP
    v2gtp::P2PS_PPD p2ps_ppd;
    v2gtp::CouplingType coupling_type;
    std::string EVID; //20 ASCII characters
    std::string EVSEID; //36 ASCII characters

private:
    const bool valid;
};

//RDB for the SDP response data coming from the EV
struct PeerResponseContext {
    explicit PeerResponseContext(bool valid_) : valid(valid_){};
    v2gtp::Security security;
    v2gtp::TransportProtocol transport_protocol;
    struct sockaddr_in6 address;
    v2gtp::DiagStatus diag_status;
    v2gtp::CouplingType coupling_type;
    std::string EVSEID; //36 ASCII characters
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
    PeerResponseContext get_peer_response();

    void send_response(const PeerRequestContext&, const Ipv6EndPoint&);

    // RDB - This is always the same (or set from config), so the request and endpoint don't need to be passed in.
    void send_request(const bool IsWireless);

    auto get_fd() const {
        return fd;
    }

    //RDB add a property for Wireless vs PLC SDP. Default is PLC.
    enum class SDPType {
        SDP_FOR_PLC,
        SDP_FOR_WIRELESS,
    };


private:
    int fd{-1};
    uint8_t udp_buffer[2048];
    //RDB add a property for Wireless vs PLC SDP. Default is typically PLC.
    SDPType sdp_type{SDPType::SDP_FOR_WIRELESS};

    //Convert
    void convert_id(uint8_t *out, const std::string in, const int length, const std::string defaultID);
};

class TlsKeyLoggingServer {
public:
    TlsKeyLoggingServer(const std::string& interface_name, uint16_t port);
    ~TlsKeyLoggingServer();

    ssize_t send(const char* line);

    auto get_fd() const {
        return fd;
    }

private:
    int fd{-1};
    uint8_t buffer[2048];
    sockaddr_in6 destination_address{};
};


} // namespace eviso15118::io
