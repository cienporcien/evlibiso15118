// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVereqt
#include <eviso15118/d20/state/dc_charge_parameter_discovery.hpp>
#include <eviso15118/d20/state/schedule_exchange.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/d20/state/dc_charge_parameter_discovery.hpp>
#include <eviso15118/detail/d20/state/session_stop.hpp>
#include <eviso15118/detail/helper.hpp>

namespace eviso15118::d20::state {

using DC_ModeRes = message_20::DC_ChargeParameterDiscoveryResponse::DC_CPDResEnergyTransferMode;
using BPT_DC_ModeRes = message_20::DC_ChargeParameterDiscoveryResponse::BPT_DC_CPDResEnergyTransferMode;

using DC_ModeReq = message_20::DC_ChargeParameterDiscoveryRequest::DC_CPDReqEnergyTransferMode;
using BPT_DC_ModeReq = message_20::DC_ChargeParameterDiscoveryRequest::BPT_DC_CPDReqEnergyTransferMode;


//RDB - setup the request message to avoid duplication
message_20::DC_ChargeParameterDiscoveryRequest DC_ChargeParameterDiscovery::setup_request(const d20::Session &session, const d20::SessionConfig& config)
{
    message_20::DC_ChargeParameterDiscoveryRequest req;
    setup_header(req.header,session);
    message_20::RequestCode request_code  = message_20::RequestCode::OK;

    //RDB TODO this could be done better
    auto& mode = req.transfer_mode.emplace<DC_ModeReq>();
    mode.max_charge_current = config.evse_dc_parameter.max_charge_current;
    mode.max_charge_power=config.evse_dc_parameter.max_charge_power;
    mode.max_voltage=config.evse_dc_parameter.max_voltage;
    mode.min_charge_current = config.evse_dc_parameter.min_charge_current;
    mode.min_charge_power=config.evse_dc_parameter.min_charge_power;
    mode.min_voltage=config.evse_dc_parameter.min_voltage;
    mode.target_soc=100;

    return request_with_code(req, request_code);
}


void DC_ChargeParameterDiscovery::enter() {
    ctx.log.enter_state("DC_ChargeParameterDiscovery");
    //Prepare and send the request
    const auto req = DC_ChargeParameterDiscovery::setup_request(ctx.session, ctx.config);
    ctx.request(req);

}

FsmSimpleState::HandleEventReturnType DC_ChargeParameterDiscovery::handle_event(AllocatorType& sa, FsmEvent ev) {
    if (ev != FsmEvent::V2GTP_MESSAGE) {
        return sa.PASS_ON;
    }

    const auto variant = ctx.get_response();

    if (const auto res = variant->get_if<message_20::DC_ChargeParameterDiscoveryResponse>()) {
        message_20::RationalNumber max_current;

        if (std::holds_alternative<DC_ModeRes>(res->transfer_mode)) {

            max_current = std::get<0>(res->transfer_mode).max_charge_current;

        } else if (std::holds_alternative<BPT_DC_ModeRes>(res->transfer_mode)) {

            max_current = std::get<1>(res->transfer_mode).max_charge_current;

            const auto& max_discharge_current = std::get<1>(res->transfer_mode).max_discharge_current;

            logf("Max discharge current %de%d\n", max_discharge_current.value, max_discharge_current.exponent);
        }

        logf("Max charge current %de%d\n", max_current.value, max_current.exponent);


        if (res->response_code >= message_20::ResponseCode::FAILED) {
            ctx.session_stopped = true;
            return sa.PASS_ON;
        }

        return sa.create_simple<ScheduleExchange>(ctx);
        

    } else if (const auto res = variant->get_if<message_20::SessionStopResponse>()) {

        ctx.session_stopped = true;

        return sa.PASS_ON;
    } else {
        ctx.log("expected DC_ChargeParameterDiscovery! But code type id: %d", variant->get_type());
        ctx.session_stopped = true;

        // Sequence Error
        const message_20::Type res_type = variant->get_type();
        send_sequence_error(res_type, ctx);

        return sa.PASS_ON;
    }
}

} // namespace eviso15118::d20::state
