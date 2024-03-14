// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <thread>

#include <eviso15118/d20/state/dc_cable_check.hpp>
#include <eviso15118/d20/state/dc_pre_charge.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/dc_cable_check.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {


//RDB - setup the request message to avoid duplication
message_20::DC_CableCheckRequest DC_CableCheck::setup_request(const d20::Session &session)
{
    message_20::DC_CableCheckRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}

void DC_CableCheck::enter() {
    ctx.log.enter_state("DC_CableCheck");

    //Prepare and send the request
    const auto req = DC_CableCheck::setup_request(ctx.session);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType DC_CableCheck::handle_event(AllocatorType& sa, FsmEvent ev) {


    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::DC_CableCheckResponse>()) {


        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        if (res->processing == message_20::Processing::Ongoing)
        {
            // Prepare and send the request
            // Wait a little bit to slow things down otherwise too many messages.
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            
            const auto req = DC_CableCheck::setup_request(ctx.session);
            ctx.request(req);
            return sa.HANDLED_INTERNALLY;
        }
              
        return sa.create_simple<DC_PreCharge>(ctx);

    } else if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {

        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected DC_CableCheckRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
