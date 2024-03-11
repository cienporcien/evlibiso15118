// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../fsm.hpp"
#include <eviso15118/message/authorization.hpp>

namespace eviso15118::d20::state {

struct Authorization : public FsmSimpleState {
    using FsmSimpleState::FsmSimpleState;

    void enter() final;

    HandleEventReturnType handle_event(AllocatorType&, FsmEvent) final;
    message_20::AuthorizationRequest setup_request(const d20::Session &session);

private:
    message_20::AuthStatus authorization_status{message_20::AuthStatus::Pending};
};

} // namespace eviso15118::d20::state
