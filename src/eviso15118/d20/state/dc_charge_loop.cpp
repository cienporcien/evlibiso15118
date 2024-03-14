// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <thread>
#include <eviso15118/d20/state/dc_charge_loop.hpp>
#include <eviso15118/d20/state/dc_welding_detection.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/dc_charge_loop.hpp>
#include <eviso15118/detail/d20/state/power_delivery.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {

using Scheduled_DC_Res = message_20::DC_ChargeLoopResponse::Scheduled_DC_CLResControlMode;
using Scheduled_BPT_DC_Res = message_20::DC_ChargeLoopResponse::BPT_Scheduled_DC_CLResControlMode;
using Dynamic_DC_Res = message_20::DC_ChargeLoopResponse::Dynamic_DC_CLResControlMode;
using Dynamic_BPT_DC_Res = message_20::DC_ChargeLoopResponse::BPT_Dynamic_DC_CLResControlMode;

using Scheduled_DC_Req = message_20::DC_ChargeLoopRequest::Scheduled_DC_CLReqControlMode;
using Scheduled_BPT_DC_Req = message_20::DC_ChargeLoopRequest::BPT_Scheduled_DC_CLReqControlMode;

// std::tuple<message_20::DC_ChargeLoopRequest, std::optional<session::feedback::DcChargeTarget>>
// handle_response(const message_20::DC_ChargeLoopResponse& res, const d20::Session& session, const float preqent_voltage,
//                const float preqent_current) {

//     message_20::DC_ChargeLoopRequest req;
//     std::optional<session::feedback::DcChargeTarget> charge_target{std::nullopt};

//     if (std::holds_alternative<Scheduled_DC_Res>(res.control_mode)) {

//         if (session.get_selected_energy_service() != message_20::ServiceCategory::DC) {
//             return {request_with_code(req, message_20::RequestCode::FAILED), charge_target};
//         }

//         const auto& res_mode = std::get<Scheduled_DC_Res>(res.control_mode);

//         charge_target = {
//             message_20::from_RationalNumber(res_mode.target_voltage),
//             message_20::from_RationalNumber(res_mode.target_current),
//         };

//         auto& mode = req.control_mode.emplace<Scheduled_DC_Req>();

//     } else if (std::holds_alternative<Scheduled_BPT_DC_Res>(res.control_mode)) {

//         if (session.get_selected_energy_service() != message_20::ServiceCategory::DC_BPT) {
//             return {request_with_code(req, message_20::RequestCode::FAILED), charge_target};
//         }

//         const auto& res_mode = std::get<Scheduled_BPT_DC_Res>(res.control_mode);

//         charge_target = {
//             message_20::from_RationalNumber(res_mode.target_voltage),
//             message_20::from_RationalNumber(res_mode.target_current),
//         };

//         auto& mode = req.control_mode.emplace<Scheduled_BPT_DC_Req>();
//     }

//     req.preqent_voltage = eviso15118::message_20::from_float(preqent_voltage);
//     req.preqent_current = eviso15118::message_20::from_float(preqent_current);

//     if (validate_and_setup_header(req.header, session, res.header.session_id) == false) {
//         return {request_with_code(req, message_20::RequestCode::FAILED_UnknownSession), charge_target};
//     }

//     return {request_with_code(req, message_20::RequestCode::OK), charge_target};
// }


//RDB - setup the request message to avoid duplication
message_20::DC_ChargeLoopRequest DC_ChargeLoop::setup_request(const d20::Session &session)
{
    message_20::DC_ChargeLoopRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;
    return request_with_code(req, request_code);
}



void DC_ChargeLoop::enter() {
    ctx.log.enter_state("DC_ChargeLoop");
    //Prepare and send the request
    const auto req = DC_ChargeLoop::setup_request(ctx.session);
    ctx.request(req);

}

FsmSimpleState::HandleEventReturnType DC_ChargeLoop::handle_event(AllocatorType& sa, FsmEvent ev) {


    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::PowerDeliveryResponse>()) {

        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        // Reqet
        // first_entry_in_charge_loop = true;

        // // Todo(sl): React properly to Start, Stop, Standby and ScheduleRenegotiation
        // if (res->response_code == message_20::PowerDeliveryResponse::Progreqs::Stop) {
        //     ctx.feedback.signal(session::feedback::Signal::CHARGE_LOOP_FINISHED);
        //     ctx.feedback.signal(session::feedback::Signal::DC_OPEN_CONTACTOR);
        //     return sa.create_simple<DC_WeldingDetection>(ctx);
        // }

//        return sa.HANDLED_INTERNALLY;
    } else if (const auto res = variant->get_if<message_20::DC_ChargeLoopResponse>()) {


        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        // Prepare and send the request
        //  Wait a little bit to slow things down otherwise too many messages.
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        const auto req = DC_ChargeLoop::setup_request(ctx.session);
        ctx.request(req);

        return sa.HANDLED_INTERNALLY;
    } else {
        ctx.log("Expected PowerDeliveryRes or DC_ChargeLoopRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
