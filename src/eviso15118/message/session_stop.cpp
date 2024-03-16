// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/session_stop.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_CommonMessages_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_SessionStopReqType& in, SessionStopRequest& out) {
    convert(in.Header, out.header);

    cb_convert_enum(in.ChargingSession, out.charging_session);
    if (in.EVTerminationCode_isUsed) {
        out.ev_termination_code = CB2CPP_STRING(in.EVTerminationCode);
    }
    if (in.EVTerminationExplanation_isUsed) {
        out.ev_termination_explanation = CB2CPP_STRING(in.EVTerminationExplanation);
    }
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_SessionStopResType& in, SessionStopResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);
    
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const SessionStopRequest& in, iso20_SessionStopReqType& out) {
    init_iso20_SessionStopReqType(&out);

    //RDB TODO Handle the various options in the request

    convert(in.header, out.Header);

}

template <> void insert_type(VariantAccess& va, const struct iso20_SessionStopReqType& in) {
    va.insert_type<SessionStopRequest>(in);
}

//RBL handle the response
template <> void insert_type(VariantAccess& va, const struct iso20_SessionStopResType& in) {
    va.insert_type<SessionStopResponse>(in);
};

template <> void convert(const SessionStopResponse& in, struct iso20_SessionStopResType& out) {
    init_iso20_SessionStopResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
}

template <> int serialize_to_exi(const SessionStopResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.SessionStopRes);

    convert(in, doc.SessionStopRes);

    return encode_iso20_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const SessionStopRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.SessionStopReq);

    convert(in, doc.SessionStopReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const SessionStopResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const SessionStopRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace eviso15118::message_20