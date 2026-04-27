set PART     "xc7z020clg400-1"
set CLOCK_NS "15"
set TOP      "sort_top"
set PROJ     "hls_bitonic_sort"
set SRC_DIR  [expr {[info exists env(HLS_SRC_DIR)] ? $env(HLS_SRC_DIR) : "../src"}]

open_project ${PROJ} -reset ;# reset the project and synthesize from scratch
set_top ${TOP}
add_files [glob ${SRC_DIR}/*.cpp]

open_solution "sol1" -flow_target vivado -reset
set_part ${PART}

create_clock -period ${CLOCK_NS} -name default

csynth_design

export_design -format ip_catalog \
  -description "Bitonic Sort IP" \
  -vendor "ee5332" -library "hls" -version "1.0"

exit
