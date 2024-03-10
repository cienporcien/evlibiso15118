// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d20/context.hpp>
#include <eviso15118/d20/session.hpp>
#include <eviso15118/message/common.hpp>

namespace eviso15118::d20 {

// FIXME (aw): not sure about correct signature here for RVO
template <typename Request, typename RequestCode> Request& request_with_code(Request& req, RequestCode code) {
    // FIXME (aw): could add an static_assert here that ResponseCode is an enum?
    //req.request_code = code;
    return req;
}

bool validate_and_setup_header(message_20::Header&, const Session&, const decltype(message_20::Header::session_id)&);

void setup_header(message_20::Header&, const Session&);

void send_sequence_error(const message_20::Type, d20::Context&);

} // namespace eviso15118::d20
