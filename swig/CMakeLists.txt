########################################################################
# C# support (optional)
########################################################################
message(STATUS "")
message(STATUS "#############################################")
message(STATUS "## Begin configuration for C# support...")
message(STATUS "#############################################")
message(STATUS "Enabling optional C# bindings if possible...")
add_subdirectory(csharp)

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
