########################################################################
# Build the Converter Registry plugin module
########################################################################
SOAPY_SDR_MODULE_UTIL(
  TARGET converterRegistry
  SOURCES
    DefaultConverters.cpp
    # Registration.cpp
    # Settings.cpp
    # Streaming.cpp
    # LogAcceptor.cpp
    # ClientStreamData.cpp
  # LIBRARIES
  #   SoapySDRRemoteCommon
)
