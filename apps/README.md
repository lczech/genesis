genesis applications
====================

This directory is provided for conveniently and quickly developing small applications which use
genesis as their main library.

Reasons to use this method instead of the genesis python bindings include:

* If Boost Python is not available on your target system.
* If not all needed functions of genesis are exported to python (yet).
* If you simply feel more comfortable developing in C++ than in python.

To create a new application, create a `*.cpp` file in the `apps` directory.
It is automatically compiled by the main build process and linked against the shared library
version of genesis.
The resulting binary is placed in `bin` and named after the `*.cpp` file (without the ending).

As each such file is an executable of its own, it needs a main function:

    #include "tree/tree.hpp"

    int main( int argc, const char* argv[] )
    {
    	// [Your code here.]
    }

Apart from that, there are no further requirements. Simply include the needed genesis headers (or
any other headers you might need) and compile. Happy coding!
