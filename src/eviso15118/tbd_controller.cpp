// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/tbd_controller.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>

#include <eviso15118/io/connection_plain.hpp>
#include <eviso15118/io/connection_ssl.hpp>
#include <eviso15118/session/iso.hpp>

#include <eviso15118/detail/helper.hpp>

namespace eviso15118 {

//RDB - no longer needs to handle the sdp server input.
TbdController::TbdController(TbdConfig config_, session::feedback::Callbacks callbacks_) :
    config(std::move(config_)), callbacks(std::move(callbacks_)) {
//    poll_manager.register_fd(sdp_server.get_fd(), [this]() { handle_sdp_server_input(); });
    session_config = d20::SessionConfig();
}

void TbdController::loop() {
    static constexpr auto POLL_MANAGER_TIMEOUT_MS = 50;

    auto next_event = get_current_time_point();

    while (true) {
        const auto poll_timeout_ms = get_timeout_ms_until(next_event, POLL_MANAGER_TIMEOUT_MS);
        poll_manager.poll(poll_timeout_ms);

        next_event = offset_time_point_by_ms(get_current_time_point(), POLL_MANAGER_TIMEOUT_MS);

        for (auto& session : sessions) {
            const auto next_session_event = session.poll();
            next_event = std::min(next_event, next_session_event);
        }
    }
}

void TbdController::send_control_event(const d20::ControlEvent& event) {
    if (sessions.size() > 1) {
        logf("Inconsistent state, sessions.size() > 1 -- dropping control event");
        return;
    } else if (sessions.size() == 0) {
        return;
    }

    sessions.front().push_control_event(event);
}

// Should be called once
void TbdController::setup_config() {
}

// Should be called before every session
void TbdController::setup_session(const std::vector<message_20::Authorization>& auth_services,
                                  bool cert_install_service) {

    if (auth_services.empty() == false) {
        session_config.authorization_services = auth_services;
    } else {
        session_config.authorization_services = {{message_20::Authorization::EIM}};
    }

    session_config.cert_install_service = cert_install_service;
}

void TbdController::handle_sdp_server_input() {
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
            return std::make_unique<io::ConnectionSSL>(poll_manager, config.interface_name, config.ssl);
        } else {
            return std::make_unique<io::ConnectionPlain>(poll_manager, config.interface_name);
        }
    }(request.security == io::v2gtp::Security::TLS);

    const auto ipv6_endpoint = connection->get_public_endpoint();

    // Todo(sl): Check if session_config is empty
    const auto& new_session = sessions.emplace_back(std::move(connection), session_config, callbacks);

    sdp_server.send_response(request, ipv6_endpoint);
}

// RBD allow to pass in the IConnection made by SAP
void TbdController::set_SAP_IConnection(std::unique_ptr<io::IConnection> connection) {
    const auto& new_session = sessions.emplace_back(std::move(connection), session_config, callbacks);
    for (auto& session : sessions) {
        session.SetSessionStateConnected();
    }
}

// RDB also allow to copy the poll manager.
void TbdController::set_PollManager(io::PollManager pm) {
    //RDB we need to keep the event_fd that was created in the constructor of PollManager in this instance
    poll_manager.copy_pm(pm);
}

} // namespace eviso15118