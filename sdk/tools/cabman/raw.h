/*
 * COPYRIGHT:   See COPYING in the top level directory
 * PROJECT:     MergeOS cabinet manager
 * FILE:        tools/cabman/raw.h
 * PURPOSE:     CAB codec for uncompressed data
 */

#pragma once

#include "cabinet.h"


/* Classes */

class CRawCodec : public CCABCodec
{
public:
    /* Default constructor */
    CRawCodec();
    /* Default destructor */
    virtual ~CRawCodec();
    /* Compresses a data block */
    virtual ULONG Compress(void* OutputBuffer,
                           void* InputBuffer,
                           ULONG InputLength,
                           PULONG OutputLength) override;
    /* Uncompresses a data block */
    virtual ULONG Uncompress(void* OutputBuffer,
                             void* InputBuffer,
                             ULONG InputLength,
                             PULONG OutputLength) override;
};

/* EOF */
