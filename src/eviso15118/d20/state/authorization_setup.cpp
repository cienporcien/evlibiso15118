// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <random>

#include <eviso15118/d20/state/authorization.hpp>
#include <eviso15118/d20/state/authorization_setup.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/helper.hpp>

#include <eviso15118/detail/d20/state/authorization_setup.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>

namespace eviso15118::d20::state {

message_20::AuthorizationSetupRequest
handle_response(const message_20::AuthorizationSetupResponse& res, d20::Session& session, bool cert_install_service,
               const std::vector<message_20::Authorization>& authorization_services) {

    //RDB TODO do the correct thing with the information sent back in the response
    
    


    auto req = message_20::AuthorizationSetupRequest(); // default mandatory values [V2G20-736]

    return request_with_code(req, message_20::ResponseCode::OK);
}

void AuthorizationSetup::enter() {
    ctx.log.enter_state("AuthorizationSetup");

    message_20::AuthorizationSetupRequest req;
    setup_header(req.header,ctx.session);

    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType AuthorizationSetup::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::AuthorizationSetupResponse>()) {
        const auto req =
            handle_response(*res, ctx.session, ctx.config.cert_install_service, ctx.config.authorization_services);

        logf("Timestamp: %d\n", res->header.timestamp);

        //ctx.respond(req);

        //if (req.request_code >= message_20::RequestCode::FAILED) {
        //    ctx.session_stopped = true;
        //    return sa.PASS_ON;
       // }

        // Todo(sl): PnC is currently not supported
        //ctx.feedback.signal(session::feedback::Signal::REQUIRE_AUTH_EIM);

        return sa.create_simple<Authorization>(ctx);

    } else if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {
        //RBL handle stop correctly.
        //const auto req = handle_response(*res, ctx.session);

        //ctx.respond(req);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected AuthorizationSetupRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
