// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <eviso15118/d20/state/dc_charge_loop.hpp>
#include <eviso15118/d20/state/power_delivery.hpp>
#include <eviso15118/d20/state/session_stop.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/dc_pre_charge.hpp>
#include <eviso15118/detail/d20/state/power_delivery.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {


//RDB - setup the request message to avoid duplication
message_20::PowerDeliveryRequest PowerDelivery::setup_request(const d20::Session &session)
{
    message_20::PowerDeliveryRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}

void PowerDelivery::enter() {
    ctx.log.enter_state("PowerDelivery");
    //Prepare and send the request
    const auto req = PowerDelivery::setup_request(ctx.session);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType PowerDelivery::handle_event(AllocatorType& sa, FsmEvent ev) {


    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::DC_PreChargeResponse>()) {

        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.HANDLED_INTERNALLY;
    } else if (const auto res = variant->get_if<message_20::PowerDeliveryResponse>()) {

        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<DC_ChargeLoop>(ctx);
        
    } else {
        ctx.log("Expected DC_PreChargeRes or PowerDeliveryRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
