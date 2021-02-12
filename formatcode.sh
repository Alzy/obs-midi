#!/usr/bin/env bash
# Original source https://github.com/Project-OSRM/osrm-backend/blob/master/scripts/format.sh

set +x
set -o errexit
set -o pipefail
set -o nounset

# Runs the Clang Formatter in parallel on the code base.
# Return codes:
#  - 1 there are files to be formatted
#  - 0 everything looks fine

# Get CPU count
OS=$(uname)
NPROC=1
if [[ $OS = "Linux" || $OS = "Darwin" ]] ; then
    NPROC=$(getconf _NPROCESSORS_ONLN)
fi

# Discover clang-format


find . -type d \( -o -path ./src -o -path ./src/forms \) -prune -type f -o -name '*.h' -or -name '*.hpp' -or -name '*.m' -or -name '*.mm' -or -name '*.c' -or -name '*.cpp' >tmpfile
while IFS= read -r line; do   clang-format-12 -i -style=file  -fallback-style=none  $line; done < tmpfile
