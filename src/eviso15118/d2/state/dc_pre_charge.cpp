// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/d2/state/dc_pre_charge.hpp>
#include <eviso15118/d2/state/power_delivery.hpp>

#include <eviso15118/detail/d2/context_helper.hpp>
#include <eviso15118/detail/d2/state/dc_pre_charge.hpp>
#include <eviso15118/detail/d2/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d2::state {

std::tuple<message_2::DC_PreChargeResponse, session_2::feedback::DcChargeTarget>
handle_request(const message_2::DC_PreChargeRequest& req, const d2::Session& session, const float present_voltage) {

    message_2::DC_PreChargeResponse res;
    session_2::feedback::DcChargeTarget charge_target{};

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return {response_with_code(res, message_2::ResponseCode::FAILED_UnknownSession), charge_target};
    }

    charge_target.voltage = message_2::from_RationalNumber(req.target_voltage);
    charge_target.current = 0;

    res.present_voltage = message_2::from_float(present_voltage);

    return {response_with_code(res, message_2::ResponseCode::OK), charge_target};
}

void DC_PreCharge::enter() {
    ctx.log.enter_state("DC_PreCharge");
}

FsmSimpleState::HandleEventReturnType DC_PreCharge::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev == FsmEvent::CONTROL_MESSAGE) {
        const auto control_data = ctx.get_control_event<PresentVoltageCurrent>();
        if (not control_data) {
            // FIXME (aw): error handling
            return sa.HANDLED_INTERNALLY;
        }

        present_voltage = control_data->voltage;

        return sa.HANDLED_INTERNALLY;
    }

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_request();

    if (const auto req = variant->get_if<message_2::DC_PreChargeRequest>()) {
        const auto [res, charge_target] = handle_request(*req, ctx.session, present_voltage);

        // FIXME (aw): should we always send this charge_target, even if the res errored?
        ctx.feedback.dc_charge_target(charge_target);

        ctx.respond(res);

        if (res.response_code >= message_2::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<PowerDelivery>(ctx);

    } else if (const auto req = variant->get_if<message_2::SessionStopRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected DC_PreChargeReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_2::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d2::state
