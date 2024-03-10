// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <ctime>

#include <eviso15118/detail/d20/context_helper.hpp>
#include <eviso15118/detail/helper.hpp>

// #include <eviso15118/message/authorization.hpp>
// #include <eviso15118/message/authorization_setup.hpp>
// #include <eviso15118/message/dc_cable_check.hpp>
// #include <eviso15118/message/dc_charge_loop.hpp>
// #include <eviso15118/message/dc_charge_parameter_discovery.hpp>
// #include <eviso15118/message/dc_pre_charge.hpp>
// #include <eviso15118/message/dc_welding_detection.hpp>
// #include <eviso15118/message/power_delivery.hpp>
// #include <eviso15118/message/schedule_exchange.hpp>
// #include <eviso15118/message/service_detail.hpp>
// #include <eviso15118/message/service_discovery.hpp>
// #include <eviso15118/message/service_selection.hpp>
// #include <eviso15118/message/session_setup.hpp>
// #include <eviso15118/message/session_stop.hpp>

namespace eviso15118::d20 {

static inline void setup_timestamp(message_20::Header& header) {
    header.timestamp = static_cast<uint64_t>(std::time(nullptr));
}

bool validate_and_setup_header(message_20::Header& header, const Session& cur_session,
                               const decltype(message_20::Header::session_id)& res_session_id) {

    setup_header(header, cur_session);

    return (cur_session.get_id() == res_session_id);
}

void setup_header(message_20::Header& header, const Session& cur_session) {
    header.session_id = cur_session.get_id();
    setup_timestamp(header);
}

template <typename Request> Request handle_sequence_error(const d20::Session& session) {
    Request req;
    setup_header(req.header, session);
    return request_with_code(req, message_20::ResponseCode::FAILED_SequenceError);
}

// Todo(sl): Not happy at all. Need refactoring. Only ctx.respond and Session is needed. Not the whole Context.
void send_sequence_error(const message_20::Type res_type, d20::Context& ctx) {

    if (res_type == message_20::Type::SessionSetupRes) {
        //const auto req = handle_sequence_error<message_20::SessionSetupRequest>(ctx.session);
        //ctx.respond(req);
    // } else if (req_type == message_20::Type::AuthorizationSetupReq) {
    //     const auto res = handle_sequence_error<message_20::AuthorizationSetupResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::AuthorizationReq) {
    //     const auto res = handle_sequence_error<message_20::AuthorizationResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::ServiceDiscoveryReq) {
    //     const auto res = handle_sequence_error<message_20::ServiceDiscoveryResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::ServiceDetailReq) {
    //     const auto res = handle_sequence_error<message_20::ServiceDetailResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::ServiceSelectionReq) {
    //     const auto res = handle_sequence_error<message_20::ServiceSelectionResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::DC_ChargeParameterDiscoveryReq) {
    //     const auto res = handle_sequence_error<message_20::DC_ChargeParameterDiscoveryResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::ScheduleExchangeReq) {
    //     const auto res = handle_sequence_error<message_20::ScheduleExchangeResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::DC_CableCheckReq) {
    //     const auto res = handle_sequence_error<message_20::DC_CableCheckResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::PowerDeliveryReq) {
    //     const auto res = handle_sequence_error<message_20::PowerDeliveryResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::DC_PreChargeReq) {
    //     const auto res = handle_sequence_error<message_20::DC_PreChargeResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::DC_ChargeLoopReq) {
    //     const auto res = handle_sequence_error<message_20::DC_ChargeLoopResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::DC_WeldingDetectionReq) {
    //     const auto res = handle_sequence_error<message_20::DC_WeldingDetectionResponse>(ctx.session);
    //     ctx.respond(res);
    // } else if (req_type == message_20::Type::SessionStopReq) {
    //     const auto res = handle_sequence_error<message_20::SessionStopResponse>(ctx.session);
    //     ctx.respond(res);
    } else {
        logf("Unknown code type id: %d ", res_type);
    }
}

} // namespace eviso15118::d20
