// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../fsm.hpp"

namespace eviso15118::d20::state {

struct DC_WeldingDetection : public FsmSimpleState {
    using FsmSimpleState::FsmSimpleState;

    void enter() final;

    HandleEventReturnType handle_event(AllocatorType&, FsmEvent) final;
    message_20::DC_WeldingDetectionRequest setup_request(const d20::Session &session);

private:
    float present_voltage{0};
};

} // namespace eviso15118::d20::state
