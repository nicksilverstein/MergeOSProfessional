/*
 * PROJECT:     MergeOS Intel PRO/1000 Driver
 * LICENSE:     GPL-2.0+ (https://spdx.org/licenses/GPL-2.0+)
 * PURPOSE:     Translate NDIS_OID to readable string
 * COPYRIGHT:   Copyright 2018 Mark Jansen (mark.jansen@reactos.org)
 */

#include "nic.h"

#include <debug.h>

const char* Oid2Str(IN NDIS_OID Oid)
{
#if DBG
    switch (Oid)
    {
#define RETURN_X(x) case x: return #x;
        /* Required Object IDs (OIDs) */
        RETURN_X(OID_GEN_SUPPORTED_LIST);
        RETURN_X(OID_GEN_HARDWARE_STATUS);
        RETURN_X(OID_GEN_MEDIA_SUPPORTED);
        RETURN_X(OID_GEN_MEDIA_IN_USE);
        RETURN_X(OID_GEN_MAXIMUM_LOOKAHEAD);
        RETURN_X(OID_GEN_MAXIMUM_FRAME_SIZE);
        RETURN_X(OID_GEN_LINK_SPEED);
        RETURN_X(OID_GEN_TRANSMIT_BUFFER_SPACE);
        RETURN_X(OID_GEN_RECEIVE_BUFFER_SPACE);
        RETURN_X(OID_GEN_TRANSMIT_BLOCK_SIZE);
        RETURN_X(OID_GEN_RECEIVE_BLOCK_SIZE);
        RETURN_X(OID_GEN_VENDOR_ID);
        RETURN_X(OID_GEN_VENDOR_DESCRIPTION);
        RETURN_X(OID_GEN_CURRENT_PACKET_FILTER);
        RETURN_X(OID_GEN_CURRENT_LOOKAHEAD);
        RETURN_X(OID_GEN_DRIVER_VERSION);
        RETURN_X(OID_GEN_MAXIMUM_TOTAL_SIZE);
        RETURN_X(OID_GEN_PROTOCOL_OPTIONS);
        RETURN_X(OID_GEN_MAC_OPTIONS);
        RETURN_X(OID_GEN_MEDIA_CONNECT_STATUS);
        RETURN_X(OID_GEN_MAXIMUM_SEND_PACKETS);
        RETURN_X(OID_GEN_VENDOR_DRIVER_VERSION);
        RETURN_X(OID_GEN_SUPPORTED_GUIDS);
        RETURN_X(OID_GEN_NETWORK_LAYER_ADDRESSES);
        RETURN_X(OID_GEN_TRANSPORT_HEADER_OFFSET);
        RETURN_X(OID_GEN_MACHINE_NAME);
        RETURN_X(OID_GEN_RNDIS_CONFIG_PARAMETER);
        RETURN_X(OID_GEN_VLAN_ID);

        /* Optional OIDs */
        RETURN_X(OID_GEN_MEDIA_CAPABILITIES);
        RETURN_X(OID_GEN_PHYSICAL_MEDIUM);

        /* Required statistics OIDs */
        RETURN_X(OID_GEN_XMIT_OK);
        RETURN_X(OID_GEN_RCV_OK);
        RETURN_X(OID_GEN_XMIT_ERROR);
        RETURN_X(OID_GEN_RCV_ERROR);
        RETURN_X(OID_GEN_RCV_NO_BUFFER);

        /* Optional statistics OIDs */
        RETURN_X(OID_GEN_DIRECTED_BYTES_XMIT);
        RETURN_X(OID_GEN_DIRECTED_FRAMES_XMIT);
        RETURN_X(OID_GEN_MULTICAST_BYTES_XMIT);
        RETURN_X(OID_GEN_MULTICAST_FRAMES_XMIT);
        RETURN_X(OID_GEN_BROADCAST_BYTES_XMIT);
        RETURN_X(OID_GEN_BROADCAST_FRAMES_XMIT);
        RETURN_X(OID_GEN_DIRECTED_BYTES_RCV);
        RETURN_X(OID_GEN_DIRECTED_FRAMES_RCV);
        RETURN_X(OID_GEN_MULTICAST_BYTES_RCV);
        RETURN_X(OID_GEN_MULTICAST_FRAMES_RCV);
        RETURN_X(OID_GEN_BROADCAST_BYTES_RCV);
        RETURN_X(OID_GEN_BROADCAST_FRAMES_RCV);
        RETURN_X(OID_GEN_RCV_CRC_ERROR);
        RETURN_X(OID_GEN_TRANSMIT_QUEUE_LENGTH);
        RETURN_X(OID_GEN_GET_TIME_CAPS);
        RETURN_X(OID_GEN_GET_NETCARD_TIME);
        RETURN_X(OID_GEN_NETCARD_LOAD);
        RETURN_X(OID_GEN_DEVICE_PROFILE);
        RETURN_X(OID_GEN_INIT_TIME_MS);
        RETURN_X(OID_GEN_RESET_COUNTS);
        RETURN_X(OID_GEN_MEDIA_SENSE_COUNTS);
        RETURN_X(OID_GEN_FRIENDLY_NAME);
        RETURN_X(OID_GEN_MINIPORT_INFO);
        RETURN_X(OID_GEN_RESET_VERIFY_PARAMETERS);

        /* IEEE 802.3 (Ethernet) OIDs */
        //RETURN_X(NDIS_802_3_MAC_OPTION_PRIORITY);     /*Duplicate ID */

        RETURN_X(OID_802_3_PERMANENT_ADDRESS);
        RETURN_X(OID_802_3_CURRENT_ADDRESS);
        RETURN_X(OID_802_3_MULTICAST_LIST);
        RETURN_X(OID_802_3_MAXIMUM_LIST_SIZE);
        RETURN_X(OID_802_3_MAC_OPTIONS);
        RETURN_X(OID_802_3_RCV_ERROR_ALIGNMENT);
        RETURN_X(OID_802_3_XMIT_ONE_COLLISION);
        RETURN_X(OID_802_3_XMIT_MORE_COLLISIONS);
        RETURN_X(OID_802_3_XMIT_DEFERRED);
        RETURN_X(OID_802_3_XMIT_MAX_COLLISIONS);
        RETURN_X(OID_802_3_RCV_OVERRUN);
        RETURN_X(OID_802_3_XMIT_UNDERRUN);
        RETURN_X(OID_802_3_XMIT_HEARTBEAT_FAILURE);
        RETURN_X(OID_802_3_XMIT_TIMES_CRS_LOST);
        RETURN_X(OID_802_3_XMIT_LATE_COLLISIONS);

        /* IEEE 802.11 (WLAN) OIDs */
        RETURN_X(OID_802_11_BSSID);
        RETURN_X(OID_802_11_SSID);
        RETURN_X(OID_802_11_NETWORK_TYPES_SUPPORTED);
        RETURN_X(OID_802_11_NETWORK_TYPE_IN_USE);
        RETURN_X(OID_802_11_TX_POWER_LEVEL);
        RETURN_X(OID_802_11_RSSI);
        RETURN_X(OID_802_11_RSSI_TRIGGER);
        RETURN_X(OID_802_11_INFRASTRUCTURE_MODE);
        RETURN_X(OID_802_11_FRAGMENTATION_THRESHOLD);
        RETURN_X(OID_802_11_RTS_THRESHOLD);
        RETURN_X(OID_802_11_NUMBER_OF_ANTENNAS);
        RETURN_X(OID_802_11_RX_ANTENNA_SELECTED);
        RETURN_X(OID_802_11_TX_ANTENNA_SELECTED);
        RETURN_X(OID_802_11_SUPPORTED_RATES);
        RETURN_X(OID_802_11_DESIRED_RATES);
        RETURN_X(OID_802_11_CONFIGURATION);
        RETURN_X(OID_802_11_STATISTICS);
        RETURN_X(OID_802_11_ADD_WEP);
        RETURN_X(OID_802_11_REMOVE_WEP);
        RETURN_X(OID_802_11_DISASSOCIATE);
        RETURN_X(OID_802_11_POWER_MODE);
        RETURN_X(OID_802_11_BSSID_LIST);
        RETURN_X(OID_802_11_AUTHENTICATION_MODE);
        RETURN_X(OID_802_11_PRIVACY_FILTER);
        RETURN_X(OID_802_11_BSSID_LIST_SCAN);
        RETURN_X(OID_802_11_WEP_STATUS);
        RETURN_X(OID_802_11_RELOAD_DEFAULTS);

        /* OID_GEN_MINIPORT_INFO constants */
        RETURN_X(NDIS_MINIPORT_BUS_MASTER);
        RETURN_X(NDIS_MINIPORT_WDM_DRIVER);
        RETURN_X(NDIS_MINIPORT_SG_LIST);
        RETURN_X(NDIS_MINIPORT_SUPPORTS_MEDIA_QUERY);
        RETURN_X(NDIS_MINIPORT_INDICATES_PACKETS);
        RETURN_X(NDIS_MINIPORT_IGNORE_PACKET_QUEUE);
        RETURN_X(NDIS_MINIPORT_IGNORE_REQUEST_QUEUE);
        RETURN_X(NDIS_MINIPORT_IGNORE_TOKEN_RING_ERRORS);
        RETURN_X(NDIS_MINIPORT_INTERMEDIATE_DRIVER);
        RETURN_X(NDIS_MINIPORT_IS_NDIS_5);
        RETURN_X(NDIS_MINIPORT_IS_CO);
        RETURN_X(NDIS_MINIPORT_DESERIALIZE);
        RETURN_X(NDIS_MINIPORT_REQUIRES_MEDIA_POLLING);
        RETURN_X(NDIS_MINIPORT_SUPPORTS_MEDIA_SENSE);
        RETURN_X(NDIS_MINIPORT_NETBOOT_CARD);
        RETURN_X(NDIS_MINIPORT_PM_SUPPORTED);
        RETURN_X(NDIS_MINIPORT_SUPPORTS_MAC_ADDRESS_OVERWRITE);
        RETURN_X(NDIS_MINIPORT_USES_SAFE_BUFFER_APIS);
        RETURN_X(NDIS_MINIPORT_HIDDEN);
        RETURN_X(NDIS_MINIPORT_SWENUM);
        RETURN_X(NDIS_MINIPORT_SURPRISE_REMOVE_OK);
        RETURN_X(NDIS_MINIPORT_NO_HALT_ON_SUSPEND);
        RETURN_X(NDIS_MINIPORT_HARDWARE_DEVICE);
        RETURN_X(NDIS_MINIPORT_SUPPORTS_CANCEL_SEND_PACKETS);
        RETURN_X(NDIS_MINIPORT_64BITS_DMA);
    default:
        return "<UNKNOWN>";
    }
#else
    return "!DBG";
#endif
}

