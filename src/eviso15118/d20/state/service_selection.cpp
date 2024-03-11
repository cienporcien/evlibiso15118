// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <eviso15118/d20/state/dc_charge_parameter_discovery.hpp>
#include <eviso15118/d20/state/service_selection.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/service_detail.hpp>
#include <eviso15118/detail/d20/state/service_selection.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {

//RDB - setup the request message to avoid duplication
message_20::ServiceSelectionRequest ServiceSelection::setup_request(const d20::Session &session)
{
    message_20::ServiceSelectionRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    req.selected_energy_transfer_service.service_id =message_20::ServiceCategory::DC;

    //RDB TODO fix this correctly
    req.selected_energy_transfer_service.parameter_set_id=0;
    return request_with_code(req, request_code);
}


void ServiceSelection::enter() {
    ctx.log.enter_state("ServiceSelection");
       //Prepare and send the request
    const auto req = ServiceSelection::setup_request(ctx.session);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType ServiceSelection::handle_event(AllocatorType& sa, FsmEvent ev) {

    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::ServiceDetailResponse>()) {
        logf("Response info about ServiceID: %d\n", res->service);

        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }
        // Prepare and send the request
        const auto req = ServiceSelection::setup_request(ctx.session);
        ctx.request(req);

        return sa.HANDLED_INTERNALLY;

    } else if (const auto res = variant->get_if<message_20::ServiceSelectionResponse>()) {


        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<DC_ChargeParameterDiscovery>(ctx);
        
        
    } else if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {

        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected ServiceDetailRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
