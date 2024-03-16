// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <eviso15118/d20/state/session_stop.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {

// message_20::SessionStopRequest handle_response(const message_20::SessionStopResponse& res, const d20::Session& session) {

//     if (res.ev_termination_code.has_value()) {
//         logf("EV termination code: %s\n", res.ev_termination_code.value().c_str());
//     }
//     if (res.ev_termination_explanation.has_value()) {
//         logf("EV Termination explanation: %s\n", res.ev_termination_explanation.value().c_str());
//     }

//     message_20::SessionStopRequest req;

//     if (validate_and_setup_header(req.header, session, res.header.session_id) == false) {
//         return request_with_code(req, message_20::RequestCode::FAILED_UnknownSession);
//     }

//     if (res.charging_session == message_20::ChargingSession::ServiceRenegotiation &&
//         session.service_renegotiation_supported == false) {
//         return request_with_code(req, message_20::RequestCode::FAILED_NoServiceRenegotiationSupported);
//     }

//     // Todo(sl): Check res.charging_session

//     return request_with_code(req, message_20::RequestCode::OK);
// }

// RDB - setup the request message to avoid duplication
message_20::SessionStopRequest SessionStop::setup_request(const d20::Session &session)
{
    message_20::SessionStopRequest req;
    setup_header(req.header, session);

    message_20::RequestCode request_code = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}


void SessionStop::enter() {
    ctx.log.enter_state("SessionStop");
    // Prepare and send the request
    const auto req = SessionStop::setup_request(ctx.session);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType SessionStop::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {

        // Todo(sl): Tell the reason why the charger is stopping. Shutdown, Error, etc.
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected SessionStop! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
