// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d20/session.hpp>
#include <eviso15118/message/authorization.hpp>

namespace eviso15118::d20::state {

message_20::AuthorizationResponse handle_request(const message_20::AuthorizationRequest& req,
                                                 const d20::Session& session,
                                                 const message_20::AuthStatus& authorization_status);

} // namespace eviso15118::d20::state
