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
add_subdirectory(python)