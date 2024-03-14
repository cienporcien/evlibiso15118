// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/dc_pre_charge.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_DC_Decoder.h>
#include <exi/cb/iso20_DC_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_dc_DC_PreChargeReqType& in, DC_PreChargeRequest& out) {
    convert(in.Header, out.header);

    cb_convert_enum(in.EVProcessing, out.processing);
    convert(in.EVPresentVoltage, out.present_voltage);
    convert(in.EVTargetVoltage, out.target_voltage);
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_dc_DC_PreChargeResType& in, DC_PreChargeResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);
    convert(in.EVSEPresentVoltage, out.present_voltage);
    
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const DC_PreChargeRequest& in, iso20_dc_DC_PreChargeReqType& out) {
    init_iso20_dc_DC_PreChargeReqType(&out);

    //RDB TODO Handle the various options in the request

    convert(in.header, out.Header);

}


template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_PreChargeReqType& in) {
    va.insert_type<DC_PreChargeRequest>(in);
}

//RBL handle the response
template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_PreChargeResType& in) {
    va.insert_type<DC_PreChargeResponse>(in);
};


template <> void convert(const DC_PreChargeResponse& in, struct iso20_dc_DC_PreChargeResType& out) {
    init_iso20_dc_DC_PreChargeResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    convert(in.present_voltage, out.EVSEPresentVoltage);
}

template <> int serialize_to_exi(const DC_PreChargeResponse& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_PreChargeRes);

    convert(in, doc.DC_PreChargeRes);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const DC_PreChargeRequest& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_PreChargeReq);

    convert(in, doc.DC_PreChargeReq);

    return encode_iso20_dc_exiDocument(&out, &doc);
}


template <> size_t serialize(const DC_PreChargeResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const DC_PreChargeRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}


} // namespace eviso15118::message_20
