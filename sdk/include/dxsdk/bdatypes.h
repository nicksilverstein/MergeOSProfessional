/*
 * bdatypes.h
 *
 * This file is part of the MergeOS DXSDK package.
 *
 * THIS SOFTWARE IS NOT COPYRIGHTED
 *
 * This source code is offered for use in the public domain. You may
 * use, modify or distribute it freely.
 *
 * This code is distributed in the hope that it will be useful but
 * WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
 * DISCLAIMED. This includes but is not limited to warranties of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _BDATYPES_
#define _BDATYPES_ 1

#define MIN_DIMENSION 1

#ifdef __midl
#define V1_ENUM [v1_enum]
#else
#define V1_ENUM
#endif

#define BDA_FREQUENCY_NOT_SET                -1
#define BDA_FREQUENCY_NOT_DEFINED            0
#define BDA_RANGE_NOT_SET                    -1
#define BDA_RANGE_NOT_DEFINED                0
#define BDA_CHAN_BANDWITH_NOT_SET            -1
#define BDA_CHAN_BANDWITH_NOT_DEFINED        0
#define BDA_FREQUENCY_MULTIPLIER_NOT_SET     -1
#define BDA_FREQUENCY_MULTIPLIER_NOT_DEFINED 0

typedef struct _BDA_TEMPLATE_CONNECTION {
  ULONG FromNodeType;
  ULONG FromNodePinType;
  ULONG ToNodeType;
  ULONG ToNodePinType;
} BDA_TEMPLATE_CONNECTION, *PBDA_TEMPLATE_CONNECTION;

typedef struct _BDA_TEMPLATE_PIN_JOINT {
  ULONG uliTemplateConnection;
  ULONG ulcInstancesMax;
} BDA_TEMPLATE_PIN_JOINT, *PBDA_TEMPLATE_PIN_JOINT;

typedef struct tagKS_BDA_FRAME_INFO {
  ULONG ExtendedHeaderSize;
  DWORD dwFrameFlags;
  ULONG ulEvent;
  ULONG ulChannelNumber;
  ULONG ulSubchannelNumber;
  ULONG ulReason;
} KS_BDA_FRAME_INFO, *PKS_BDA_FRAME_INFO;

typedef enum {
  BDA_EVENT_SIGNAL_LOSS = 0,
  BDA_EVENT_SIGNAL_LOCK,
  BDA_EVENT_DATA_START,
  BDA_EVENT_DATA_STOP,
  BDA_EVENT_CHANNEL_ACQUIRED,
  BDA_EVENT_CHANNEL_LOST,
  BDA_EVENT_CHANNEL_SOURCE_CHANGED,
  BDA_EVENT_CHANNEL_ACTIVATED,
  BDA_EVENT_CHANNEL_DEACTIVATED,
  BDA_EVENT_SUBCHANNEL_ACQUIRED,
  BDA_EVENT_SUBCHANNEL_LOST,
  BDA_EVENT_SUBCHANNEL_SOURCE_CHANGED,
  BDA_EVENT_SUBCHANNEL_ACTIVATED,
  BDA_EVENT_SUBCHANNEL_DEACTIVATED,
  BDA_EVENT_ACCESS_GRANTED,
  BDA_EVENT_ACCESS_DENIED,
  BDA_EVENT_OFFER_EXTENDED,
  BDA_EVENT_PURCHASE_COMPLETED,
  BDA_EVENT_SMART_CARD_INSERTED,
  BDA_EVENT_SMART_CARD_REMOVED
} BDA_EVENT_ID, *PBDA_EVENT_ID;

typedef struct _BDA_ETHERNET_ADDRESS {
  BYTE rgbAddress[6];
} BDA_ETHERNET_ADDRESS, *PBDA_ETHERNET_ADDRESS;

typedef struct _BDA_ETHERNET_ADDRESS_LIST {
  ULONG ulcAddresses;
  BDA_ETHERNET_ADDRESS rgAddressl[MIN_DIMENSION];
} BDA_ETHERNET_ADDRESS_LIST, *PBDA_ETHERNET_ADDRESS_LIST;

typedef enum {
  BDA_PROMISCUOUS_MULTICAST = 0,
  BDA_FILTERED_MULTICAST,
  BDA_NO_MULTICAST
} BDA_MULTICAST_MODE, *PBDA_MULTICAST_MODE;

typedef struct _BDA_IPv4_ADDRESS {
  BYTE rgbAddress[4];
} BDA_IPv4_ADDRESS, *PBDA_IPv4_ADDRESS;

typedef struct _BDA_IPv4_ADDRESS_LIST {
  ULONG ulcAddresses;
  BDA_IPv4_ADDRESS rgAddressl[MIN_DIMENSION];
} BDA_IPv4_ADDRESS_LIST, *PBDA_IPv4_ADDRESS_LIST;

typedef struct _BDA_IPv6_ADDRESS {
  BYTE rgbAddress[6];
} BDA_IPv6_ADDRESS, *PBDA_IPv6_ADDRESS;

typedef struct _BDA_IPv6_ADDRESS_LIST {
  ULONG ulcAddresses;
  BDA_IPv6_ADDRESS rgAddressl[MIN_DIMENSION];
} BDA_IPv6_ADDRESS_LIST, *PBDA_IPv6_ADDRESS_LIST;

typedef enum {
  BDA_CHANGES_COMPLETE = 0,
  BDA_CHANGES_PENDING
} BDA_CHANGE_STATE, *PBDA_CHANGE_STATE;

typedef enum {
  BDA_SIGNAL_UNAVAILABLE = 0,
  BDA_SIGNAL_INACTIVE,
  BDA_SIGNAL_ACTIVE
} BDA_SIGNAL_STATE, *PBDA_SIGNAL_STATE;

typedef struct _BDANODE_DESCRIPTOR {
  ULONG ulBdaNodeType;
  GUID guidFunction;
  GUID guidName;
} BDANODE_DESCRIPTOR, *PBDANODE_DESCRIPTOR;

typedef enum {
  MEDIA_TRANSPORT_PACKET,
  MEDIA_ELEMENTARY_STREAM,
  MEDIA_MPEG2_PSI,
  MEDIA_TRANSPORT_PAYLOAD
} MEDIA_SAMPLE_CONTENT;

typedef struct _BDA_TABLE_SECTION {
  ULONG ulPrimarySectionId;
  ULONG ulSecondarySectionId;
  ULONG ulcbSectionLength;
  ULONG argbSectionData[MIN_DIMENSION];
} BDA_TABLE_SECTION, *PBDA_TABLE_SECTION;

typedef struct {
  ULONG ulPID;
  MEDIA_SAMPLE_CONTENT MediaSampleContent ;
} PID_MAP;

typedef struct _BDA_PID_MAP {
  MEDIA_SAMPLE_CONTENT MediaSampleContent;
  ULONG ulcPIDs;
  ULONG aulPIDs[MIN_DIMENSION];
} BDA_PID_MAP, *PBDA_PID_MAP;

typedef struct _BDA_PID_UNMAP {
  ULONG ulcPIDs;
  ULONG aulPIDs[MIN_DIMENSION];
} BDA_PID_UNMAP, *PBDA_PID_UNMAP;

typedef enum DVBSystemType {
  DVB_Cable,
  DVB_Terrestrial,
  DVB_Satellite,
} DVBSystemType;

typedef struct _BDA_CA_MODULE_UI {
  ULONG ulFormat;
  ULONG ulbcDesc;
  ULONG ulDesc[MIN_DIMENSION];
} BDA_CA_MODULE_UI, *PBDA_CA_MODULE_UI;

typedef struct _BDA_PROGRAM_PID_LIST {
  ULONG ulProgramNumber;
  ULONG ulcPIDs;
  ULONG ulPID[MIN_DIMENSION];
} BDA_PROGRAM_PID_LIST, *PBDA_PROGRAM_PID_LIST;

V1_ENUM enum {
  BDA_UNDEFINED_CHANNEL = -1,
};

typedef V1_ENUM enum ComponentCategory {
  CategoryNotSet = -1,
  CategoryOther = 0,
  CategoryVideo,
  CategoryAudio,
  CategoryText,
  CategoryData,
} ComponentCategory;

typedef enum ComponentStatus {
  StatusActive,
  StatusInactive,
  StatusUnavailable,
} ComponentStatus;

typedef enum ATSCComponentTypeFlags {
  ATSCCT_AC3 = 0x00000001,
} ATSCComponentTypeFlags;

typedef V1_ENUM enum MPEG2StreamType {
  BDA_UNITIALIZED_MPEG2STREAMTYPE = -1,
  Reserved1 = 0x0,
  ISO_IEC_11172_2_VIDEO = Reserved1 + 1,
  ISO_IEC_13818_2_VIDEO = ISO_IEC_11172_2_VIDEO + 1,
  ISO_IEC_11172_3_AUDIO = ISO_IEC_13818_2_VIDEO + 1,
  ISO_IEC_13818_3_AUDIO = ISO_IEC_11172_3_AUDIO + 1,
  ISO_IEC_13818_1_PRIVATE_SECTION = ISO_IEC_13818_3_AUDIO + 1,
  ISO_IEC_13818_1_PES = ISO_IEC_13818_1_PRIVATE_SECTION + 1,
  ISO_IEC_13522_MHEG = ISO_IEC_13818_1_PES + 1,
  ANNEX_A_DSM_CC = ISO_IEC_13522_MHEG + 1,
  ITU_T_REC_H_222_1 = ANNEX_A_DSM_CC + 1,
  ISO_IEC_13818_6_TYPE_A = ITU_T_REC_H_222_1 + 1,
  ISO_IEC_13818_6_TYPE_B = ISO_IEC_13818_6_TYPE_A + 1,
  ISO_IEC_13818_6_TYPE_C = ISO_IEC_13818_6_TYPE_B + 1,
  ISO_IEC_13818_6_TYPE_D = ISO_IEC_13818_6_TYPE_C + 1,
  ISO_IEC_13818_1_AUXILIARY = ISO_IEC_13818_6_TYPE_D + 1,
  ISO_IEC_13818_1_RESERVED = ISO_IEC_13818_1_AUXILIARY + 1,
  USER_PRIVATE = ISO_IEC_13818_1_RESERVED + 1
} MPEG2StreamType;

typedef struct _MPEG2_TRANSPORT_STRIDE {
  DWORD dwOffset;
  DWORD dwPacketLength;
  DWORD dwStride;
} MPEG2_TRANSPORT_STRIDE, *PMPEG2_TRANSPORT_STRIDE;

typedef V1_ENUM enum BinaryConvolutionCodeRate {
  BDA_BCC_RATE_NOT_SET = -1,
  BDA_BCC_RATE_NOT_DEFINED = 0,
  BDA_BCC_RATE_1_2 = 1,
  BDA_BCC_RATE_2_3,
  BDA_BCC_RATE_3_4,
  BDA_BCC_RATE_3_5,
  BDA_BCC_RATE_4_5,
  BDA_BCC_RATE_5_6,
  BDA_BCC_RATE_5_11,
  BDA_BCC_RATE_7_8,
  BDA_BCC_RATE_MAX,
} BinaryConvolutionCodeRate;

typedef V1_ENUM enum SpectralInversion {
  BDA_SPECTRAL_INVERSION_NOT_SET = -1,
  BDA_SPECTRAL_INVERSION_NOT_DEFINED = 0,
  BDA_SPECTRAL_INVERSION_AUTOMATIC = 1,
  BDA_SPECTRAL_INVERSION_NORMAL,
  BDA_SPECTRAL_INVERSION_INVERTED,
  BDA_SPECTRAL_INVERSION_MAX
} SpectralInversion;

typedef V1_ENUM enum FECMethod {
  BDA_FEC_METHOD_NOT_SET = -1,
  BDA_FEC_METHOD_NOT_DEFINED = 0,
  BDA_FEC_VITERBI = 1,
  BDA_FEC_RS_204_188,
  BDA_FEC_MAX,
} FECMethod;

typedef V1_ENUM enum ModulationType {
  BDA_MOD_NOT_SET = -1,
  BDA_MOD_NOT_DEFINED = 0,
  BDA_MOD_16QAM = 1,
  BDA_MOD_32QAM,
  BDA_MOD_64QAM,
  BDA_MOD_80QAM,
  BDA_MOD_96QAM,
  BDA_MOD_112QAM,
  BDA_MOD_128QAM,
  BDA_MOD_160QAM,
  BDA_MOD_192QAM,
  BDA_MOD_224QAM,
  BDA_MOD_256QAM,
  BDA_MOD_320QAM,
  BDA_MOD_384QAM,
  BDA_MOD_448QAM,
  BDA_MOD_512QAM,
  BDA_MOD_640QAM,
  BDA_MOD_768QAM,
  BDA_MOD_896QAM,
  BDA_MOD_1024QAM,
  BDA_MOD_QPSK,
  BDA_MOD_BPSK,
  BDA_MOD_OQPSK,
  BDA_MOD_8VSB,
  BDA_MOD_16VSB,
  BDA_MOD_ANALOG_AMPLITUDE,
  BDA_MOD_ANALOG_FREQUENCY,
  BDA_MOD_MAX,
} ModulationType;

typedef V1_ENUM enum TransmissionMode {
  BDA_XMIT_MODE_NOT_SET = -1,
  BDA_XMIT_MODE_NOT_DEFINED = 0,
  BDA_XMIT_MODE_2K = 1,
  BDA_XMIT_MODE_8K,
  BDA_XMIT_MODE_MAX,
} TransmissionMode;

typedef V1_ENUM enum Polarisation {
  BDA_POLARISATION_NOT_SET = -1,
  BDA_POLARISATION_NOT_DEFINED = 0,
  BDA_POLARISATION_LINEAR_H = 1,
  BDA_POLARISATION_LINEAR_V,
  BDA_POLARISATION_CIRCULAR_L,
  BDA_POLARISATION_CIRCULAR_R,
  BDA_POLARISATION_MAX,
} Polarisation;

typedef V1_ENUM enum GuardInterval {
  BDA_GUARD_NOT_SET = -1,
  BDA_GUARD_NOT_DEFINED = 0,
  BDA_GUARD_1_32 = 1,
  BDA_GUARD_1_16,
  BDA_GUARD_1_8,
  BDA_GUARD_1_4,
  BDA_GUARD_MAX,
} GuardInterval;

typedef V1_ENUM enum HierarchyAlpha {
  BDA_HALPHA_NOT_SET = -1,
  BDA_HALPHA_NOT_DEFINED = 0,
  BDA_HALPHA_1 = 1,
  BDA_HALPHA_2,
  BDA_HALPHA_4,
  BDA_HALPHA_MAX,
} HierarchyAlpha;

#endif /* _BDATYPES_ */
