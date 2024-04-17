// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../fsm.hpp"
#include <eviso15118/message/acdp_vehicle_positioning.hpp>

namespace eviso15118::d20::state {

struct ACDP_VehiclePositioning : public FsmSimpleState {
    using FsmSimpleState::FsmSimpleState;

    void enter() final;

    HandleEventReturnType handle_event(AllocatorType&, FsmEvent) final;

    message_20::ACDP_VehiclePositioningRequest setup_request(const d20::Session &session, const bool ev_mobility_status);

private:
    bool vehicle_positioning_initiated{false};
    bool vehicle_positioning_done{false};
};

} // namespace eviso15118::d20::state
