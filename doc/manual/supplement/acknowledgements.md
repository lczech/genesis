Acknowledgements {#supplement_acknowledgements}
================

# Acknowledgements @anchor supplement_acknowledgements_acknowledgements

This work is financially supported by the
[Klaus Tschira Foundation](http://www.klaus-tschira-stiftung.de/) and the
[HITS gGmbH - Heidelberg Institute for Theoretical Studies](http://www.h-its.org).

See the webpage of the [Exelixis Lab](http://exelixis-lab.org/) for our other projects.

# Dependencies @anchor supplement_acknowledgements_dependencies

In the following, we list all direct dependencies of Genesis and their licenses.

## GTest @anchor supplement_acknowledgements_dependencies_gtest

If tests are enabled, Genesis automatically tries to download and compile
[Google Test](https://github.com/google/googletest) (GTest) in order to
compile our test cases.

<blockquote>
Copyright 2008, Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

 * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
 * Neither the name of Google Inc. nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</blockquote>

The license information above was copied from
https://github.com/google/googletest/blob/master/googletest/LICENSE
on 2017-03-09.

## Pybind11 @anchor supplement_acknowledgements_dependencies_pybind

If Python bindings are enabled, Genesis automatically tries to download
[Pybind11](https://github.com/pybind/pybind11) to create those bindings.

<blockquote>
Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>, All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

You are under no obligation whatsoever to provide any bug fixes, patches, or
upgrades to the features, functionality or performance of the source code
("Enhancements") to anyone; however, if you choose to make your Enhancements
available either publicly, or directly to the author of this software, without
imposing a separate written license agreement for such Enhancements, then you
hereby grant the following license: a non-exclusive, royalty-free perpetual
license to install, use, modify, prepare derivative works, incorporate into
other computer software, distribute, and sublicense such enhancements or
derivative works thereof, in binary and source code form.
</blockquote>

The license information above was copied from https://github.com/pybind/pybind11/blob/master/LICENSE
on 2017-03-09.

# Code Reuse @anchor supplement_acknowledgements_code_reuse

In the following, we list all sources that were adapted and incorporated in Genesis in modified
form.

## Input Stream @anchor supplement_acknowledgements_code_reuse_input_stream

Genesis contains a set of classes that deal with input streams, i.e.,
@link genesis::utils::InputStream InputStream@endlink,
@link genesis::utils::SynchronousReader SynchronousReader@endlink,
@link genesis::utils::BaseInputSource BaseInputSource@endlink,
@link genesis::utils::StringInputSource StringInputSource@endlink,
@link genesis::utils::StreamInputSource StreamInputSource@endlink and
@link genesis::utils::FileInputSource FileInputSource@endlink.
<!-- @link genesis::utils::AsynchronousReader AsynchronousReader@endlink, -->

The design and implementation of those classes is inspired by
[fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser,
with his explicit permission to modify and use the code in Genesis.

<blockquote>
Copyright (c) 2015, ben-strasser
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of fast-cpp-csv-parser nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</blockquote>

The license information above was copied from
https://github.com/ben-strasser/fast-cpp-csv-parser/blob/master/LICENSE
on 2017-03-09.

## Json Document @anchor supplement_acknowledgements_code_reuse_json_document

Our @link genesis::utils::JsonDocument JsonDocument@endlink data structure as well as the accompanying @link genesis::utils::JsonIterator JsonIterator@endlink are based on the excellent
[JSON for Modern C++](https://github.com/nlohmann/json) library by Niels Lohmann.

<blockquote>
MIT License

Copyright (c) 2013-2017 Niels Lohmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
</blockquote>

The license information above was copied from
https://github.com/nlohmann/json/blob/develop/LICENSE.MIT
on 2017-03-09.

## Succinct Range Minimum Query @anchor supplement_acknowledgements_code_reuse_succinct_rmq

The implementation of our @link genesis::utils::RangeMinimumQuery RangeMinimumQuery@endlink data
structure is based on the
[Succinct RMQ implementation](https://www.bio.ifi.lmu.de/forschung/succinct/#software) by Johannes
Fischer, with his
explicit permission to use his code in Genesis.

## SHA1 @anchor supplement_acknowledgements_code_reuse_sha

Our implementation of the @link genesis::utils::SHA1 SHA1 hashing function@endlink is based on
[https://github.com/vog/sha1](https://github.com/vog/sha1), which is 100% Public Domain.

## Matplotlib Color Maps @anchor supplement_acknowledgements_code_reuse_matplotlib_color_maps

The color codes for the color maps @link genesis::utils::magma magma@endlink,
@link genesis::utils::inferno inferno@endlink, @link genesis::utils::plasma plasma@endlink and
@link genesis::utils::viridis viridis@endlink are taken from https://github.com/BIDS/colormap.
They were created by Nathaniel J. Smith, Stefan van der Walt, and (in the case of viridis)
Eric Firing. These colormaps are released under the CC0 license / public domain dedication.
See https://creativecommons.org/publicdomain/zero/1.0/ for the license.
