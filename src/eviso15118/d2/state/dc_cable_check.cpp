// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/d2/state/dc_cable_check.hpp>
#include <eviso15118/d2/state/dc_pre_charge.hpp>

#include <eviso15118/detail/d2/context_helper.hpp>
#include <eviso15118/detail/d2/state/dc_cable_check.hpp>
#include <eviso15118/detail/d2/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d2::state {

message_2::DC_CableCheckResponse handle_request(const message_2::DC_CableCheckRequest& req,
                                                 const d2::Session& session, bool cable_check_done) {

    message_2::DC_CableCheckResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, message_2::ResponseCode::FAILED_UnknownSession);
    }

    if (not cable_check_done) {
        res.processing = message_2::Processing::Ongoing;
    } else {
        res.processing = message_2::Processing::Finished;
    }

    return response_with_code(res, message_2::ResponseCode::OK);
}

void DC_CableCheck::enter() {
    ctx.log.enter_state("DC_CableCheck");
}

FsmSimpleState::HandleEventReturnType DC_CableCheck::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev == FsmEvent::CONTROL_MESSAGE) {
        const auto control_data = ctx.get_control_event<CableCheckFinished>();
        if (not control_data) {
            // FIXME (aw): error handling
            return sa.HANDLED_INTERNALLY;
        }

        cable_check_done = *control_data;

        return sa.HANDLED_INTERNALLY;
    }

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_request();

    if (const auto req = variant->get_if<message_2::DC_CableCheckRequest>()) {
        if (not cable_check_initiated) {
            ctx.feedback.signal(session_2::feedback::Signal::START_CABLE_CHECK);
            cable_check_initiated = true;
        }

        const auto res = handle_request(*req, ctx.session, cable_check_done);

        ctx.respond(res);

        if (res.response_code >= message_2::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        if (cable_check_done) {
            return sa.create_simple<DC_PreCharge>(ctx);
        } else {
            return sa.HANDLED_INTERNALLY;
        }
    } else if (const auto req = variant->get_if<message_2::SessionStopRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected DC_CableCheckReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_2::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d2::state
