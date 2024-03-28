// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <thread>

#include <eviso15118/d20/state/acdp_vehicle_positioning.hpp>
#include <eviso15118/d20/state/authorization_setup.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/acdp_vehicle_positioning.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {


//RDB - setup the request message to avoid duplication
message_20::ACDP_VehiclePositioningRequest ACDP_VehiclePositioning::setup_request(const d20::Session &session)
{
    message_20::ACDP_VehiclePositioningRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}

void ACDP_VehiclePositioning::enter() {
    ctx.log.enter_state("ACDP_VehiclePositioning");

        //Prepare and send the request
    auto req = ACDP_VehiclePositioning::setup_request(ctx.session);
    req.EVMobilityStatus = 0;
    req.EVPositioningSupport = 0;
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType ACDP_VehiclePositioning::handle_event(AllocatorType& sa, FsmEvent ev) {


    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    //RDB TODO - in some cases, it is possible for the vehicle to decide for itself whether positioning
    //has been reached, however here we wait for the PPD on the infrastructure to decide.
    if (const auto res = variant->get_if<message_20::ACDP_VehiclePositioningResponse>()) {

        if (res->processing == message_20::Processing::Ongoing)
        {
            // Prepare and send the request
            // Wait a little bit to slow things down otherwise too many messages.
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            
            const auto req = ACDP_VehiclePositioning::setup_request(ctx.session);
            ctx.request(req);
            return sa.HANDLED_INTERNALLY;
        }
        return sa.create_simple<AuthorizationSetup>(ctx);
    } 
    else {
        ctx.log("expected ACDP_VehiclePositioningRes! But code type id: %d", variant->get_type());
        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
