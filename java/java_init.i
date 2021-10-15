/*
 * Copyright (c) 2016 Nicholas Corgan (n.corgan@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

/*
 * This file supplies the macro JAVA_INIT, which takes in a SWIG module name
 * and tells the Java JAR to load the module's native library when it is
 * imported.
 *
 * Example: JAVA_INIT("module_name")
 */

#ifndef SWIGJAVA
#error java_init.i is only meant to be used with Java modules.
#endif

%define JAVA_INIT(libname)
    %pragma(java) jniclasscode=%{
        static {
            try {
                System.loadLibrary(libname);
            } catch (UnsatisfiedLinkError e) {
                System.err.println("Failed to load " + libname + " with error: " + e);
                System.exit(1);
            }
        }
    %}
%enddef
