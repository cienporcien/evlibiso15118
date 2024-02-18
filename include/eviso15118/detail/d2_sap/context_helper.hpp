// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d2_sap/context.hpp>
#include <eviso15118/d2_sap/session.hpp>
#include <eviso15118/message_d2_sap/common.hpp>

namespace eviso15118::d2_sap {

// FIXME (aw): not sure about correct signature here for RVO
template <typename Response, typename ResponseCode> Response& response_with_code(Response& res, ResponseCode code) {
    // FIXME (aw): could add an static_assert here that ResponseCode is an enum?
    res.response_code = code;
    return res;
}

bool validate_and_setup_header(message_2_sap::Header&, const Session&, const decltype(message_2_sap::Header::session_id)&);

void setup_header(message_2_sap::Header&, const Session&);

void send_sequence_error(const message_2_sap::Type, d2_sap::Context&);

} // namespace eviso15118::d2_sap
