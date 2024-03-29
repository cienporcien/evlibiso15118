// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <ctime>

#include <eviso15118/d2/state/dc_cable_check.hpp>
#include <eviso15118/d2/state/schedule_exchange.hpp>

#include <eviso15118/detail/d2/context_helper.hpp>
#include <eviso15118/detail/d2/state/schedule_exchange.hpp>
#include <eviso15118/detail/d2/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d2::state {

message_2::ScheduleExchangeResponse handle_request(const message_2::ScheduleExchangeRequest& req,
                                                    const d2::Session& session,
                                                    const message_2::RationalNumber& max_power) {

    message_2::ScheduleExchangeResponse res;

    if (validate_and_setup_header(res.header, session, req.header.session_id) == false) {
        return response_with_code(res, message_2::ResponseCode::FAILED_UnknownSession);
    }

    // Todo(SL): Publish data from request?

    if (session.get_selected_control_mode() == message_2::ControlMode::Scheduled &&
        std::holds_alternative<message_2::ScheduleExchangeRequest::Scheduled_SEReqControlMode>(req.control_mode)) {

        auto& control_mode =
            res.control_mode.emplace<message_2::ScheduleExchangeResponse::Scheduled_SEResControlMode>();

        // Define one minimal default schedule
        // No price schedule, no discharging power schedule
        // Todo(sl): Adding price schedule
        // Todo(sl): Adding discharging schedule
        message_2::ScheduleExchangeResponse::ScheduleTuple schedule;
        schedule.schedule_tuple_id = 1;
        schedule.charging_schedule.power_schedule.time_anchor =
            static_cast<uint64_t>(std::time(nullptr)); // PowerSchedule is now active

        message_2::ScheduleExchangeResponse::PowerScheduleEntry power_schedule;
        power_schedule.power = max_power;
        power_schedule.duration = message_2::ScheduleExchangeResponse::SCHEDULED_POWER_DURATION_S;

        schedule.charging_schedule.power_schedule.entries.push_back(power_schedule);

        control_mode.schedule_tuple.push_back(schedule);

    } else if (session.get_selected_control_mode() == message_2::ControlMode::Dynamic &&
               std::holds_alternative<message_2::ScheduleExchangeRequest::Dynamic_SEReqControlMode>(
                   req.control_mode)) {

        auto& control_mode = res.control_mode.emplace<message_2::ScheduleExchangeResponse::Dynamic_SEResControlMode>();

    } else {
        logf("The control mode of the req message does not match the previously agreed contol mode.");
        return response_with_code(res, message_2::ResponseCode::FAILED);
    }

    res.processing = message_2::Processing::Finished;

    return response_with_code(res, message_2::ResponseCode::OK);
}

void ScheduleExchange::enter() {
    ctx.log.enter_state("ScheduleExchange");
}

FsmSimpleState::HandleEventReturnType ScheduleExchange::handle_event(AllocatorType& sa, FsmEvent ev) {
    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_request();

    if (const auto req = variant->get_if<message_2::ScheduleExchangeRequest>()) {

        message_2::RationalNumber max_charge_power = {0, 0};

        const auto selected_energy_service = ctx.session.get_selected_energy_service();

        if (selected_energy_service == message_2::ServiceCategory::DC) {
            max_charge_power = ctx.config.evse_dc_parameter.max_charge_power;
        } else if (selected_energy_service == message_2::ServiceCategory::DC_BPT) {
            max_charge_power = ctx.config.evse_dc_bpt_parameter.max_charge_power;
        }

        const auto res = handle_request(*req, ctx.session, max_charge_power);

        ctx.respond(res);

        if (res.response_code >= message_2::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        if (res.processing == message_2::Processing::Ongoing) {
            return sa.HANDLED_INTERNALLY;
        }

        return sa.create_simple<DC_CableCheck>(ctx);
    } else if (const auto req = variant->get_if<message_2::SessionStopRequest>()) {
        const auto res = handle_request(*req, ctx.session);

        ctx.respond(res);
        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected ScheduleExchangeReq! But code type id: %d", variant->get_type());

        // Sequence Error
        const message_2::Type req_type = variant->get_type();
        send_sequence_error(req_type, ctx);

        ctx.session_stopped = true;
        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d2::state
