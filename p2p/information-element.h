/*
 * This file is part of wysiwidi
 *
 * Copyright (C) 2014 Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef INFORMATION_ELEMENT_H_
#define INFORMATION_ELEMENT_H_

#include <map>
#include <memory>
#include <string>
#include <stdint.h>


namespace P2P {

enum DeviceType {
    SOURCE,
    PRIMARY_SINK,
    SECONDARY_SINK,
    DUAL_ROLE
};

enum SubelementId {
    DEVICE_INFORMATION,
    ASSOCIATED_BSSID,
    AUDIO_FORMATS,
    VIDEO_FORMATS,
    FORMATS_3D,
    CONTENT_PROTECTION,
    COUPLED_SINK_INFORMATION,
    EXTENDED_CAPABILITY,
    LOCAL_IP_ADDRESS,
    SESSION_INFORMATION,
    ALTERNATIVE_MAC,
};

// SubelementSize == subelement.length - 3
const uint16_t SubelementSize[] = {
    9,
    9,
    18,
    24,
    20,
    4,
    10,
    5,
    11,
    3, // variable: 3 + N*24, where N is number of devices connected to GO
    9,
};

typedef struct Subelement_ {
    uint8_t id;
    uint16_t length;
} __attribute__ ((packed)) Subelement;

typedef struct DeviceinformationBits1_{
    unsigned device_type : 2; // DeviceType
    unsigned coupled_sink_support_at_source : 1;
    unsigned coupled_sink_support_at_sink : 1;
    unsigned session_availability : 1;
    unsigned reserved : 1;
    unsigned service_discovery_support : 1;
    unsigned preferred_connectivity : 1;
} __attribute__ ((packed)) DeviceinformationBits1;

typedef struct DeviceinformationBits2_{
    unsigned hdcp_support : 1;
    unsigned time_synchronization_support : 1;
    unsigned audio_unsupport_at_primary_sink : 1;
    unsigned audio_only_support_at_source : 1;
    unsigned tdls_persistent_group : 1;
    unsigned tdls_persistent_group_reinvoke : 1;
    unsigned reserved2 : 2;
} __attribute__ ((packed)) DeviceinformationBits2;

typedef struct DeviceInformationSubelement_{
    uint8_t id;
    uint16_t length;
    DeviceinformationBits2 field2;
    DeviceinformationBits1 field1;
    uint16_t session_management_control_port;
    uint16_t maximum_throughput;
} __attribute__ ((packed)) DeviceInformationSubelement;

typedef struct AssociatedBSSIDSubelement_ {
    uint8_t id;
    uint16_t length;
    uint8_t bssid[6];
} __attribute__ ((packed)) AssociatedBSSIDSubelement;

typedef struct CoupledSinkStatus_ {
    unsigned status : 2;
    unsigned reserved : 6; 
} __attribute__ ((packed)) CoupledSinkStatus;

typedef struct CoupledSinkInformationSubelement_ {
    uint8_t id;
    uint16_t length;
    CoupledSinkStatus status;
    uint8_t mac_address[6];
} __attribute__ ((packed)) CoupledSinkInformationSubelement;

struct InformationElementArray {
    uint8_t *bytes;
    uint length;

    InformationElementArray(uint len) : length(len) {
        bytes = new uint8_t[length];
    }
    ~InformationElementArray() {
        delete[] bytes;
    }
};

Subelement* new_subelement (SubelementId id);

class InformationElement {
  public:
    InformationElement();
    InformationElement(const std::unique_ptr<InformationElementArray> &array);
    virtual ~InformationElement();

    void add_subelement(P2P::Subelement* subelement);

    std::unique_ptr<InformationElementArray> serialize () const;
    std::string to_string() const;

  private:
    void delete_subelement(P2P::Subelement* subelement);

    uint length_;
    std::map<SubelementId, P2P::Subelement*> subelements_;
};

} // namespace WFD

#endif // INFORMATION_ELEMENT_H_
