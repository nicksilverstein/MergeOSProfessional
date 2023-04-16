
add_definitions(
    -DISOHYBRID_C_STANDALONE)

add_host_tool(isohybrid
    isohybrid.c
    mergeos_support_code.c
    ../port/getopt.c
    ../port/getopt1.c)
