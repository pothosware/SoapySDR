#!/bin/bash
#
# See: https://buildkite.com/docs/guides/writing-build-scripts
#
# TL;DR
#
# # You can group your build output by echoing "--- group name" in your build output.
#
# echo "--- build group here"
# # If you want to have the group open by default, use +++ instead of ---:
#
# echo "+++ build group here"
# # You can even include colors and emojis!
#
# echo -e "--- Running \033[33mspecs\033[0m \:cow:\:bell:"
#


#
# Add a buildkite format comment with each new item added.
# This script is run from the top directory of the FPGA repo.
#
set -eo pipefail

echo "--- Environment."
env

#
# Add hook(s) to build commands/script here.
#
echo "--- Build SoapySDR"
rm -rf build
mkdir build
cd build
cmake ../
make -j4

# Got to the end, all is good, close out last buildkite output group.
echo "--- BUILD SUCCEEDED"
exit 0
