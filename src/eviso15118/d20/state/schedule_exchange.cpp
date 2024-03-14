// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <ctime>

#include <eviso15118/d20/state/dc_cable_check.hpp>
#include <eviso15118/d20/state/schedule_exchange.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/schedule_exchange.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {



//RDB - setup the request message to avoid duplication
message_20::ScheduleExchangeRequest ScheduleExchange::setup_request(const d20::Session &session, const d20::SessionConfig &config)
{
    message_20::ScheduleExchangeRequest req;
    setup_header(req.header, session);
    message_20::RequestCode request_code = message_20::RequestCode::OK;

    // RDB TODO do this properly
    req.max_supporting_points = 1024;

    auto &control_mode =
        req.control_mode.emplace<message_20::ScheduleExchangeRequest::Scheduled_SEReqControlMode>();

    //Set one of the optional members
    message_20::RationalNumber max_energy = {20, 3};
    control_mode.max_energy.emplace<>(max_energy);

    return request_with_code(req, request_code);

}

void ScheduleExchange::enter() {
    ctx.log.enter_state("ScheduleExchange");
    //Prepare and send the request
    const auto req = ScheduleExchange::setup_request(ctx.session, ctx.config);
    ctx.request(req);
}

FsmSimpleState::HandleEventReturnType ScheduleExchange::handle_event(AllocatorType& sa, FsmEvent ev) {
    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::ScheduleExchangeResponse>()) {

        message_20::RationalNumber max_charge_power = {0, 0};


        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        if (res->processing == message_20::Processing::Ongoing) {
            return sa.HANDLED_INTERNALLY;
        }

        return sa.create_simple<DC_CableCheck>(ctx);
        

    } else if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected ScheduleExchangeRes! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
