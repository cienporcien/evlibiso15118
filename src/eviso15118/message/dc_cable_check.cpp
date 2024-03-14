// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/dc_cable_check.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_DC_Decoder.h>
#include <exi/cb/iso20_DC_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_dc_DC_CableCheckReqType& in, DC_CableCheckRequest& out) {
    convert(in.Header, out.header);
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_dc_DC_CableCheckResType& in, DC_CableCheckResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);
    cb_convert_enum(in.EVSEProcessing, out.processing);
    
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const DC_CableCheckRequest& in, iso20_dc_DC_CableCheckReqType& out) {
    init_iso20_dc_DC_CableCheckReqType(&out);

    //RDB TODO Handle the various options in the request

    convert(in.header, out.Header);

}

template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_CableCheckReqType& in) {
    va.insert_type<DC_CableCheckRequest>(in);
}

//RBL handle the response
template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_CableCheckResType& in) {
    va.insert_type<DC_CableCheckResponse>(in);
};

template <> void convert(const DC_CableCheckResponse& in, struct iso20_dc_DC_CableCheckResType& out) {
    init_iso20_dc_DC_CableCheckResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
    cb_convert_enum(in.processing, out.EVSEProcessing);
}

template <> int serialize_to_exi(const DC_CableCheckResponse& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_CableCheckRes);

    convert(in, doc.DC_CableCheckRes);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const DC_CableCheckRequest& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_CableCheckReq);

    convert(in, doc.DC_CableCheckReq);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_CableCheckResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const DC_CableCheckRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace eviso15118::message_20
