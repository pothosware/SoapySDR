# Build Python version 3 bindings only

The python3/ directory is essentially a copy of the top level python/ directory,
however it looks solely for the Python version 3 executable and development files.
This directory is only activated when the top level python/ directory
is configured to build for a Python version 2 environment.

The purpose of the python3/ directory is allow for building both
Python version 2 and Python version 3 language bindings
on systems that support both Python installs simultaneously.
This also allows the debian packaging to build debs for both versions as well.
