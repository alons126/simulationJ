#!/bin/csh

# Setup
# ---------------------------------------------------------------------------

# Colors (override by exporting COLOR_START / COLOR_END before running)
unsetenv COLOR_START
setenv COLOR_START \033[35m

unsetenv COLOR_END
setenv COLOR_END \033[0m

unsetenv COLOR_ERROR_START
setenv COLOR_ERROR_START \033[31m

# Functions
# ---------------------------------------------------------------------------
banner() {
    echo ""
    echo "${COLOR_START}=======================================================================${COLOR_END}"
    printf "%s%s%s\n" "${COLOR_START}= " "$1" " =${COLOR_END}"
    echo "${COLOR_START}=======================================================================${COLOR_END}"
    echo ""
}

Check_if_dir_exist() {
    if (! -d "${1}") then
        echo "${COLOR_ERROR_START}Error:${COLOR_END} the following directory does not exist: ${1}"
        exit 1
    endif
}

Check_if_file_exist() {
    if (! -f "${1}") then
        echo "${COLOR_ERROR_START}Error:${COLOR_END} the following file does not exist: ${1}"
        exit 1
    endif
}
