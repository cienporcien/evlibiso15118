// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <algorithm>

#include <eviso15118/d20/state/authorization_setup.hpp>
#include <eviso15118/d20/state/session_setup.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/session_setup.hpp>
#include <eviso15118/detail/helper.hpp>

static bool session_is_zero(const eviso15118::message_20::Header& header) {
    return std::all_of(header.session_id.begin(), header.session_id.end(), [](int i) { return i == 0; });
}

namespace eviso15118::d20::state {

message_20::SessionSetupRequest handle_response(const message_20::SessionSetupResponse& res, const d20::Session& session,
                                                const std::string evse_id, bool new_session) {

    message_20::SessionSetupRequest req;
    setup_header(req.header, session);

    //req.evseid = evse_id;

    if (new_session) {
        return request_with_code(req, message_20::ResponseCode::OK_NewSessionEstablished);
    } else {
        return request_with_code(req, message_20::ResponseCode::OK_OldSessionJoined);
    }
}

void SessionSetup::enter()
{
    ctx.log.enter_state("SessionSetup");

    // Here, send the SessionSetupReq
    // RDB TODO make sure a timer is started.
    message_20::SessionSetupRequest req;

    // RDB TODO This needs to come from the config or the EV ECU
    req.evccid = "WMIV1234567890ABCDEX";
    req.header.session_id.fill(0);
    req.header.timestamp = static_cast<uint64_t>(std::time(nullptr));

    // Send it.
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType SessionSetup::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::SessionSetupResponse>()) {

        logf("Received session setup response with evseid: %s \n", res->evseid.c_str());

        bool new_session{true};

        if (session_is_zero(res->header)) {
            ctx.session = Session();
        } else if (res->header.session_id == ctx.session.get_id()) {
            new_session = false;
        } else {
            ctx.session = Session(res->header.session_id);
        }

        evse_id = ctx.config.evse_id;

        //RDB TODO - This needs to be updated, or maybe removed.
        const auto req = handle_response(*res, ctx.session, evse_id, new_session);

        //ctx.respond(req);

        return sa.create_simple<AuthorizationSetup>(ctx);


    } else {
        ctx.log("expected SessionSetupRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
