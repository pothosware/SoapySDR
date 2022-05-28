########################################################################
# Java support (optional)
########################################################################
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Java support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Java bindings if possible...")
add_subdirectory(java)

########################################################################
# Python support (optional)
########################################################################
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Python support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Python bindings if possible...")
add_subdirectory(python)

message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for Python3 support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional Python3 bindings if possible...")
add_subdirectory(python3)
