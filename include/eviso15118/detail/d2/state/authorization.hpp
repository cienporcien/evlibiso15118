// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d2/session.hpp>
#include <eviso15118/message_d2/authorization.hpp>

namespace eviso15118::d2::state {

message_2::AuthorizationResponse handle_request(const message_2::AuthorizationRequest& req,
                                                 const d2::Session& session,
                                                 const message_2::AuthStatus& authorization_status);

} // namespace eviso15118::d2::state
