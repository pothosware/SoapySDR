import os
import sys
import site
from distutils.sysconfig import get_python_lib

if __name__ == '__main__':
    prefix = sys.argv[1]

    #use sites when the prefix is already recognized
    paths = [p for p in site.getsitepackages() if p.startswith(prefix)]
    if len(paths) == 1: install_dir = paths[0]

    #ask distutils where to install the python module
    else: install_dir = get_python_lib(plat_specific=True, prefix=prefix)

    #strip the prefix to return a relative path
    print(os.path.relpath(install_dir, prefix))
