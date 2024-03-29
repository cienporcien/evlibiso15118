// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../fsm.hpp"

namespace eviso15118::d20::state {

struct DC_ChargeParameterDiscovery : public FsmSimpleState {
    using FsmSimpleState::FsmSimpleState;

    void enter() final;

    HandleEventReturnType handle_event(AllocatorType&, FsmEvent) final;
    message_20::DC_ChargeParameterDiscoveryRequest setup_request(const d20::Session &session, const d20::SessionConfig& config);
};

} // namespace eviso15118::d20::state
