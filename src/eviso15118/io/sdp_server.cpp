// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/io/sdp_server.hpp>

#include <cstring>

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <exi/cb/exi_v2gtp.h>

#include <eviso15118/detail/helper.hpp>

namespace eviso15118 {

static void log_peer_hostname(const struct sockaddr_in6& address) {
    char hostname[128];
    socklen_t hostname_len = sizeof(hostname);

    const auto get_if_name_result = getnameinfo(reinterpret_cast<const struct sockaddr*>(&address), sizeof(address),
                                                hostname, hostname_len, nullptr, 0, NI_NUMERICHOST);

    if (0 == get_if_name_result) {
        logf("Got SDP response from %s\n", hostname);
    } else {
        logf("Got SDP response, but failed to get the address\n");
    }
}

namespace io {

SdpServer::SdpServer() {
    //RDB for the EV side, we don't bind. Just set up the socket.
    fd = socket(AF_INET6, SOCK_DGRAM, 0);

    if (fd == -1) {
        log_and_throw("Failed to open socket");
    }

}

SdpServer::~SdpServer() {
    // FIXME (aw): rather use some RAII class for this!
    logf("Shutting down SDP server!");
    if (fd != -1) {
        close(fd);
    }
}

PeerRequestContext SdpServer::get_peer_request() {
    decltype(PeerRequestContext::address) peer_address;
    socklen_t peer_addr_len = sizeof(peer_address);

    const auto read_result = recvfrom(fd, udp_buffer, sizeof(udp_buffer), 0,
                                      reinterpret_cast<struct sockaddr*>(&peer_address), &peer_addr_len);
    if (read_result <= 0) {
        log_and_throw("Read on sdp server socket failed");
    }

    if (peer_addr_len > sizeof(peer_address)) {
        log_and_throw("Unexpected address length during read on sdp server socket");
    }

    log_peer_hostname(peer_address);

    if (read_result == sizeof(udp_buffer)) {
        logf("Read on sdp server socket succeeded, but message is to big for the buffer");
        return PeerRequestContext{false};
    }

    uint32_t sdp_payload_len;
    const auto parse_sdp_result = V2GTP20_ReadHeader(udp_buffer, &sdp_payload_len, V2GTP20_SDP_REQUEST_PAYLOAD_ID);

    if (parse_sdp_result != V2GTP_ERROR__NO_ERROR) {
        // FIXME (aw): we should not die here immediately
        logf("Sdp server received an unexpected payload");
        return PeerRequestContext{false};
    }

    PeerRequestContext peer_request{true};

    // NOTE (aw): this could be moved into a constructor
    const uint8_t sdp_request_byte1 = udp_buffer[8];
    const uint8_t sdp_request_byte2 = udp_buffer[9];
    peer_request.security = static_cast<v2gtp::Security>(sdp_request_byte1);
    peer_request.transport_protocol = static_cast<v2gtp::TransportProtocol>(sdp_request_byte2);
    memcpy(&peer_request.address, &peer_address, sizeof(peer_address));

    return peer_request;
}

//RDB this needs to be changed to get the response from the SECC instead of request from EVCC.
PeerRequestContext SdpServer::get_peer_response() {
    decltype(PeerRequestContext::address) peer_address;
    socklen_t peer_addr_len = sizeof(peer_address);

    const auto read_result = recvfrom(fd, udp_buffer, sizeof(udp_buffer), 0,
                                      reinterpret_cast<struct sockaddr*>(&peer_address), &peer_addr_len);
    if (read_result <= 0) {
        log_and_throw("Read on sdp server socket failed");
    }

    if (peer_addr_len > sizeof(peer_address)) {
        log_and_throw("Unexpected address length during read on sdp server socket");
    }

//    log_peer_hostname(peer_address);

    if (read_result == sizeof(udp_buffer)) {
        logf("Read on sdp server socket succeeded, but message is to big for the buffer");
        return PeerRequestContext{false};
    }

    uint32_t sdp_payload_len;
    //RDB change the payload ID to response, since we are on the other side now.
    const auto parse_sdp_result = V2GTP20_ReadHeader(udp_buffer, &sdp_payload_len, V2GTP20_SDP_RESPONSE_PAYLOAD_ID);

    if (parse_sdp_result != V2GTP_ERROR__NO_ERROR) {
        // FIXME (aw): we should not die here immediately
        logf("Sdp server received an unexpected payload 2");
        return PeerRequestContext{false};
    }

    PeerRequestContext peer_response{true};
    bzero(&peer_response.address, sizeof(peer_response.address));
    peer_response.address.sin6_family=AF_INET6;

    // RDB get the address and port from the body.
    // NOTE (aw): this could be moved into a constructor
    uint8_t* sdp_response = udp_buffer + 8;
    const uint8_t sdp_request_byte1 = sdp_response[18];
    const uint8_t sdp_request_byte2 = sdp_response[19];
    peer_response.security = static_cast<v2gtp::Security>(sdp_request_byte1);
    peer_response.transport_protocol = static_cast<v2gtp::TransportProtocol>(sdp_request_byte2);

    memcpy(&peer_response.address.sin6_addr, sdp_response, sizeof(peer_response.address.sin6_addr));
    uint16_t port;
    memcpy(&peer_response.address.sin6_port, sdp_response + 16 , sizeof(peer_response.address.sin6_port));
    peer_response.address.sin6_scope_id=peer_address.sin6_scope_id;
    log_peer_hostname(peer_response.address);

    return peer_response;
}

void SdpServer::send_response(const PeerRequestContext& request, const Ipv6EndPoint& ipv6_endpoint) {
    // that worked, now response
    uint8_t v2g_packet[28];
    uint8_t* sdp_response = v2g_packet + 8;
    memcpy(sdp_response, ipv6_endpoint.address, sizeof(ipv6_endpoint.address));

    uint16_t port = htobe16(ipv6_endpoint.port);
    memcpy(sdp_response + 16, &port, sizeof(port));

    // FIXME (aw): which values to take here?
    sdp_response[18] = static_cast<std::underlying_type_t<v2gtp::Security>>(request.security);
    sdp_response[19] =
        static_cast<std::underlying_type_t<v2gtp::TransportProtocol>>(request.transport_protocol);

    V2GTP20_WriteHeader(v2g_packet, 20, V2GTP20_SDP_RESPONSE_PAYLOAD_ID);

    const auto peer_addr_len = sizeof(request.address);

    sendto(fd, v2g_packet, sizeof(v2g_packet), 0, reinterpret_cast<const sockaddr*>(&request.address),
           peer_addr_len);
}

// RDB - This is always the same from the EV side (or set from config), so the request and endpoint don't need to be passed in.
// This is a UDP broadcast to port 15118 on the connected network. The sending IP address and port are included in the UDP
// broadcast so that the receiving SDP server can return a unicast UDP message to the sender
void SdpServer::send_request() {

    PeerRequestContext request(true);
    Ipv6EndPoint ipv6_endpoint;

    // The address is:
    //[V2G2-139]An SDP client shall send SECC Discovery Request message to the destination local-link
    //multicast address (FF02::1) as defined in IETF RFC 4291.
    bzero(&request.address, sizeof(request.address));
    request.address={AF_INET6, htons(15118)};
    inet_pton(AF_INET6, "ff02::1", &request.address.sin6_addr);

    request.security=v2gtp::Security::NO_TRANSPORT_SECURITY;
    request.transport_protocol=v2gtp::TransportProtocol::TCP;

    // The request has a payload length of 2 bytes and header of 8 bytes
    uint8_t v2g_packet[10];
    // Location of payload
    uint8_t* sdp_request = v2g_packet + 8;

    // FIXME (aw): which values to take here?
    sdp_request[0] = static_cast<std::underlying_type_t<v2gtp::Security>>(request.security);
    sdp_request[1] =
        static_cast<std::underlying_type_t<v2gtp::TransportProtocol>>(request.transport_protocol);

    V2GTP20_WriteHeader(v2g_packet, 2, V2GTP20_SDP_REQUEST_PAYLOAD_ID);

    sendto(fd, v2g_packet, sizeof(v2g_packet), 0, reinterpret_cast<const sockaddr*>(&request.address),
           sizeof(request.address));
}


#if 0

void parse_sdp_request(uint8_t* packet) {
    // check sdp header
    uint32_t sdp_payload_len;
    const auto parse_sdp_result = V2GTP20_ReadHeader(packet, &sdp_payload_len, V2GTP20_SDP_REQUEST_PAYLOAD_ID);

    if (parse_sdp_result != V2GTP_ERROR__NO_ERROR) {
        log_and_throw("Failed to parse sdp header");
    }

    logf("Got sdp payload of %d bytes\n", sdp_payload_len);
    const uint8_t sdp_request_byte1 = packet[8];

    switch (static_cast<v2gtp::Security>(sdp_request_byte1)) {
    case v2gtp::Security::TLS:
        logf(" -> TLS requested\n");
        break;
    case v2gtp::Security::NO_TRANSPORT_SECURITY:
        logf(" -> no security\n");
        break;
    default:
        logf(" -> EXCEPTION: reserved value\n");
        break;
    }

    const uint8_t sdp_request_byte2 = packet[9];
    switch (static_cast<v2gtp::TransportProtocol>(sdp_request_byte2)) {
    case v2gtp::TransportProtocol::TCP:
        logf(" -> TCP requested\n");
        break;
    case v2gtp::TransportProtocol::RESERVED_FOR_UDP:
        logf(" -> reserved for UDP\n");
        break;
    default:
        logf(" -> EXCEPTION: reserved value\n");
        break;
    }
}
#endif
} // namespace io

} // namespace eviso15118
