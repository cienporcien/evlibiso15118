// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/tbd_controller.hpp>
#include <eviso15118/d20/control_event.hpp>

#include <algorithm>
#include <chrono>
#include <cstdio>

#include <cstring>


#include <eviso15118/io/connection_plain.hpp>
#include <eviso15118/io/connection_ssl.hpp>
#include <eviso15118/session/iso.hpp>
#include <eviso15118/io/sdp_server.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118 {


TbdController::TbdController(TbdConfig config_, session::feedback::Callbacks callbacks_) :
    config(std::move(config_)), callbacks(std::move(callbacks_)) {
    poll_manager.register_fd(sdp_server.get_fd(), [this]() { handle_sdp_server_input(); });
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

//RDB since the EV doesn't have a session until after the SDP response is processed (see below),
//we need a special way to handle the SDP startup event (START_CHARGING with no existing session). 
void TbdController::send_control_event(const d20::ControlEvent &event)
{
    if (sessions.size() > 1)
    {
        logf("Inconsistent state, sessions.size() > 1 -- dropping control event");
        return;
    }
    else if (sessions.size() == 0)
    {
        //RDB this should only be the case if there is no session yet, and the event is a start charging event
        if (not std::holds_alternative<d20::StartStopCharging>(event)) {
            logf("Event Received with Inconsistent state, sessions.size() == 0 -- dropping control event");
            return;
        }
        else{
            //Get the value of the variant
            d20::start_stop_charging ssval = std::get<d20::StartStopCharging>(event);
            if (ssval == d20::start_stop_charging::START_CHARGING){
                //RDB TODO what about the timeout waiting for the SDP response? Start up a timer.
                this->sdp_server.send_request();
                return;
            }
            else{ //This should not happen for PAUSE and STOP, so ignore
                logf("Pause or Stop Charging received. Inconsistent state, sessions.size() == 0 -- dropping control event");
                return;
            }
        }
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

//RDB This needs to be changed for the EV side. Since the EV is initiating the SDP process, in order to
//respond to a start charging message from the ECU, it needs to initialize the first session before the first
//sdp response comes back so we have a functional state machine. This means we don't know whether or not the EVSE supports TLS until then. 
//Or, we can handle the SDP request process separately from the state machine (session), and set up the state machine here on the SDP reply, leaving
//things more or less the same. This is what I did.
void TbdController::handle_sdp_server_input() {
    auto response = sdp_server.get_peer_response();

    //memcpy(end_point.address, &response.address.sin6_addr, sizeof(&response.address.sin6_addr));
    std::copy(std::begin(response.address.sin6_addr.__in6_u.__u6_addr16), std::end(response.address.sin6_addr.__in6_u.__u6_addr16), std::begin(end_point.address));
    end_point.port=response.address.sin6_port;
    

    // RDB on the ev side, we connect with the IP, Port, and security specified in the SDP response. So, we need to pass in the address and port.  
    auto connection = [this](bool secure_connection) -> std::unique_ptr<io::IConnection> {
        if (secure_connection) {
            return std::make_unique<io::ConnectionSSL>(poll_manager, config.interface_name, config.ssl, end_point);
        } else {
            return std::make_unique<io::ConnectionPlain>(poll_manager, config.interface_name, end_point );
        }
    }(response.security == io::v2gtp::Security::TLS);

    const auto ipv6_endpoint = connection->get_public_endpoint();

    // Todo(sl): Check if session_config is empty
    const auto& new_session = sessions.emplace_back(std::move(connection), session_config, callbacks);


    //RDB don't reply, instead go to the first state (SupportedAppRequest) by sending a control event
    this->send_control_event(d20::StartStopCharging(d20::start_stop_charging::START_CHARGING));
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
