Android's NDK didn't include glob until Android 9 (API level 28),
which was released in 2018. For the sake of ease, if we're building
for Android, add the glob implementation from later Android code.

Note that these files are licensed under BSD-3.
