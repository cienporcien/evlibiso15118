// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/variant.hpp>

#include <cassert>
#include <string>

#include <eviso15118/detail/helper.hpp>
#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/appHand_Decoder.h>
#include <exi/cb/iso20_CommonMessages_Decoder.h>
#include <exi/cb/iso20_DC_Decoder.h>

using PayloadType = eviso15118::io::v2gtp::PayloadType;

namespace eviso15118::message_20 {

static void handle_sap(VariantAccess& va) {
    appHand_exiDocument doc;

    const auto decode_status = decode_appHand_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_appHand_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.supportedAppProtocolRes_isUsed) {
        insert_type(va, doc.supportedAppProtocolRes);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_main(VariantAccess& va) {
    iso20_exiDocument doc;

    const auto decode_status = decode_iso20_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.SessionSetupRes_isUsed) {
        insert_type(va, doc.SessionSetupRes);
     } else if (doc.AuthorizationSetupRes_isUsed) {
        insert_type(va, doc.AuthorizationSetupRes);
    // } else if (doc.AuthorizationRes_isUsed) {
    //     insert_type(va, doc.AuthorizationRes);
    // } else if (doc.ServiceDiscoveryRes_isUsed) {
    //     insert_type(va, doc.ServiceDiscoveryRes);
    // } else if (doc.ServiceDetailRes_isUsed) {
    //     insert_type(va, doc.ServiceDetailRes);
    // } else if (doc.ServiceSelectionRes_isUsed) {
    //     insert_type(va, doc.ServiceSelectionRes);
    // } else if (doc.ScheduleExchangeRes_isUsed) {
    //     insert_type(va, doc.ScheduleExchangeRes);
    // } else if (doc.PowerDeliveryRes_isUsed) {
    //     insert_type(va, doc.PowerDeliveryRes);
    // } else if (doc.SessionStopRes_isUsed) {
    //     insert_type(va, doc.SessionStopRes);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_dc(VariantAccess& va) {
    iso20_dc_exiDocument doc;

    const auto decode_status = decode_iso20_dc_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_dc_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.DC_ChargeParameterDiscoveryRes_isUsed) {
    //     insert_type(va, doc.DC_ChargeParameterDiscoveryRes);
    // } else if (doc.DC_CableCheckRes_isUsed) {
    //     insert_type(va, doc.DC_CableCheckRes);
    // } else if (doc.DC_PreChargeRes_isUsed) {
    //     insert_type(va, doc.DC_PreChargeRes);
    // } else if (doc.DC_ChargeLoopRes_isUsed) {
    //     insert_type(va, doc.DC_ChargeLoopRes);
    // } else if (doc.DC_WeldingDetectionRes_isUsed) {
    //     insert_type(va, doc.DC_WeldingDetectionRes);
    } else {
        va.error = "chosen message type unhandled";
    }
}

Variant::Variant(io::v2gtp::PayloadType payload_type, const io::StreamInputView& buffer_view) {

    VariantAccess va{
        get_exi_input_stream(buffer_view), this->data, this->type, this->custom_deleter, this->error,
    };

    if (payload_type == PayloadType::SAP) {
        handle_sap(va);
    } else if (payload_type == PayloadType::Part20Main) {
        handle_main(va);
    } else if (payload_type == PayloadType::Part20DC) {
        handle_dc(va);
    } else {
        logf("Unknown type\n");
    }

    if (data) {
        // in case data was set, make sure the custom deleter and the type were set!
        assert(custom_deleter != nullptr);
        assert(type != Type::None);
    } else {
        logf("Failed due to: %s\n", error.c_str());
    }
}

Variant::~Variant() {
    if (data) {
        custom_deleter(data);
    }
}

Type Variant::get_type() const {
    return type;
}

const std::string& Variant::get_error() const {
    return error;
}

} // namespace eviso15118::message_20
