// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <thread>

#include <eviso15118/d20/state/acds_connect.hpp>
#include <eviso15118/d20/state/dc_cable_check.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/acds_connect.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {


//RDB - setup the request message to avoid duplication
message_20::ACDS_ConnectRequest ACDS_Connect::setup_request(const d20::Session &session)
{
    message_20::ACDS_ConnectRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}

void ACDS_Connect::enter() {
    ctx.log.enter_state("ACDS_Connect");

        //Prepare and send the request
    auto req = ACDS_Connect::setup_request(ctx.session);
    ctx.request(req);

}

FsmSimpleState::HandleEventReturnType ACDS_Connect::handle_event(AllocatorType& sa, FsmEvent ev) {


    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::ACDS_ConnectResponse>()) {

        if (res->processing == message_20::Processing::Ongoing)
        {
            // Prepare and send the request
            // Wait a little bit to slow things down otherwise too many messages.
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            
            const auto req = ACDS_Connect::setup_request(ctx.session);
            ctx.request(req);
            return sa.HANDLED_INTERNALLY;
        }
        return sa.create_simple<DC_CableCheck>(ctx);
        
    } else {
        ctx.log("expected ACDS_ConnectRes! But code type id: %d", variant->get_type());
        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
