
include_directories(
    ${MERGEOS_SOURCE_DIR}/drivers/filesystems/btrfs)

list(APPEND SOURCE
    ${MERGEOS_SOURCE_DIR}/drivers/filesystems/btrfs/blake2b-ref.c
    ${MERGEOS_SOURCE_DIR}/drivers/filesystems/btrfs/crc32c.c
    ${MERGEOS_SOURCE_DIR}/drivers/filesystems/btrfs/sha256.c
    ${MERGEOS_SOURCE_DIR}/drivers/filesystems/btrfs/xxhash.c
    btrfslib.c)

if((ARCH STREQUAL "i386") OR (ARCH STREQUAL "amd64"))
    list(APPEND ASM_SOURCE ${MERGEOS_SOURCE_DIR}/drivers/filesystems/btrfs/crc32c.S)
    add_asm_files(btrfs_asm ${ASM_SOURCE})
endif()

add_library(btrfslib ${SOURCE} ${btrfs_asm})
target_compile_definitions(btrfslib PRIVATE _USRDLL)
add_dependencies(btrfslib psdk)
