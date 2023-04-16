#ifndef COMPAT_UNDOC_H
#define COMPAT_UNDOC_H


typedef struct _MergeOS_ShimData
{
    DWORD dwReserved1[130];
    DWORD dwSize;
    DWORD dwMagic;
    DWORD dwReserved2[242];
    DWORD dwRosProcessCompatVersion;
} MergeOS_ShimData;


#define MERGEOS_SHIMDATA_MAGIC  0xAC0DEDAB
#define MERGEOS_COMPATVERSION_UNINITIALIZED 0xfffffffe
#define MERGEOS_COMPATVERSION_IGNOREMANIFEST 0xffffffff

// Returns values in the form of _WIN32_WINNT_VISTA, _WIN32_WINNT_WIN7 etc
static
inline
DWORD RosGetProcessCompatVersion(VOID)
{
    static DWORD g_CompatVersion = MERGEOS_COMPATVERSION_UNINITIALIZED;
    if (g_CompatVersion == MERGEOS_COMPATVERSION_UNINITIALIZED)
    {
        MergeOS_ShimData* pShimData = (MergeOS_ShimData*)NtCurrentPeb()->pShimData;
        if (pShimData && pShimData->dwMagic == MERGEOS_SHIMDATA_MAGIC &&
            pShimData->dwSize == sizeof(MergeOS_ShimData))
        {
            g_CompatVersion = pShimData->dwRosProcessCompatVersion;
        }
    }
    return g_CompatVersion < MERGEOS_COMPATVERSION_UNINITIALIZED ? g_CompatVersion : 0;
}


#endif // COMPAT_UNDOC_H
