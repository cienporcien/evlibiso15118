// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <ctime>

#include <eviso15118/detail/d2/context_helper.hpp>
#include <eviso15118/detail/helper.hpp>

#include <eviso15118/message_d2/authorization.hpp>
#include <eviso15118/message_d2/authorization_setup.hpp>
#include <eviso15118/message_d2/dc_cable_check.hpp>
#include <eviso15118/message_d2/dc_charge_loop.hpp>
#include <eviso15118/message_d2/dc_charge_parameter_discovery.hpp>
#include <eviso15118/message_d2/dc_pre_charge.hpp>
#include <eviso15118/message_d2/dc_welding_detection.hpp>
#include <eviso15118/message_d2/power_delivery.hpp>
#include <eviso15118/message_d2/schedule_exchange.hpp>
#include <eviso15118/message_d2/service_detail.hpp>
#include <eviso15118/message_d2/service_discovery.hpp>
#include <eviso15118/message_d2/service_selection.hpp>
#include <eviso15118/message_d2/session_setup.hpp>
#include <eviso15118/message_d2/session_stop.hpp>

namespace eviso15118::d2 {

static inline void setup_timestamp(message_2::Header& header) {
    header.timestamp = static_cast<uint64_t>(std::time(nullptr));
}

bool validate_and_setup_header(message_2::Header& header, const Session& cur_session,
                               const decltype(message_2::Header::session_id)& req_session_id) {

    setup_header(header, cur_session);

    return (cur_session.get_id() == req_session_id);
}

void setup_header(message_2::Header& header, const Session& cur_session) {
    header.session_id = cur_session.get_id();
    setup_timestamp(header);
}

template <typename Response> Response handle_sequence_error(const d2::Session& session) {
    Response res;
    setup_header(res.header, session);
    return response_with_code(res, message_2::ResponseCode::FAILED_SequenceError);
}

// Todo(sl): Not happy at all. Need refactoring. Only ctx.respond and Session is needed. Not the whole Context.
void send_sequence_error(const message_2::Type req_type, d2::Context& ctx) {

    if (req_type == message_2::Type::SessionSetupReq) {
        const auto res = handle_sequence_error<message_2::SessionSetupResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::AuthorizationSetupReq) {
        const auto res = handle_sequence_error<message_2::AuthorizationSetupResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::AuthorizationReq) {
        const auto res = handle_sequence_error<message_2::AuthorizationResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::ServiceDiscoveryReq) {
        const auto res = handle_sequence_error<message_2::ServiceDiscoveryResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::ServiceDetailReq) {
        const auto res = handle_sequence_error<message_2::ServiceDetailResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::ServiceSelectionReq) {
        const auto res = handle_sequence_error<message_2::ServiceSelectionResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::DC_ChargeParameterDiscoveryReq) {
        const auto res = handle_sequence_error<message_2::DC_ChargeParameterDiscoveryResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::ScheduleExchangeReq) {
        const auto res = handle_sequence_error<message_2::ScheduleExchangeResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::DC_CableCheckReq) {
        const auto res = handle_sequence_error<message_2::DC_CableCheckResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::PowerDeliveryReq) {
        const auto res = handle_sequence_error<message_2::PowerDeliveryResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::DC_PreChargeReq) {
        const auto res = handle_sequence_error<message_2::DC_PreChargeResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::DC_ChargeLoopReq) {
        const auto res = handle_sequence_error<message_2::DC_ChargeLoopResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::DC_WeldingDetectionReq) {
        const auto res = handle_sequence_error<message_2::DC_WeldingDetectionResponse>(ctx.session);
        ctx.respond(res);
    } else if (req_type == message_2::Type::SessionStopReq) {
        const auto res = handle_sequence_error<message_2::SessionStopResponse>(ctx.session);
        ctx.respond(res);
    } else {
        logf("Unknown code type id: %d ", req_type);
    }
}

} // namespace eviso15118::d2
