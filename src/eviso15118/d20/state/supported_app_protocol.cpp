// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/d20/state/supported_app_protocol.hpp>

#include <eviso15118/d20/state/session_setup.hpp>

#include <eviso15118/message/supported_app_protocol.hpp>

#include <eviso15118/detail/d20/context_helper.hpp>

namespace eviso15118::d20::state {

//RDB on the EV side, we are getting back the response from the EVSE which is simply the
//response code and SchemaID. This will determine which schema that will be used by the EV
//from now on.
static auto handle_response(const message_20::SupportedAppProtocolResponse& res) {
    message_20::SupportedAppProtocolRequest req;

    if(res.schema_id == 2)
    {
        //TODO RDB For now, we just use 15118-20. Later add the split.
    }

    return true;
}

void SupportedAppProtocol::enter() {
    ctx.log.enter_state("SupportedAppProtocol");



}

FsmSimpleState::HandleEventReturnType SupportedAppProtocol::handle_event(AllocatorType &sa, FsmEvent ev)
{

    if (ev == FsmEvent::CONTROL_MESSAGE)
    {
        const auto control_data = ctx.get_control_event<StartStopCharging>();
        if (not control_data)
        {
            // FIXME (aw): error handling
            return sa.HANDLED_INTERNALLY;
        }

        //TODO RDB handle the different possibilities of StartStopCharging
        if (*control_data)
        {
            // authorization_status = AuthStatus::Accepted;
        }
        else
        {
            // authorization_status = AuthStatus::Rejected;
        }

        // RDB here we can send the SAP request in response to the incoming StartStopCharging message from the vehicle ECU.
        //  Then, when the response arrives back from the EVCC, we can react
        // and move on to the next state.
        // RDB TODO We should use the supported protocols from the module config.
        message_20::SupportedAppProtocolRequest req;

        auto &item_out2 = req.app_protocol.emplace_back();
        item_out2.protocol_namespace = "urn:iso:std:iso:15118:-20:DC";
        item_out2.version_number_major = 1;
        item_out2.version_number_minor = 0;
        item_out2.schema_id = 2;
        item_out2.priority = 2;

        auto &item_out = req.app_protocol.emplace_back();
        item_out.protocol_namespace = "urn:iso:15118:2:2013:MsgDef";
        item_out.version_number_major = 2;
        item_out.version_number_minor = 0;
        item_out.schema_id = 1;
        item_out.priority = 1;

        // Send it.
        ctx.request(req);

        return sa.HANDLED_INTERNALLY;
    }

    if (ev == FsmEvent::V2GTP_MESSAGE)
    {
        auto variant = ctx.get_response();
        if (variant->get_type() != message_20::Type::SupportedAppProtocolRes)
        {
            ctx.log("expected SupportedAppProtocolRes!");
            return sa.PASS_ON;
        }

        const auto res = handle_response(variant->get<message_20::SupportedAppProtocolResponse>());

        // Here we go into the SessionSetup state, and then send out the SessionSetupReq message.
        // to avoid any sort of timing issue. So, the message will be sent in the void SessionSetup::enter()
        // instead of here.

        // RDB go to SessionSetup.
        return sa.create_simple<SessionSetup>(ctx);
    }

    return sa.PASS_ON;
}

} // namespace eviso15118::d20::state
