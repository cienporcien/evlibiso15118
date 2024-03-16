// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/dc_welding_detection.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_DC_Decoder.h>
#include <exi/cb/iso20_DC_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_dc_DC_WeldingDetectionReqType& in, DC_WeldingDetectionRequest& out) {
    convert(in.Header, out.header);

    cb_convert_enum(in.EVProcessing, out.processing);
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_dc_DC_WeldingDetectionResType& in, DC_WeldingDetectionResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);
    convert(in.EVSEPresentVoltage, out.present_voltage);
    
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const DC_WeldingDetectionRequest& in, iso20_dc_DC_WeldingDetectionReqType& out) {
    init_iso20_dc_DC_WeldingDetectionReqType(&out);

    //RDB TODO Handle the various options in the request

    convert(in.header, out.Header);

}


template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_WeldingDetectionReqType& in) {
    va.insert_type<DC_WeldingDetectionRequest>(in);
}

//RBL handle the response
template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_WeldingDetectionResType& in) {
    va.insert_type<DC_WeldingDetectionResponse>(in);
};

template <> void convert(const DC_WeldingDetectionResponse& in, struct iso20_dc_DC_WeldingDetectionResType& out) {
    init_iso20_dc_DC_WeldingDetectionResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    convert(in.present_voltage, out.EVSEPresentVoltage);
}

template <> int serialize_to_exi(const DC_WeldingDetectionResponse& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_WeldingDetectionRes);

    convert(in, doc.DC_WeldingDetectionRes);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const DC_WeldingDetectionRequest& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_WeldingDetectionReq);

    convert(in, doc.DC_WeldingDetectionReq);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_WeldingDetectionResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const DC_WeldingDetectionRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}


} // namespace eviso15118::message_20