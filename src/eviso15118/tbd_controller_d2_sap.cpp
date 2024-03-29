// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/tbd_controller_d2_sap.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>

#include <eviso15118/io/connection_plain.hpp>
#include <eviso15118/io/connection_ssl.hpp>
#include <eviso15118/session_d2_sap/iso.hpp>

#include <eviso15118/detail/helper.hpp>

namespace eviso15118 {

TbdController_2_sap::TbdController_2_sap(TbdConfig_2_sap config_, session_2_sap::feedback::Callbacks callbacks_) :
    config(std::move(config_)), callbacks(std::move(callbacks_)) {
    poll_manager.register_fd(sdp_server.get_fd(), [this]() { handle_sdp_server_input(); });
    session_config = d2_sap::SessionConfig();
}

//RDB - get the selected SAP version hopefully there is only one session...
int TbdController_2_sap::get_SAP_Version(){
    int SAP_Version=-1;
    for (auto& session : sessions) {
      SAP_Version = session.get_SAP_Version();
    }
    return SAP_Version;
}

//RDB - get the selected SAP version hopefully there is only one session...
std::unique_ptr<io::IConnection> TbdController_2_sap::get_SAP_IConnection(){
    std::unique_ptr<io::IConnection> SAP_IConnection=nullptr;
    for (auto& session : sessions) {
      SAP_IConnection = std::move(session.connection);
    }
    return SAP_IConnection;
}

//RDB Get the Poll manager so we can initialize either the ISO20 or ISO2 poll manager
io::PollManager TbdController_2_sap::GetPollManager(){
    return std::move(poll_manager);
}


//RDB Here we want to only go as far as SupportedApplicationProtocol, then return.
void TbdController_2_sap::loop() {
    static constexpr auto POLL_MANAGER_TIMEOUT_MS = 50;

    auto next_event = get_current_time_point();

    //RDB just do this once for SupportedApplicationProtocol.
    while (true) {
        int thesapversion = -1;
        const auto poll_timeout_ms = get_timeout_ms_until(next_event, POLL_MANAGER_TIMEOUT_MS);
        poll_manager.poll(poll_timeout_ms);

        next_event = offset_time_point_by_ms(get_current_time_point(), POLL_MANAGER_TIMEOUT_MS);

        for (auto& session : sessions) {
            const auto next_session_event = session.poll();
            next_event = std::min(next_event, next_session_event);
            //RDB if SAP is handled, get out
            thesapversion = session.get_SAP_Version(); 
            if (thesapversion != -1){
                return;
            }
         
        }
         
    }
}

void TbdController_2_sap::send_control_event(const d2_sap::ControlEvent& event) {
    if (sessions.size() > 1) {
        logf("Inconsistent state, sessions.size() > 1 -- dropping control event");
        return;
    } else if (sessions.size() == 0) {
        return;
    }

    sessions.front().push_control_event(event);
}

// Should be called once
void TbdController_2_sap::setup_config() {
}

// Should be called before every session
void TbdController_2_sap::setup_session(const std::vector<message_2_sap::Authorization>& auth_services,
                                  bool cert_install_service) {

    if (auth_services.empty() == false) {
        session_config.authorization_services = auth_services;
    } else {
        session_config.authorization_services = {{message_2_sap::Authorization::EIM}};
    }

    session_config.cert_install_service = cert_install_service;
}

void TbdController_2_sap::handle_sdp_server_input() {
    auto request = sdp_server.get_peer_request();

    if (not request) {
        return;
    }

    switch (config.tls_negotiation_strategy) {
    case config::TlsNegotiationStrategy::ACCEPT_CLIENT_OFFER:
        // nothing to change
        break;
    case config::TlsNegotiationStrategy::ENFORCE_TLS:
        request.security = io::v2gtp::Security::TLS;
        break;
    case config::TlsNegotiationStrategy::ENFORCE_NO_TLS:
        request.security = io::v2gtp::Security::NO_TRANSPORT_SECURITY;
        break;
    }

    auto connection = [this](bool secure_connection) -> std::unique_ptr<io::IConnection> {
        if (secure_connection) {
            return std::make_unique<io::ConnectionSSL>(poll_manager, config.interface_name, config.ssl, end_point);
        } else {
            return std::make_unique<io::ConnectionPlain>(poll_manager, config.interface_name, end_point);
        }
    }(request.security == io::v2gtp::Security::TLS);

    const auto ipv6_endpoint = connection->get_public_endpoint();

    // Todo(sl): Check if session_config is empty
    const auto& new_session = sessions.emplace_back(std::move(connection), session_config, callbacks);

    sdp_server.send_response(request, ipv6_endpoint);
}

} // namespace eviso15118
