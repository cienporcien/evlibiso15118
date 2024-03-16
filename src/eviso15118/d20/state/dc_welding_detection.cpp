// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <eviso15118/d20/state/dc_welding_detection.hpp>
#include <eviso15118/d20/state/session_stop.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/dc_welding_detection.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {

// message_20::DC_WeldingDetectionRequest handle_response(const message_20::DC_WeldingDetectionResponse& res,
//                                                        const d20::Session& session, const float preqent_voltage) {
//     message_20::DC_WeldingDetectionRequest req;

//     if (validate_and_setup_header(req.header, session, res.header.session_id) == false) {
//         return request_with_code(req, message_20::RequestCode::FAILED_UnknownSession);
//     }

//     req.preqent_voltage = message_20::from_float(preqent_voltage);

//     return request_with_code(req, message_20::RequestCode::OK);
// }

// RDB - setup the request message to avoid duplication
message_20::DC_WeldingDetectionRequest DC_WeldingDetection::setup_request(const d20::Session &session)
{
    message_20::DC_WeldingDetectionRequest req;
    setup_header(req.header, session);

    message_20::RequestCode request_code = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}

void DC_WeldingDetection::enter()
{
    ctx.log.enter_state("DC_WeldingDetection");
    // Prepare and send the request
    const auto req = DC_WeldingDetection::setup_request(ctx.session);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType DC_WeldingDetection::handle_event(AllocatorType& sa, FsmEvent ev) {


    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::DC_WeldingDetectionResponse>()) {

        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        // RDB TODO handle this correctly
        // if (res->present_voltage )
        // {
        //     // Prepare and send the request
        //     const auto req = DC_WeldingDetection::setup_request(ctx.session);
        //     ctx.request(req);
        //     return sa.HANDLED_INTERNALLY;
        // }

        return sa.create_simple<SessionStop>(ctx);
        
    } else {
        ctx.log("expected DC_WeldingDetection! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
