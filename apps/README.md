genesis applications
====================

This directory is provided for conveniently and quickly developing small applications which use
genesis as their main C++ library. This is a way of using C++ similar to a scripting language.

Reasons to use this method (instead of the genesis python bindings) include:

 *  If Boost Python is not available on your target system.
 *  If not all needed functions of genesis are exported to python (yet).
 *  If you simply feel more comfortable developing in C++ than in python.

To create a new application, create a `*.cpp` file in the `./apps` directory and provide it with
a main function:

    int main( int argc, const char* argv[] )
    {
    	// [Your code here.]
    }

Include all needed headers and write the desired functionality.
Of course, you can use other local functions in addition to the main function in order to structure
your code.

The app is automatically compiled by the main build process and linked against the shared library
version of genesis by calling

    make update

in the main directory. The resulting executable is placed in `./bin` and named after the `*.cpp`
file (without the ending).

Apart from that, there are no further requirements. Simply include the needed genesis headers (or
any other headers you might need) and compile. Happy coding!
