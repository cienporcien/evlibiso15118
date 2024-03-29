// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>

namespace eviso15118::message_2 {

const uint8_t SESSION_ID_LENGTH = 8;

struct Header {
    std::array<uint8_t, SESSION_ID_LENGTH> session_id{};
    uint64_t timestamp;
    // missing signature
};

template <typename cb_HeaderType> void convert(const cb_HeaderType& in, Header& out);

enum class ResponseCode {
//RBL This needs to match the ISO2 Exigen Enum, not the ISO20 one.
    OK = 0,
    OK_NewSessionEstablished = 1,
    OK_OldSessionJoined = 2,
    OK_CertificateExpiresSoon = 3,
    FAILED = 4,
    FAILED_SequenceError = 5,
    FAILED_ServiceIDInvalid = 6,
    FAILED_UnknownSession = 7,
    FAILED_ServiceSelectionInvalid = 8,
    FAILED_PaymentSelectionInvalid = 9,
    FAILED_CertificateExpired = 10,
    FAILED_SignatureError = 11,
    FAILED_NoCertificateAvailable = 12,
    FAILED_CertChainError = 13,
    FAILED_ChallengeInvalid = 14,
    FAILED_ContractCanceled = 15,
    FAILED_WrongChargeParameter = 16,
    FAILED_PowerDeliveryNotApplied = 17,
    FAILED_TariffSelectionInvalid = 18,
    FAILED_ChargingProfileInvalid = 19,
    FAILED_MeteringSignatureNotValid = 20,
    FAILED_NoChargeServiceSelected = 21,
    FAILED_WrongEnergyTransferMode = 22,
    FAILED_ContactorError = 23,
    FAILED_CertificateNotAllowedAtThisEVSE = 24,
    FAILED_CertificateRevoked = 25
};

enum class Processing {
    Finished = 0,
    Ongoing = 1,
    Ongoing_WaitingForCustomerInteraction = 2,
};

enum class Authorization {
    EIM = 0,
    PnC = 1,
};

enum class ServiceCategory : uint16_t {
    AC = 1,
    DC = 2,
    WPT = 3,
    DC_ACDP = 4,
    AC_BPT = 5,
    DC_BPT = 6,
    DC_ACDP_BPT = 7,
    Internet = 65,
    ParkingStatus = 66,
};

enum class EvseNotification {
    Pause,
    ExitStandby,
    Terminate,
    ScheduleRenegotiation,
    ServiceRenegotiation,
    MeteringConfirmation,
};

enum class ChargingSession {
    Pause,
    Terminate,
    ServiceRenegotiation,
};

enum class AcConnector {
    SinglePhase = 1,
    ThreePhase = 3,
};
enum class DcConnector {
    Core = 1,
    Extended = 2,
    Dual2 = 3,
    Dual4 = 4,
};
enum class ControlMode {
    Scheduled = 1,
    Dynamic = 2,
};
enum class MobilityNeedsMode {
    ProvidedByEvcc = 1,
    ProvidedBySecc = 2,
};
enum class Pricing {
    NoPricing = 0,
    AbsolutePricing = 1,
    PriceLevels = 2,
};

enum class BptChannel {
    Unified = 1,
    Separated = 2,
};

enum class GeneratorMode {
    GridFollowing = 1,
    GridForming = 2,
};

enum class Protocol {
    Ftp,
    Http,
    Https,
};

enum class Port {
    Port20 = 20,
    Port21 = 21,
    Port80 = 80,
    Port443 = 443,
};

enum class IntendedService {
    VehicleCheckIn = 1,
    VehicleCheckOut = 2,
};

enum class ParkingStatus {
    AutoInternal = 1,
    AutoExternal = 2,
    ManualInternal = 3,
    ManualExternal = 4,
};

struct RationalNumber {
    int16_t value{0};
    int8_t exponent{0};
};

struct EvseStatus {
    uint16_t notification_max_delay;
    EvseNotification notification;
};

struct AcParameterList {
    AcConnector connector;
    ControlMode control_mode;
    MobilityNeedsMode mobility_needs_mode;
    uint32_t evse_nominal_voltage;
    Pricing pricing;
};

struct AcBptParameterList : AcParameterList {
    enum class GridCodeIslandingDetectionMethode {
        Active = 1,
        Passive = 2,
    };
    BptChannel bpt_channel;
    GeneratorMode generator_mode;
    GridCodeIslandingDetectionMethode grid_code_detection_methode;
};

struct DcParameterList {
    DcConnector connector;
    ControlMode control_mode;
    MobilityNeedsMode mobility_needs_mode;
    Pricing pricing;
};

struct DcBptParameterList : DcParameterList {
    BptChannel bpt_channel;
    GeneratorMode generator_mode;
};

struct InternetParameterList {
    Protocol protocol;
    Port port;
};

struct ParkingParameterList {
    IntendedService intended_service;
    ParkingStatus parking_status;
};

struct Scheduled_CLReqControlMode {
    std::optional<RationalNumber> target_energy_request;
    std::optional<RationalNumber> max_energy_request;
    std::optional<RationalNumber> min_energy_request;
};

struct Dynamic_CLReqControlMode {
    std::optional<uint32_t> departure_time;
    RationalNumber target_energy_request;
    RationalNumber max_energy_request;
    RationalNumber min_energy_request;
};

struct Scheduled_CLResControlMode {};

struct Dynamic_CLResControlMode {
    std::optional<uint32_t> departure_time;
    std::optional<uint8_t> minimum_soc;
    std::optional<uint8_t> target_soc;
    std::optional<uint16_t> ack_max_delay;
};

struct MeterInfo {
    std::string meter_id;
    uint64_t charged_energy_reading_wh;
    std::optional<uint64_t> bpt_discharged_energy_reading_wh;
    std::optional<uint64_t> capacitive_energy_reading_varh;
    std::optional<uint64_t> bpt_inductive_energery_reading_varh;
    std::optional<std::string> meter_signature;
    std::optional<int16_t> meter_status;
    std::optional<uint64_t> meter_timestamp;
};

using PercentValue = uint8_t;

using NumericID = uint32_t; // [1 - 4294967295]

template <typename cb_RationalNumberType> void convert(const cb_RationalNumberType& in, RationalNumber& out);
template <typename cb_RationalNumberType> void convert(const RationalNumber& in, cb_RationalNumberType& out);

float from_RationalNumber(const RationalNumber& in);
RationalNumber from_float(float in);

std::string from_Protocol(const Protocol& in);

} // namespace eviso15118::message_2
