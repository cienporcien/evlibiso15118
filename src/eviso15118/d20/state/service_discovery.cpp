// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <algorithm>

#include <eviso15118/d20/state/service_detail.hpp>
#include <eviso15118/d20/state/service_discovery.hpp>

#include <eviso15118/detail/helper.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/service_discovery.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>

namespace eviso15118::d20::state {


//RDB - setup the request message to avoid duplication
message_20::ServiceDiscoveryRequest ServiceDiscovery::setup_request(const d20::Session &session)
{
    message_20::ServiceDiscoveryRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}


void ServiceDiscovery::enter() {
    ctx.log.enter_state("ServiceDiscovery");

    //Prepare and send the request
    const auto req = ServiceDiscovery::setup_request(ctx.session);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType ServiceDiscovery::handle_event(AllocatorType& sa, FsmEvent ev) {
    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::ServiceDiscoveryResponse>()) {

        //RDB TODO handle the response correctly, including VAS and service list.
        //res->energy_transfer_service_list.

        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        //Move along to ServiceDetail
        return sa.create_simple<ServiceDetail>(ctx);
        

    } else if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {
        // const auto req = handle_response(*res, ctx.session);

        // ctx.reqpond(req);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected ServiceDiscoveryRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
