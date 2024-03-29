// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include <eviso15118/session_d2/feedback.hpp>

#include <eviso15118/detail/helper.hpp>

namespace eviso15118::session_2 {

Feedback::Feedback(feedback::Callbacks callbacks_) : callbacks(std::move(callbacks_)) {
}

void Feedback::signal(feedback::Signal signal) const {
    call_if_available(callbacks.signal, signal);
}

void Feedback::dc_charge_target(const feedback::DcChargeTarget& charge_target) const {
    call_if_available(callbacks.dc_charge_target, charge_target);
}

void Feedback::dc_max_limits(const feedback::DcMaximumLimits& max_limits) const {
    call_if_available(callbacks.dc_max_limits, max_limits);
}

} // namespace eviso15118::session

