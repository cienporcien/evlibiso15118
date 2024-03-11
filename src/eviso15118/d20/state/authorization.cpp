// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <algorithm>

#include <eviso15118/d20/state/authorization.hpp>
#include <eviso15118/d20/state/service_discovery.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/authorization.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>

namespace eviso15118::d20::state {

using AuthStatus = message_20::AuthStatus;

static bool find_auth_service_in_offered_services(const message_20::Authorization& res_selected_auth_service,
                                                  const d20::Session& session) {
    auto& offered_auth_services = session.offered_services.auth_services;
    return std::find(offered_auth_services.begin(), offered_auth_services.end(), res_selected_auth_service) !=
           offered_auth_services.end();
}


//RDB - setup the request message to avoid duplication
message_20::AuthorizationRequest Authorization::setup_request(const d20::Session &session)
{
    message_20::AuthorizationRequest req;
    setup_header(req.header,session);
    req.selected_authorization_service=message_20::Authorization::EIM;
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    return request_with_code(req, request_code);

}

void Authorization::enter() {
    ctx.log.enter_state("Authorization");

    //Prepare and send the request
    const auto req = Authorization::setup_request(ctx.session);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType Authorization::handle_event(AllocatorType& sa, FsmEvent ev) {

    // if (ev == FsmEvent::CONTROL_MESSAGE) {
    //     const auto control_data = ctx.get_control_event<AuthorizationRequest>();
    //     if (not control_data) {
    //         // FIXME (aw): error handling
    //         return sa.HANDLED_INTERNALLY;
    //     }

    //     if (*control_data) {
    //         authorization_status = AuthStatus::Accepted;
    //     } else {
    //         authorization_status = AuthStatus::Rejected;
    //     }
    
    //     return sa.HANDLED_INTERNALLY;
    // }

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::AuthorizationResponse>()) {
        // const auto req = handle_response(*res, ctx.session, authorization_status);

        // ctx.respond(req);

        // if (req.request_code >= message_20::RequestCode::FAILED) {
        //     ctx.session_stopped = true;
        //     return sa.PASS_ON;
        // }

        // RDB if the EIM is finished, go on, otherwise send another AuthorizationReq
        if (res->evse_processing == message_20::Processing::Finished)
        {
            authorization_status = AuthStatus::Pending; // reqet
            return sa.create_simple<ServiceDiscovery>(ctx);
        }
        else
        {
            // Prepare and send the request
            const auto req = Authorization::setup_request(ctx.session);
            ctx.request(req);
            return sa.HANDLED_INTERNALLY;
        }
    } else if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {
        // const auto req = handle_response(*res, ctx.session);
        // ctx.respond(req);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    } else {
        ctx.log("expected AuthorizationRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
