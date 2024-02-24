MacOS and GDB
=============

MacOS does not allow gdb to run unless it is codesigned.
We need this for the automated test in `genesis/.github/workflows/ci.yaml`
in our GitHub Actions MacOS runners. See the CI setup for our usage.

The files and scrips in this directory are from https://sourceware.org/gdb/wiki/PermissionsDarwin.
See there for details on the process.
