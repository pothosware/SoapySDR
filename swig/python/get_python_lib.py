import os
import pathlib
import sys
import sysconfig

if __name__ == '__main__':
    prefix = pathlib.Path(sys.argv[1]).resolve()

    # default install dir for the running Python interpreter
    default_install_dir = pathlib.Path(sysconfig.get_path('platlib')).resolve()

    # if default falls under the desired prefix, we're done
    try:
        relative_install_dir = default_install_dir.relative_to(prefix)
    except ValueError:
        # get install dir for the specified prefix
        # can't use the default scheme because distributions modify it
        # newer Python versions have 'venv' scheme, use for all OSs.
        if 'venv' in sysconfig.get_scheme_names():
            scheme = 'venv'
        elif os.name == 'nt':
            scheme = 'nt'
        else:
            scheme = 'posix_prefix'
        prefix_install_dir = pathlib.Path(sysconfig.get_path(
            'platlib',
            scheme=scheme,
            vars={'base': prefix, 'platbase': prefix},
        )).resolve()
        relative_install_dir = prefix_install_dir.relative_to(prefix)

    # want a relative path for use in the build system
    print(relative_install_dir)
