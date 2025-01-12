/* MIT License
 *
 * Copyright (c) 2023 Brad House
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * SPDX-License-Identifier: MIT
 */
#include "ares_setup.h"
#include "ares.h"
#include "ares_private.h"

ares_bool_t ares_dns_opcode_isvalid(ares_dns_opcode_t opcode)
{
  switch (opcode) {
    case ARES_OPCODE_QUERY:
    case ARES_OPCODE_IQUERY:
    case ARES_OPCODE_STATUS:
    case ARES_OPCODE_NOTIFY:
    case ARES_OPCODE_UPDATE:
      return ARES_TRUE;
  }
  return ARES_FALSE;
}

ares_bool_t ares_dns_rcode_isvalid(ares_dns_rcode_t rcode)
{
  switch (rcode) {
    case ARES_RCODE_NOERROR:
    case ARES_RCODE_FORMAT_ERROR:
    case ARES_RCODE_SERVER_FAILURE:
    case ARES_RCODE_NAME_ERROR:
    case ARES_RCODE_NOT_IMPLEMENTED:
    case ARES_RCODE_REFUSED:
    case ARES_RCODE_YXDOMAIN:
    case ARES_RCODE_YXRRSET:
    case ARES_RCODE_NXRRSET:
    case ARES_RCODE_NOTAUTH:
    case ARES_RCODE_NOTZONE:
      return ARES_TRUE;
  }
  return ARES_FALSE;
}

ares_bool_t ares_dns_flags_arevalid(unsigned short flags)
{
  unsigned short allflags = ARES_FLAG_QR | ARES_FLAG_AA | ARES_FLAG_TC |
                            ARES_FLAG_RD | ARES_FLAG_RA | ARES_FLAG_AD |
                            ARES_FLAG_CD;

  if (flags & ~allflags) {
    return ARES_FALSE;
  }

  return ARES_TRUE;
}

ares_bool_t ares_dns_rec_type_isvalid(ares_dns_rec_type_t type,
                                      ares_bool_t         is_query)
{
  switch (type) {
    case ARES_REC_TYPE_A:
    case ARES_REC_TYPE_NS:
    case ARES_REC_TYPE_CNAME:
    case ARES_REC_TYPE_SOA:
    case ARES_REC_TYPE_PTR:
    case ARES_REC_TYPE_HINFO:
    case ARES_REC_TYPE_MX:
    case ARES_REC_TYPE_TXT:
    case ARES_REC_TYPE_AAAA:
    case ARES_REC_TYPE_SRV:
    case ARES_REC_TYPE_NAPTR:
    case ARES_REC_TYPE_OPT:
    case ARES_REC_TYPE_TLSA:
    case ARES_REC_TYPE_SVCB:
    case ARES_REC_TYPE_HTTPS:
    case ARES_REC_TYPE_ANY:
    case ARES_REC_TYPE_URI:
    case ARES_REC_TYPE_CAA:
      return ARES_TRUE;
    case ARES_REC_TYPE_RAW_RR:
      return is_query ? ARES_FALSE : ARES_TRUE;
    default:
      break;
  }
  return is_query ? ARES_TRUE : ARES_FALSE;
}

ares_bool_t ares_dns_rec_type_allow_name_compression(ares_dns_rec_type_t type)
{
  /* Only record types defined in RFC1035 allow name compression within the
   * RDATA.  Otherwise nameservers that don't understand an RR may not be
   * able to pass along the RR in a proper manner */
  switch (type) {
    case ARES_REC_TYPE_A:
    case ARES_REC_TYPE_NS:
    case ARES_REC_TYPE_CNAME:
    case ARES_REC_TYPE_SOA:
    case ARES_REC_TYPE_PTR:
    case ARES_REC_TYPE_HINFO:
    case ARES_REC_TYPE_MX:
    case ARES_REC_TYPE_TXT:
      return ARES_TRUE;
    default:
      break;
  }
  return ARES_FALSE;
}

ares_bool_t ares_dns_class_isvalid(ares_dns_class_t qclass,
                                   ares_bool_t      is_query)
{
  switch (qclass) {
    case ARES_CLASS_IN:
    case ARES_CLASS_CHAOS:
    case ARES_CLASS_HESOID:
    case ARES_CLASS_NONE:
      return ARES_TRUE;
    case ARES_CLASS_ANY:
      return is_query ? ARES_TRUE : ARES_FALSE;
  }
  return ARES_FALSE;
}

ares_bool_t ares_dns_section_isvalid(ares_dns_section_t sect)
{
  switch (sect) {
    case ARES_SECTION_ANSWER:
    case ARES_SECTION_AUTHORITY:
    case ARES_SECTION_ADDITIONAL:
      return ARES_TRUE;
  }
  return ARES_FALSE;
}

ares_dns_rec_type_t ares_dns_rr_key_to_rec_type(ares_dns_rr_key_t key)
{
  /* NOTE: due to the way we've numerated the keys, we can simply divide by
   *       100 to get the type rather than having to do a huge switch
   *       statement.  That said, we do then validate the type returned is
   *       valid in case something completely bogus is passed in */
  ares_dns_rec_type_t type = key / 100;
  if (!ares_dns_rec_type_isvalid(type, ARES_FALSE)) {
    return 0;
  }
  return type;
}

const char *ares_dns_rec_type_tostr(ares_dns_rec_type_t type)
{
  switch (type) {
    case ARES_REC_TYPE_A:
      return "A";
    case ARES_REC_TYPE_NS:
      return "NS";
    case ARES_REC_TYPE_CNAME:
      return "CNAME";
    case ARES_REC_TYPE_SOA:
      return "SOA";
    case ARES_REC_TYPE_PTR:
      return "PTR";
    case ARES_REC_TYPE_HINFO:
      return "HINFO";
    case ARES_REC_TYPE_MX:
      return "MX";
    case ARES_REC_TYPE_TXT:
      return "TXT";
    case ARES_REC_TYPE_AAAA:
      return "AAAA";
    case ARES_REC_TYPE_SRV:
      return "SRV";
    case ARES_REC_TYPE_NAPTR:
      return "NAPTR";
    case ARES_REC_TYPE_OPT:
      return "OPT";
    case ARES_REC_TYPE_TLSA:
      return "TLSA";
    case ARES_REC_TYPE_SVCB:
      return "SVCB";
    case ARES_REC_TYPE_HTTPS:
      return "HTTPS";
    case ARES_REC_TYPE_ANY:
      return "ANY";
    case ARES_REC_TYPE_URI:
      return "URI";
    case ARES_REC_TYPE_CAA:
      return "CAA";
    case ARES_REC_TYPE_RAW_RR:
      return "RAWRR";
  }
  return "UNKNOWN";
}

const char *ares_dns_class_tostr(ares_dns_class_t qclass)
{
  switch (qclass) {
    case ARES_CLASS_IN:
      return "IN";
    case ARES_CLASS_CHAOS:
      return "CH";
    case ARES_CLASS_HESOID:
      return "HS";
    case ARES_CLASS_ANY:
      return "ANY";
    case ARES_CLASS_NONE:
      return "NONE";
  }
  return "UNKNOWN";
}

const char *ares_dns_opcode_tostr(ares_dns_opcode_t opcode)
{
  switch (opcode) {
    case ARES_OPCODE_QUERY:
      return "QUERY";
    case ARES_OPCODE_IQUERY:
      return "IQUERY";
    case ARES_OPCODE_STATUS:
      return "STATUS";
    case ARES_OPCODE_NOTIFY:
      return "NOTIFY";
    case ARES_OPCODE_UPDATE:
      return "UPDATE";
  }
  return "UNKNOWN";
}

const char *ares_dns_rr_key_tostr(ares_dns_rr_key_t key)
{
  switch (key) {
    case ARES_RR_A_ADDR:
      return "ADDR";

    case ARES_RR_NS_NSDNAME:
      return "NSDNAME";

    case ARES_RR_CNAME_CNAME:
      return "CNAME";

    case ARES_RR_SOA_MNAME:
      return "MNAME";

    case ARES_RR_SOA_RNAME:
      return "RNAME";

    case ARES_RR_SOA_SERIAL:
      return "SERIAL";

    case ARES_RR_SOA_REFRESH:
      return "REFRESH";

    case ARES_RR_SOA_RETRY:
      return "RETRY";

    case ARES_RR_SOA_EXPIRE:
      return "EXPIRE";

    case ARES_RR_SOA_MINIMUM:
      return "MINIMUM";

    case ARES_RR_PTR_DNAME:
      return "DNAME";

    case ARES_RR_AAAA_ADDR:
      return "ADDR";

    case ARES_RR_HINFO_CPU:
      return "CPU";

    case ARES_RR_HINFO_OS:
      return "OS";

    case ARES_RR_MX_PREFERENCE:
      return "PREFERENCE";

    case ARES_RR_MX_EXCHANGE:
      return "EXCHANGE";

    case ARES_RR_TXT_DATA:
      return "DATA";

    case ARES_RR_SRV_PRIORITY:
      return "PRIORITY";

    case ARES_RR_SRV_WEIGHT:
      return "WEIGHT";

    case ARES_RR_SRV_PORT:
      return "PORT";

    case ARES_RR_SRV_TARGET:
      return "TARGET";

    case ARES_RR_NAPTR_ORDER:
      return "ORDER";

    case ARES_RR_NAPTR_PREFERENCE:
      return "PREFERENCE";

    case ARES_RR_NAPTR_FLAGS:
      return "FLAGS";

    case ARES_RR_NAPTR_SERVICES:
      return "SERVICES";

    case ARES_RR_NAPTR_REGEXP:
      return "REGEXP";

    case ARES_RR_NAPTR_REPLACEMENT:
      return "REPLACEMENT";

    case ARES_RR_OPT_UDP_SIZE:
      return "UDP_SIZE";

    case ARES_RR_OPT_EXT_RCODE:
      return "EXT_RCODE";

    case ARES_RR_OPT_VERSION:
      return "VERSION";

    case ARES_RR_OPT_FLAGS:
      return "FLAGS";

    case ARES_RR_OPT_OPTIONS:
      return "OPTIONS";

    case ARES_RR_TLSA_CERT_USAGE:
      return "CERT_USAGE";

    case ARES_RR_TLSA_SELECTOR:
      return "SELECTOR";

    case ARES_RR_TLSA_MATCH:
      return "MATCH";

    case ARES_RR_TLSA_DATA:
      return "DATA";

    case ARES_RR_SVCB_PRIORITY:
      return "PRIORITY";

    case ARES_RR_SVCB_TARGET:
      return "TARGET";

    case ARES_RR_SVCB_PARAMS:
      return "PARAMS";

    case ARES_RR_HTTPS_PRIORITY:
      return "PRIORITY";

    case ARES_RR_HTTPS_TARGET:
      return "TARGET";

    case ARES_RR_HTTPS_PARAMS:
      return "PARAMS";

    case ARES_RR_URI_PRIORITY:
      return "PRIORITY";

    case ARES_RR_URI_WEIGHT:
      return "WEIGHT";

    case ARES_RR_URI_TARGET:
      return "TARGET";

    case ARES_RR_CAA_CRITICAL:
      return "CRITICAL";

    case ARES_RR_CAA_TAG:
      return "TAG";

    case ARES_RR_CAA_VALUE:
      return "VALUE";

    case ARES_RR_RAW_RR_TYPE:
      return "TYPE";

    case ARES_RR_RAW_RR_DATA:
      return "DATA";
  }

  return "UNKNOWN";
}

ares_dns_datatype_t ares_dns_rr_key_datatype(ares_dns_rr_key_t key)
{
  switch (key) {
    case ARES_RR_A_ADDR:
      return ARES_DATATYPE_INADDR;

    case ARES_RR_AAAA_ADDR:
      return ARES_DATATYPE_INADDR6;

    case ARES_RR_NS_NSDNAME:
    case ARES_RR_CNAME_CNAME:
    case ARES_RR_SOA_MNAME:
    case ARES_RR_SOA_RNAME:
    case ARES_RR_PTR_DNAME:
    case ARES_RR_HINFO_CPU:
    case ARES_RR_HINFO_OS:
    case ARES_RR_MX_EXCHANGE:
    case ARES_RR_SRV_TARGET:
    case ARES_RR_SVCB_TARGET:
    case ARES_RR_HTTPS_TARGET:
    case ARES_RR_NAPTR_FLAGS:
    case ARES_RR_NAPTR_SERVICES:
    case ARES_RR_NAPTR_REGEXP:
    case ARES_RR_NAPTR_REPLACEMENT:
    case ARES_RR_URI_TARGET:
    case ARES_RR_CAA_TAG:
      return ARES_DATATYPE_STR;

    case ARES_RR_SOA_SERIAL:
    case ARES_RR_SOA_REFRESH:
    case ARES_RR_SOA_RETRY:
    case ARES_RR_SOA_EXPIRE:
    case ARES_RR_SOA_MINIMUM:
      return ARES_DATATYPE_U32;

    case ARES_RR_MX_PREFERENCE:
    case ARES_RR_SRV_PRIORITY:
    case ARES_RR_SRV_WEIGHT:
    case ARES_RR_SRV_PORT:
    case ARES_RR_NAPTR_ORDER:
    case ARES_RR_NAPTR_PREFERENCE:
    case ARES_RR_OPT_UDP_SIZE:
    case ARES_RR_OPT_FLAGS:
    case ARES_RR_SVCB_PRIORITY:
    case ARES_RR_HTTPS_PRIORITY:
    case ARES_RR_URI_PRIORITY:
    case ARES_RR_URI_WEIGHT:
    case ARES_RR_RAW_RR_TYPE:
      return ARES_DATATYPE_U16;

    case ARES_RR_OPT_EXT_RCODE:
    case ARES_RR_OPT_VERSION:
    case ARES_RR_TLSA_CERT_USAGE:
    case ARES_RR_TLSA_SELECTOR:
    case ARES_RR_TLSA_MATCH:
    case ARES_RR_CAA_CRITICAL:
      return ARES_DATATYPE_U8;

    case ARES_RR_CAA_VALUE:
    case ARES_RR_TXT_DATA:
      return ARES_DATATYPE_BINP;

    case ARES_RR_TLSA_DATA:
    case ARES_RR_RAW_RR_DATA:
      return ARES_DATATYPE_BIN;

    case ARES_RR_OPT_OPTIONS:
    case ARES_RR_SVCB_PARAMS:
    case ARES_RR_HTTPS_PARAMS:
      return ARES_DATATYPE_OPT;
  }

  return 0;
}

static const ares_dns_rr_key_t rr_a_keys[]     = { ARES_RR_A_ADDR };
static const ares_dns_rr_key_t rr_ns_keys[]    = { ARES_RR_NS_NSDNAME };
static const ares_dns_rr_key_t rr_cname_keys[] = { ARES_RR_CNAME_CNAME };
static const ares_dns_rr_key_t rr_soa_keys[]   = {
  ARES_RR_SOA_MNAME,   ARES_RR_SOA_RNAME, ARES_RR_SOA_SERIAL,
  ARES_RR_SOA_REFRESH, ARES_RR_SOA_RETRY, ARES_RR_SOA_EXPIRE,
  ARES_RR_SOA_MINIMUM
};
static const ares_dns_rr_key_t rr_ptr_keys[]   = { ARES_RR_PTR_DNAME };
static const ares_dns_rr_key_t rr_hinfo_keys[] = { ARES_RR_HINFO_CPU,
                                                   ARES_RR_HINFO_OS };
static const ares_dns_rr_key_t rr_mx_keys[]    = { ARES_RR_MX_PREFERENCE,
                                                   ARES_RR_MX_EXCHANGE };
static const ares_dns_rr_key_t rr_txt_keys[]   = { ARES_RR_TXT_DATA };
static const ares_dns_rr_key_t rr_aaaa_keys[]  = { ARES_RR_AAAA_ADDR };
static const ares_dns_rr_key_t rr_srv_keys[]   = {
  ARES_RR_SRV_PRIORITY, ARES_RR_SRV_WEIGHT, ARES_RR_SRV_PORT, ARES_RR_SRV_TARGET
};
static const ares_dns_rr_key_t rr_naptr_keys[] = {
  ARES_RR_NAPTR_ORDER,    ARES_RR_NAPTR_PREFERENCE, ARES_RR_NAPTR_FLAGS,
  ARES_RR_NAPTR_SERVICES, ARES_RR_NAPTR_REGEXP,     ARES_RR_NAPTR_REPLACEMENT
};
static const ares_dns_rr_key_t rr_opt_keys[] = {
  ARES_RR_OPT_UDP_SIZE, ARES_RR_OPT_EXT_RCODE, ARES_RR_OPT_VERSION,
  ARES_RR_OPT_FLAGS, ARES_RR_OPT_OPTIONS
};
static const ares_dns_rr_key_t rr_tlsa_keys[]   = { ARES_RR_TLSA_CERT_USAGE,
                                                    ARES_RR_TLSA_SELECTOR,
                                                    ARES_RR_TLSA_MATCH,
                                                    ARES_RR_TLSA_DATA };
static const ares_dns_rr_key_t rr_svcb_keys[]   = { ARES_RR_SVCB_PRIORITY,
                                                    ARES_RR_SVCB_TARGET,
                                                    ARES_RR_SVCB_PARAMS };
static const ares_dns_rr_key_t rr_https_keys[]  = { ARES_RR_HTTPS_PRIORITY,
                                                    ARES_RR_HTTPS_TARGET,
                                                    ARES_RR_HTTPS_PARAMS };
static const ares_dns_rr_key_t rr_uri_keys[]    = { ARES_RR_URI_PRIORITY,
                                                    ARES_RR_URI_WEIGHT,
                                                    ARES_RR_URI_TARGET };
static const ares_dns_rr_key_t rr_caa_keys[]    = { ARES_RR_CAA_CRITICAL,
                                                    ARES_RR_CAA_TAG,
                                                    ARES_RR_CAA_VALUE };
static const ares_dns_rr_key_t rr_raw_rr_keys[] = { ARES_RR_RAW_RR_TYPE,
                                                    ARES_RR_RAW_RR_DATA };

const ares_dns_rr_key_t       *ares_dns_rr_get_keys(ares_dns_rec_type_t type,
                                                    size_t             *cnt)
{
  if (cnt == NULL) {
    return NULL;
  }

  *cnt = 0;

  switch (type) {
    case ARES_REC_TYPE_A:
      *cnt = sizeof(rr_a_keys) / sizeof(*rr_a_keys);
      return rr_a_keys;
    case ARES_REC_TYPE_NS:
      *cnt = sizeof(rr_ns_keys) / sizeof(*rr_ns_keys);
      return rr_ns_keys;
    case ARES_REC_TYPE_CNAME:
      *cnt = sizeof(rr_cname_keys) / sizeof(*rr_cname_keys);
      return rr_cname_keys;
    case ARES_REC_TYPE_SOA:
      *cnt = sizeof(rr_soa_keys) / sizeof(*rr_soa_keys);
      return rr_soa_keys;
    case ARES_REC_TYPE_PTR:
      *cnt = sizeof(rr_ptr_keys) / sizeof(*rr_ptr_keys);
      return rr_ptr_keys;
    case ARES_REC_TYPE_HINFO:
      *cnt = sizeof(rr_hinfo_keys) / sizeof(*rr_hinfo_keys);
      return rr_hinfo_keys;
    case ARES_REC_TYPE_MX:
      *cnt = sizeof(rr_mx_keys) / sizeof(*rr_mx_keys);
      return rr_mx_keys;
    case ARES_REC_TYPE_TXT:
      *cnt = sizeof(rr_txt_keys) / sizeof(*rr_txt_keys);
      return rr_txt_keys;
    case ARES_REC_TYPE_AAAA:
      *cnt = sizeof(rr_aaaa_keys) / sizeof(*rr_aaaa_keys);
      return rr_aaaa_keys;
    case ARES_REC_TYPE_SRV:
      *cnt = sizeof(rr_srv_keys) / sizeof(*rr_srv_keys);
      return rr_srv_keys;
    case ARES_REC_TYPE_NAPTR:
      *cnt = sizeof(rr_naptr_keys) / sizeof(*rr_naptr_keys);
      return rr_naptr_keys;
    case ARES_REC_TYPE_OPT:
      *cnt = sizeof(rr_opt_keys) / sizeof(*rr_opt_keys);
      return rr_opt_keys;
    case ARES_REC_TYPE_TLSA:
      *cnt = sizeof(rr_tlsa_keys) / sizeof(*rr_tlsa_keys);
      return rr_tlsa_keys;
    case ARES_REC_TYPE_SVCB:
      *cnt = sizeof(rr_svcb_keys) / sizeof(*rr_svcb_keys);
      return rr_svcb_keys;
    case ARES_REC_TYPE_HTTPS:
      *cnt = sizeof(rr_https_keys) / sizeof(*rr_https_keys);
      return rr_https_keys;
    case ARES_REC_TYPE_ANY:
      /* Not real */
      break;
    case ARES_REC_TYPE_URI:
      *cnt = sizeof(rr_uri_keys) / sizeof(*rr_uri_keys);
      return rr_uri_keys;
    case ARES_REC_TYPE_CAA:
      *cnt = sizeof(rr_caa_keys) / sizeof(*rr_caa_keys);
      return rr_caa_keys;
    case ARES_REC_TYPE_RAW_RR:
      *cnt = sizeof(rr_raw_rr_keys) / sizeof(*rr_raw_rr_keys);
      return rr_raw_rr_keys;
  }

  return NULL;
}
