Acknowledgements {#supplement_acknowledgements}
================

# Acknowledgements @anchor supplement_acknowledgements_acknowledgements

This work is financially supported:

  * 2014-2019: [Klaus Tschira Foundation](http://www.klaus-tschira-stiftung.de/) and
    [HITS gGmbH - Heidelberg Institute for Theoretical Studies](http://www.h-its.org)
    in Heiderberg, Germany.
  * 2020-2021: [Carnegie Institution For Science](https://carnegiescience.edu/) in Stanford, USA.

See the webpages of the [Exelixis Lab](http://exelixis-lab.org/) and the
[Moi Lab](http://moisesexpositoalonso.org/) for our other projects.

# Dependencies @anchor supplement_acknowledgements_dependencies

In the following, we list all direct dependencies of Genesis and their licenses.

## GTest @anchor supplement_acknowledgements_dependencies_gtest

If tests are enabled, Genesis automatically tries to download and compile
[Google Test](https://github.com/google/googletest) (GTest) in order to
compile our test cases.

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Copyright 2008, Google Inc.
>    All rights reserved.
>
>    Redistribution and use in source and binary forms, with or without
>    modification, are permitted provided that the following conditions are
>    met:
>
>     * Redistributions of source code must retain the above copyright
>    notice, this list of conditions and the following disclaimer.
>     * Redistributions in binary form must reproduce the above
>    copyright notice, this list of conditions and the following disclaimer
>    in the documentation and/or other materials provided with the
>    distribution.
>     * Neither the name of Google Inc. nor the names of its
>    contributors may be used to endorse or promote products derived from
>    this software without specific prior written permission.
>
>    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
>    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
>    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
>    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
>    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
>    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
>    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
>    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
>    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
>    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
>    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/google/googletest/blob/master/googletest/LICENSE
on 2017-03-09.

## Pybind11 @anchor supplement_acknowledgements_dependencies_pybind

If Python bindings are enabled, Genesis automatically tries to download
[Pybind11](https://github.com/pybind/pybind11) to create those bindings.

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>, All rights reserved.
>
>    Redistribution and use in source and binary forms, with or without
>    modification, are permitted provided that the following conditions are met:
>
>    1. Redistributions of source code must retain the above copyright notice, this
>       list of conditions and the following disclaimer.
>
>    2. Redistributions in binary form must reproduce the above copyright notice,
>       this list of conditions and the following disclaimer in the documentation
>       and/or other materials provided with the distribution.
>
>    3. Neither the name of the copyright holder nor the names of its contributors
>       may be used to endorse or promote products derived from this software
>       without specific prior written permission.
>
>    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
>    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
>    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
>    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
>    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
>    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
>    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
>    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
>    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
>    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
>
>    You are under no obligation whatsoever to provide any bug fixes, patches, or
>    upgrades to the features, functionality or performance of the source code
>    ("Enhancements") to anyone; however, if you choose to make your Enhancements
>    available either publicly, or directly to the author of this software, without
>    imposing a separate written license agreement for such Enhancements, then you
>    hereby grant the following license: a non-exclusive, royalty-free perpetual
>    license to install, use, modify, prepare derivative works, incorporate into
>    other computer software, distribute, and sublicense such enhancements or
>    derivative works thereof, in binary and source code form.
@htmlonly </details> @endhtmlonly

The license information above was copied from https://github.com/pybind/pybind11/blob/master/LICENSE
on 2017-03-09.

## htslib @anchor supplement_acknowledgements_dependencies_htslib

We do not deliver any code of htslib with our sources, but
if htslib is enabled (in `CMakeLists.txt` via `GENESIS_USE_HTSLIB`),
genesis automatically downloads and builds [htslib](https://github.com/samtools/htslib)
as part of its own build process.

@htmlonly <details><summary>License</summary> @endhtmlonly
>    [Files in this distribution outwith the cram/ subdirectory are distributed
>    according to the terms of the following MIT/Expat license.]
>
>    The MIT/Expat License
>
>    Copyright (C) 2012-2021 Genome Research Ltd.
>
>    Permission is hereby granted, free of charge, to any person obtaining a copy
>    of this software and associated documentation files (the "Software"), to deal
>    in the Software without restriction, including without limitation the rights
>    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>    copies of the Software, and to permit persons to whom the Software is
>    furnished to do so, subject to the following conditions:
>
>    The above copyright notice and this permission notice shall be included in
>    all copies or substantial portions of the Software.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
>    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
>    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
>    DEALINGS IN THE SOFTWARE.
>
>
>    [Files within the cram/ subdirectory in this distribution are distributed
>    according to the terms of the following Modified 3-Clause BSD license.]
>
>    The Modified-BSD License
>
>    Copyright (C) 2012-2020 Genome Research Ltd.
>
>    Redistribution and use in source and binary forms, with or without
>    modification, are permitted provided that the following conditions are met:
>
>    1. Redistributions of source code must retain the above copyright notice,
>       this list of conditions and the following disclaimer.
>
>    2. Redistributions in binary form must reproduce the above copyright notice,
>       this list of conditions and the following disclaimer in the documentation
>       and/or other materials provided with the distribution.
>
>    3. Neither the names Genome Research Ltd and Wellcome Trust Sanger Institute
>       nor the names of its contributors may be used to endorse or promote products
>       derived from this software without specific prior written permission.
>
>    THIS SOFTWARE IS PROVIDED BY GENOME RESEARCH LTD AND CONTRIBUTORS "AS IS"
>    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
>    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
>    DISCLAIMED. IN NO EVENT SHALL GENOME RESEARCH LTD OR ITS CONTRIBUTORS BE LIABLE
>    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
>    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
>    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
>    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
>    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
>    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
>
>
>    [The use of a range of years within a copyright notice in this distribution
>    should be interpreted as being equivalent to a list of years including the
>    first and last year specified and all consecutive years between them.
>
>    For example, a copyright notice that reads "Copyright (C) 2005, 2007-2009,
>    2011-2012" should be interpreted as being identical to a notice that reads
>    "Copyright (C) 2005, 2007, 2008, 2009, 2011, 2012" and a copyright notice
>    that reads "Copyright (C) 2005-2012" should be interpreted as being identical
>    to a notice that reads "Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010,
>    2011, 2012".]
@htmlonly </details> @endhtmlonly

The license information above was copied from https://github.com/samtools/htslib/blob/develop/LICENSE
on 2021-09-18.

# Code Reuse @anchor supplement_acknowledgements_code_reuse

In the following, we list all sources that were adapted and incorporated in Genesis in modified
form.

## Json Document @anchor supplement_acknowledgements_code_reuse_json_document

Our @link genesis::utils::JsonDocument JsonDocument@endlink data structure as well as the accompanying @link genesis::utils::JsonIterator JsonIterator@endlink are based on the excellent
[JSON for Modern C++](https://github.com/nlohmann/json) library by Niels Lohmann.

@htmlonly <details><summary>License</summary> @endhtmlonly
>    MIT License
>
>    Copyright (c) 2013-2017 Niels Lohmann
>
>    Permission is hereby granted, free of charge, to any person obtaining a copy
>    of this software and associated documentation files (the "Software"), to deal
>    in the Software without restriction, including without limitation the rights
>    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>    copies of the Software, and to permit persons to whom the Software is
>    furnished to do so, subject to the following conditions:
>
>    The above copyright notice and this permission notice shall be included in all
>    copies or substantial portions of the Software.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
>    SOFTWARE.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/nlohmann/json/blob/develop/LICENSE.MIT
on 2017-03-09.

## Input Stream @anchor supplement_acknowledgements_code_reuse_input_stream

Genesis contains a set of classes that deal with input streams:

  * @link genesis::utils::InputStream InputStream@endlink
  * @link genesis::utils::SynchronousReader SynchronousReader@endlink
  * @link genesis::utils::BaseInputSource BaseInputSource@endlink
  * @link genesis::utils::StringInputSource StringInputSource@endlink
  * @link genesis::utils::StreamInputSource StreamInputSource@endlink
  * @link genesis::utils::FileInputSource FileInputSource@endlink
  * @link genesis::utils::AsynchronousReader AsynchronousReader@endlink

The design and implementation of those classes is inspired by
[fast-cpp-csv-parser](https://github.com/ben-strasser/fast-cpp-csv-parser) by Ben Strasser,
with his explicit permission to modify and use the code in Genesis.

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Copyright (c) 2015, ben-strasser
>    All rights reserved.
>
>    Redistribution and use in source and binary forms, with or without
>    modification, are permitted provided that the following conditions are met:
>
>    * Redistributions of source code must retain the above copyright notice, this
>      list of conditions and the following disclaimer.
>
>    * Redistributions in binary form must reproduce the above copyright notice,
>      this list of conditions and the following disclaimer in the documentation
>      and/or other materials provided with the distribution.
>
>    * Neither the name of fast-cpp-csv-parser nor the names of its
>      contributors may be used to endorse or promote products derived from
>      this software without specific prior written permission.
>
>    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
>    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
>    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
>    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
>    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
>    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
>    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
>    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
>    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
>    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/ben-strasser/fast-cpp-csv-parser/blob/master/LICENSE
on 2017-03-09.

## Gzip Streams @anchor supplement_acknowledgements_code_reuse_gzip_streams

Genesis contains a set of classes that deal with gzip/zlib compressed/decompressed input streams,
and some other stream-related functionality:

  * @link genesis::utils::GzipIStream GzipIStream@endlink
  * @link genesis::utils::GzipOStream GzipOStream@endlink
  * @link genesis::utils::GzipIFStream GzipIFStream@endlink
  * @link genesis::utils::GzipOFStream GzipOFStream@endlink
  * @link genesis::utils::StrictIFStream StrictIFStream@endlink
  * @link genesis::utils::StrictOFStream StrictOFStream@endlink
  * @link genesis::utils::StrictFStream StrictFStream@endlink

as well as the helper and internal classes:

  * @link genesis::utils::GzipStreamWrapper GzipStreamWrapper@endlink
  * @link genesis::utils::GzipIStreambuf GzipIStreambuf@endlink
  * @link genesis::utils::GzipOStreambuf GzipOStreambuf@endlink
  * @link genesis::utils::StrictFStreamHolder StrictFStreamHolder@endlink

The code of these classes is adapted from the excellent
[zstr library](https://github.com/mateidavid/zstr) by Matei David.
We adapted the original code by renaming all classes and variables to our standards,
moving much of the implementation into a source file (so that the header does not clutter
its callers with zlib-internal symbols), and refining some functionality.

The repository at https://github.com/mateidavid/zstr is published under the
[MIT license](https://opensource.org/licenses/MIT):

@htmlonly <details><summary>License</summary> @endhtmlonly
>    The MIT License (MIT)
>
>    Copyright (c) 2015 Matei David, Ontario Institute for Cancer Research
>
>    Permission is hereby granted, free of charge, to any person obtaining a copy
>    of this software and associated documentation files (the "Software"), to deal
>    in the Software without restriction, including without limitation the rights
>    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>    copies of the Software, and to permit persons to whom the Software is
>    furnished to do so, subject to the following conditions:
>
>    The above copyright notice and this permission notice shall be included in all
>    copies or substantial portions of the Software.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
>    SOFTWARE.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/mateidavid/zstr/blob/master/LICENSE
on 2020-05-10.

## optional-bare @anchor supplement_acknowledgements_code_reuse_optional_bare

Genesis contains an implementation for optional data:

  * @link genesis::utils::Optional Optional@endlink

The code of this class is adapted from the excellent
[optional-bare](https://github.com/martinmoene/optional-bare) by Martin Moene.
We adapted the original code by renaming the classes and variables and formatting the code
to our standards.

All code for this class, located in `genesis/lib/genesis/utils/containers/optional.hpp` is

>    Copyright 2017-2019 by Martin Moene.
>    Distributed under the Boost Software License, Version 1.0.
>    See http://www.boost.org/LICENSE_1_0.txt

The original code repository, located at https://github.com/martinmoene/optional-bare,
is published under the [Boost Software License - Version 1.0](http://www.boost.org/LICENSE_1_0.txt):

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Boost Software License - Version 1.0 - August 17th, 2003
>
>    Permission is hereby granted, free of charge, to any person or organization
>    obtaining a copy of the software and accompanying documentation covered by
>    this license (the "Software") to use, reproduce, display, distribute,
>    execute, and transmit the Software, and to prepare derivative works of the
>    Software, and to permit third-parties to whom the Software is furnished to
>    do so, all subject to the following:
>
>    The copyright notices in the Software and this entire statement, including
>    the above license grant, this restriction and the following disclaimer,
>    must be included in all copies of the Software, in whole or in part, and
>    all derivative works of the Software, unless such copies or derivative
>    works are solely in the form of machine-executable object code generated by
>    a source language processor.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
>    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
>    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
>    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
>    DEALINGS IN THE SOFTWARE.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/martinmoene/optional-bare/blob/master/LICENSE.txt
on 2021-09-18.

## interval-tree @anchor supplement_acknowledgements_code_reuse_interval_tree

Genesis contains an implementation of an interval tree:

  * @link genesis::utils::Interval Interval@endlink
  * @link genesis::utils::IntervalTree IntervalTree@endlink
  * @link genesis::utils::IntervalTreeNode IntervalTreeNode@endlink
  * @link genesis::utils::IntervalTreeIterator IntervalTreeIterator@endlink

The code of these classes is adapted from
[interval-tree](https://github.com/5cript/interval-tree) by
Tim Ebbeke ([\@5cript](https://github.com/5cript)) and
Nick Weber ([\@WeberNick](https://github.com/WeberNick)).
We adapted the original code by renaming the classes and variables and formatting the code
to our standards, as well as improving some of the functions and adding additional functionality
for our needs.

The original code repository, located at https://github.com/5cript/interval-tree,
is published under the CC0-1.0 License (Creative Commons Zero v1.0 Universal):

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Creative Commons Legal Code
>
>    CC0 1.0 Universal
>
>        CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
>        LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
>        ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
>        INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
>        REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
>        PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
>        THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
>        HEREUNDER.
>
>    Statement of Purpose
>
>    The laws of most jurisdictions throughout the world automatically confer
>    exclusive Copyright and Related Rights (defined below) upon the creator
>    and subsequent owner(s) (each and all, an "owner") of an original work of
>    authorship and/or a database (each, a "Work").
>
>    Certain owners wish to permanently relinquish those rights to a Work for
>    the purpose of contributing to a commons of creative, cultural and
>    scientific works ("Commons") that the public can reliably and without fear
>    of later claims of infringement build upon, modify, incorporate in other
>    works, reuse and redistribute as freely as possible in any form whatsoever
>    and for any purposes, including without limitation commercial purposes.
>    These owners may contribute to the Commons to promote the ideal of a free
>    culture and the further production of creative, cultural and scientific
>    works, or to gain reputation or greater distribution for their Work in
>    part through the use and efforts of others.
>
>    For these and/or other purposes and motivations, and without any
>    expectation of additional consideration or compensation, the person
>    associating CC0 with a Work (the "Affirmer"), to the extent that he or she
>    is an owner of Copyright and Related Rights in the Work, voluntarily
>    elects to apply CC0 to the Work and publicly distribute the Work under its
>    terms, with knowledge of his or her Copyright and Related Rights in the
>    Work and the meaning and intended legal effect of CC0 on those rights.
>
>    1. Copyright and Related Rights. A Work made available under CC0 may be
>    protected by copyright and related or neighboring rights ("Copyright and
>    Related Rights"). Copyright and Related Rights include, but are not
>    limited to, the following:
>
>      i. the right to reproduce, adapt, distribute, perform, display,
>         communicate, and translate a Work;
>     ii. moral rights retained by the original author(s) and/or performer(s);
>    iii. publicity and privacy rights pertaining to a person's image or
>         likeness depicted in a Work;
>     iv. rights protecting against unfair competition in regards to a Work,
>         subject to the limitations in paragraph 4(a), below;
>      v. rights protecting the extraction, dissemination, use and reuse of data
>         in a Work;
>     vi. database rights (such as those arising under Directive 96/9/EC of the
>         European Parliament and of the Council of 11 March 1996 on the legal
>         protection of databases, and under any national implementation
>         thereof, including any amended or successor version of such
>         directive); and
>    vii. other similar, equivalent or corresponding rights throughout the
>         world based on applicable law or treaty, and any national
>         implementations thereof.
>
>    2. Waiver. To the greatest extent permitted by, but not in contravention
>    of, applicable law, Affirmer hereby overtly, fully, permanently,
>    irrevocably and unconditionally waives, abandons, and surrenders all of
>    Affirmer's Copyright and Related Rights and associated claims and causes
>    of action, whether now known or unknown (including existing as well as
>    future claims and causes of action), in the Work (i) in all territories
>    worldwide, (ii) for the maximum duration provided by applicable law or
>    treaty (including future time extensions), (iii) in any current or future
>    medium and for any number of copies, and (iv) for any purpose whatsoever,
>    including without limitation commercial, advertising or promotional
>    purposes (the "Waiver"). Affirmer makes the Waiver for the benefit of each
>    member of the public at large and to the detriment of Affirmer's heirs and
>    successors, fully intending that such Waiver shall not be subject to
>    revocation, rescission, cancellation, termination, or any other legal or
>    equitable action to disrupt the quiet enjoyment of the Work by the public
>    as contemplated by Affirmer's express Statement of Purpose.
>
>    3. Public License Fallback. Should any part of the Waiver for any reason
>    be judged legally invalid or ineffective under applicable law, then the
>    Waiver shall be preserved to the maximum extent permitted taking into
>    account Affirmer's express Statement of Purpose. In addition, to the
>    extent the Waiver is so judged Affirmer hereby grants to each affected
>    person a royalty-free, non transferable, non sublicensable, non exclusive,
>    irrevocable and unconditional license to exercise Affirmer's Copyright and
>    Related Rights in the Work (i) in all territories worldwide, (ii) for the
>    maximum duration provided by applicable law or treaty (including future
>    time extensions), (iii) in any current or future medium and for any number
>    of copies, and (iv) for any purpose whatsoever, including without
>    limitation commercial, advertising or promotional purposes (the
>    "License"). The License shall be deemed effective as of the date CC0 was
>    applied by Affirmer to the Work. Should any part of the License for any
>    reason be judged legally invalid or ineffective under applicable law, such
>    partial invalidity or ineffectiveness shall not invalidate the remainder
>    of the License, and in such case Affirmer hereby affirms that he or she
>    will not (i) exercise any of his or her remaining Copyright and Related
>    Rights in the Work or (ii) assert any associated claims and causes of
>    action with respect to the Work, in either case contrary to Affirmer's
>    express Statement of Purpose.
>
>    4. Limitations and Disclaimers.
>
>     a. No trademark or patent rights held by Affirmer are waived, abandoned,
>        surrendered, licensed or otherwise affected by this document.
>     b. Affirmer offers the Work as-is and makes no representations or
>        warranties of any kind concerning the Work, express, implied,
>        statutory or otherwise, including without limitation warranties of
>        title, merchantability, fitness for a particular purpose, non
>        infringement, or the absence of latent or other defects, accuracy, or
>        the present or absence of errors, whether or not discoverable, all to
>        the greatest extent permissible under applicable law.
>     c. Affirmer disclaims responsibility for clearing rights of other persons
>        that may apply to the Work or any use thereof, including without
>        limitation any person's Copyright and Related Rights in the Work.
>        Further, Affirmer disclaims responsibility for obtaining any necessary
>        consents, permissions or other rights required for any use of the
>        Work.
>     d. Affirmer understands and acknowledges that Creative Commons is not a
>        party to this document and has no duty or obligation with respect to
>        this CC0 or use of the Work.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/5cript/interval-tree/blob/master/LICENSE
on 2021-12-04.

## threadsafe queue @anchor supplement_acknowledgements_code_reuse_threadsafe_queue

Genesis contains an implementation of a thread-safe queue:

  * @link genesis::utils::ThreadsafeQueue ThreadsafeQueue@endlink

This implementation is from the excellent "C++ Concurrency in Action" book by Anthony Williams, second edition, chapter 4, listing 4.5. The code is published at https://github.com/anthonywilliams/ccia_code_samples under the [Boost Software License](https://www.boost.org/LICENSE_1_0.txt).

We adapted the original code by renaming the classes and variables and formatting the code
to our standards, as well as improving some of the functions and adding additional functionality
for our needs.

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Boost Software License - Version 1.0 - August 17th, 2003
>
>    Permission is hereby granted, free of charge, to any person or organization
>    obtaining a copy of the software and accompanying documentation covered by
>    this license (the "Software") to use, reproduce, display, distribute,
>    execute, and transmit the Software, and to prepare derivative works of the
>    Software, and to permit third-parties to whom the Software is furnished to
>    do so, all subject to the following:
>
>    The copyright notices in the Software and this entire statement, including
>    the above license grant, this restriction and the following disclaimer,
>    must be included in all copies of the Software, in whole or in part, and
>    all derivative works of the Software, unless such copies or derivative
>    works are solely in the form of machine-executable object code generated by
>    a source language processor.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
>    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
>    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
>    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
>    DEALINGS IN THE SOFTWARE.
@htmlonly </details> @endhtmlonly

## Concurrent Queue @anchor supplement_acknowledgements_code_reuse_concurrent_queue

Genesis contains an implementation of a Concurrent Queue and related classes:

  * @link genesis::utils::ConcurrentQueue ConcurrentQueue@endlink
  * @link genesis::utils::BlockingConcurrentQueue BlockingConcurrentQueue@endlink
  * @link genesis::utils::LightweightSemaphore LightweightSemaphore@endlink

This implementation is from the excellent moodycamel::ConcurrentQueue
(https://github.com/cameron314/concurrentqueue), using version v1.0.4,
which was published under a simplified BSD license, and also dual-licensed under the Boost
Software License. The full [original license](https://github.com/cameron314/concurrentqueue/blob/master/LICENSE.md), as of 2024-07-04, is copied below.

We adapted the original code by (roughly) formatting it to our formatting standard, as well as
renaming the namespace from moodycamel to be contained within our namespace, to keep our
documentation and usage consistent. Other than that, all functionality is kept as-is.

@htmlonly <details><summary>License</summary> @endhtmlonly
>    This license file applies to everything in this repository except that which
>    is explicitly annotated as being written by other authors, i.e. the Boost
>    queue (included in the benchmarks for comparison), Intel's TBB library (ditto),
>    dlib::pipe (ditto),
>    the CDSChecker tool (used for verification), the Relacy model checker (ditto),
>    and Jeff Preshing's semaphore implementation (used in the blocking queue) which
>    has a zlib license (embedded in lightweightsempahore.h).
>
>    ---
>
>    Simplified BSD License:
>
>    Copyright (c) 2013-2016, Cameron Desrochers.
>    All rights reserved.
>
>    Redistribution and use in source and binary forms, with or without modification,
>    are permitted provided that the following conditions are met:
>
>    - Redistributions of source code must retain the above copyright notice, this list of
>    conditions and the following disclaimer.
>    - Redistributions in binary form must reproduce the above copyright notice, this list of
>    conditions and the following disclaimer in the documentation and/or other materials
>    provided with the distribution.
>
>    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
>    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
>    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
>    THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
>    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
>    OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
>    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
>    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
>    EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
>
>    ---
>
>    I have also chosen to dual-license under the Boost Software License as an alternative to
>    the Simplified BSD license above:
>
>    Boost Software License - Version 1.0 - August 17th, 2003
>
>    Permission is hereby granted, free of charge, to any person or organization
>    obtaining a copy of the software and accompanying documentation covered by
>    this license (the "Software") to use, reproduce, display, distribute,
>    execute, and transmit the Software, and to prepare derivative works of the
>    Software, and to permit third-parties to whom the Software is furnished to
>    do so, all subject to the following:
>
>    The copyright notices in the Software and this entire statement, including
>    the above license grant, this restriction and the following disclaimer,
>    must be included in all copies of the Software, in whole or in part, and
>    all derivative works of the Software, unless such copies or derivative
>    works are solely in the form of machine-executable object code generated by
>    a source language processor.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
>    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
>    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
>    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
>    DEALINGS IN THE SOFTWARE.
@htmlonly </details> @endhtmlonly

## Succinct Range Minimum Query @anchor supplement_acknowledgements_code_reuse_succinct_rmq

The implementation of our @link genesis::utils::RangeMinimumQuery RangeMinimumQuery@endlink data
structure is based on the
[Succinct RMQ implementation](https://www.bio.ifi.lmu.de/forschung/succinct/#software) by Johannes
Fischer, with his
explicit permission to use his code in Genesis.

## Multi-Dimensional Scaling (MDS) @anchor supplement_acknowledgements_code_reuse_mds

The implementation of our
@link genesis::utils::multi_dimensional_scaling() multi_dimensional_scaling()@endlink function
is based on the [SimpleMatrix library](https://sites.google.com/site/simpmatrix/)
by [Quan Wang](https://github.com/wq2012), with his explicit permission to use this code in Genesis.
The copyright (C) of the implementation is held by Quan Wang, 2013.
We adapted the implementation to our @link genesis::utils::Matrix Matrix@endlink class and
changed the error reporting mechanism to exceptions.

The implementation by Quan Wang (and our adaptation) offers two variants of MDS. The code of these
variants is based on two Matlab implementations, and re-implements those in C++.
The original sources are:

 * The @link genesis::utils::MdsAlgorithm::kUcf UCF@endlink variant re-implements
   Laurens van der Maaten's MDS in his
   [Matlab Toolbox for Dimensionality Reduction](http://crcv.ucf.edu/source/dimension).
   It is free to use, change, or redistribute this code for non-commercial purposes.
 * The @link genesis::utils::MdsAlgorithm::kSmacof SMACOF@endlink variant re-implements
   Michael Bronstein's SMACOF in his
   [Matlab Toolbox for Surface Comparison and Analysis](http://tosca.cs.technion.ac.il/).
   It may be used for academic purposes, which we consider Genesis to be.

See there for further details. Note however that none of the original code of these two
implementations is used in Genesis, as we solely rely on the ported and re-implemented code
by Quan Wang.

## Generalized Linear Model (GLM) @anchor supplement_acknowledgements_code_reuse_glm

The implementation of our Generalized Linear Model, in particular
@link genesis::utils::glm_fit() glm_fit()@endlink, but also some related helper functions,
is based on the snp.matrix and X.snp.matrix classes by
David Clayton <david.clayton@cimr.cam.ac.uk> and Hin-Tak Leung <htl10@users.sourceforge.net>.
The source is in C, but is originally intended for usage in R.

The package does not seem to be maintained any more, and does not seem to
have a proper repository. For more information, try these sites:
- https://bioconductor.org/packages/release/bioc/html/snpStats.html
- https://www.rdocumentation.org/packages/snpStats/
- http://www-gene.cimr.cam.ac.uk/clayton/software/

The original code is published under the GNU General Public Licence version 3 (GPLv3).

> The snp.matrix and X.snp.matrix classes.
> Copyright (C) 2008 David Clayton and Hin-Tak Leung

As we use the same license, see @ref supplement_license for the complete license.

## GSL (GNU Scientific Library) @anchor supplement_acknowledgements_code_reuse_gsl

The implementation of the multinomial distribution
@link genesis::utils::multinomial_distribution() multinomial_distribution()@endlink
is based on the GSL (GNU Scientific Library) function `gsl_ran_multinomial()`.
We further have an unused function
@link genesis::utils::hypergeometric_distribution_gsl() hypergeometric_distribution_gsl()@endlink
based on `gsl_ran_hypergeometric()` from the GSL.

The original code is published under the GNU General Public Licence version 3 (GPLv3).

> randist/multinomial.c
> Copyright (C) 2002 Gavin E. Crooks <gec@compbio.berkeley.edu>
>
> randist/hyperg.c
> Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough

As we use the same license, see @ref supplement_license for the complete license.

## stocc random project @anchor supplement_acknowledgements_code_reuse_stocc

The implementation of the hypergeometric distribution functions
@link genesis::utils::multivariate_hypergeometric_distribution() multivariate_hypergeometric_distribution()@endlink,
@link genesis::utils::hypergeometric_distribution() hypergeometric_distribution()@endlink,
@link genesis::utils::hypergeometric_distribution_inversion_mode_() hypergeometric_distribution_inversion_mode_()@endlink,
@link genesis::utils::hypergeometric_distribution_ratio_of_unifoms_() hypergeometric_distribution_ratio_of_unifoms_()@endlink,
are based on the stocc.zip non-uniform random number generator functions by Agner Fog.
The original code is published under the GNU General Public Licence.

> Author:        Agner Fog
> Date created:  2002-01-04
> Last modified: 2008-11-30
> Project:       stocc.zip
> Source URL:    www.agner.org/random
>
> Description:
> Non-uniform random number generator functions.
>
> This file contains source code for the class StochasticLib1 defined in stocc.h.
>
> Documentation:
> ==============
> The file stocc.h contains class definitions.
> The file stocc.htm contains further instructions.
> The file distrib.pdf contains definitions of the statistic distributions.
> The file sampmet.pdf contains theoretical descriptions of the methods used
> for sampling from these distributions.
> The file ran-instructions.pdf contains general instructions.
>
> Copyright 2002-2008 by Agner Fog.
> GNU General Public License http://www.gnu.org/licenses/gpl.html

As we use the same license, see @ref supplement_license for the complete license.

## MD5 @anchor supplement_acknowledgements_code_reuse_md

Our implementation of the @link genesis::utils::MD5 MD5 hashing function@endlink is based on
http://www.zedwood.com/article/cpp-md5-function,
which itself was converted to C++ class by Frank Thilo (thilo@unix-ag.org) for bzflag
(http://www.bzflag.org), and is based on the reference implementation of `RFC 1321`:

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.
>
>    License to copy and use this software is granted provided that it
>    is identified as the "RSA Data Security, Inc. MD5 Message-Digest
>    Algorithm" in all material mentioning or referencing this software
>    or this function.
>
>    License is also granted to make and use derivative works provided
>    that such works are identified as "derived from the RSA Data
>    Security, Inc. MD5 Message-Digest Algorithm" in all material
>    mentioning or referencing the derived work.
>
>    RSA Data Security, Inc. makes no representations concerning either
>    the merchantability of this software or the suitability of this
>    software for any particular purpose. It is provided "as is"
>    without express or implied warranty of any kind.
>
>    These notices must be retained in any copies of any part of this
>    documentation and/or software.
@htmlonly </details> @endhtmlonly

## SHA1 @anchor supplement_acknowledgements_code_reuse_sha

Our implementation of the @link genesis::utils::SHA1 SHA1 hashing function@endlink is based on
[https://github.com/vog/sha1](https://github.com/vog/sha1), which is 100% Public Domain.

## SHA256 @anchor supplement_acknowledgements_code_reuse_sha256

Our implementation of the @link genesis::utils::SHA256 SHA256 hashing function@endlink
is based on http://www.zedwood.com/article/cpp-sha256-function,
which itself is based on Olivier Gay's version, and was published with a Modified BSD License:

@htmlonly <details><summary>License</summary> @endhtmlonly
>    Updated to C++, zedwood.com 2012
>    Based on Olivier Gay's version
>    See Modified BSD License below:
>
>    FIPS 180-2 SHA-224/256/384/512 implementation
>    Issue date:  04/30/2005
>    http://www.ouah.org/ogay/sha2/
>
>    Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
>    All rights reserved.
>
>    Redistribution and use in source and binary forms, with or without
>    modification, are permitted provided that the following conditions
>    are met:
>    1. Redistributions of source code must retain the above copyright
>       notice, this list of conditions and the following disclaimer.
>    2. Redistributions in binary form must reproduce the above copyright
>       notice, this list of conditions and the following disclaimer in the
>       documentation and/or other materials provided with the distribution.
>    3. Neither the name of the project nor the names of its contributors
>       may be used to endorse or promote products derived from this software
>       without specific prior written permission.
>
>    THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS "AS IS" AND
>    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
>    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
>    ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
>    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
>    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
>    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
>    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
>    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
>    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
>    SUCH DAMAGE.
@htmlonly </details> @endhtmlonly

## Matplotlib Color Maps @anchor supplement_acknowledgements_code_reuse_matplotlib_color_maps

The color codes for the color maps @link genesis::utils::color_list_magma magma@endlink,
@link genesis::utils::color_list_inferno inferno@endlink,
@link genesis::utils::color_list_plasma plasma@endlink and
@link genesis::utils::color_list_viridis viridis@endlink are taken from
https://github.com/BIDS/colormap. They were created by Nathaniel J. Smith, Stefan van der Walt,
and (in the case of viridis) Eric Firing. These colormaps are released under the CC0 license /
public domain dedication. See https://creativecommons.org/publicdomain/zero/1.0/ for the license.

## xkcd  Color Names @anchor supplement_acknowledgements_code_reuse_xkcd_color_names

The color names and codes for the list used in
@link genesis::utils::color_from_name_xkcd() color_from_name_xkcd()@endlink and
@link genesis::utils::is_xkcd_color_name() is_xkcd_color_name()@endlink are taken from
[https://xkcd.com/color/rgb.txt](https://xkcd.com/color/rgb.txt).
They were published under Public Domain, see https://xkcd.com/color/rgb/ and
https://blog.xkcd.com/2010/05/03/color-survey-results/ for details.

## ColorBrewer Color Lists @anchor supplement_acknowledgements_code_reuse_colorbrewer

The color lists
@link genesis::utils::color_list_brbg brbg@endlink,
@link genesis::utils::color_list_piyg piyg@endlink,
@link genesis::utils::color_list_prgn prgn@endlink,
@link genesis::utils::color_list_puor puor@endlink,
@link genesis::utils::color_list_rdbu rdbu@endlink,
@link genesis::utils::color_list_rdgy rdgy@endlink,
@link genesis::utils::color_list_rdylbu rdylbu@endlink,
@link genesis::utils::color_list_rdylgn rdylgn@endlink,
@link genesis::utils::color_list_blues blues@endlink,
@link genesis::utils::color_list_bugn bugn@endlink,
@link genesis::utils::color_list_bupu bupu@endlink,
@link genesis::utils::color_list_gnbu gnbu@endlink,
@link genesis::utils::color_list_greens greens@endlink,
@link genesis::utils::color_list_greys greys@endlink,
@link genesis::utils::color_list_oranges oranges@endlink,
@link genesis::utils::color_list_orrd orrd@endlink,
@link genesis::utils::color_list_pubu pubu@endlink,
@link genesis::utils::color_list_pubugn pubugn@endlink,
@link genesis::utils::color_list_purd purd@endlink,
@link genesis::utils::color_list_purples purples@endlink,
@link genesis::utils::color_list_rdpu rdpu@endlink,
@link genesis::utils::color_list_reds reds@endlink,
@link genesis::utils::color_list_ylgn ylgn@endlink,
@link genesis::utils::color_list_ylgnbu ylgnbu@endlink,
@link genesis::utils::color_list_ylorbr ylorbr@endlink,
@link genesis::utils::color_list_ylorrd ylorrd@endlink,
@link genesis::utils::color_list_accent accent@endlink,
@link genesis::utils::color_list_dark2 dark2@endlink,
@link genesis::utils::color_list_paired paired@endlink,
@link genesis::utils::color_list_pastel1 pastel1@endlink,
@link genesis::utils::color_list_pastel2 pastel2@endlink,
@link genesis::utils::color_list_spectral spectral@endlink,
@link genesis::utils::color_list_set1 set1@endlink,
@link genesis::utils::color_list_set2 set2@endlink and
@link genesis::utils::color_list_set3 set3@endlink
were adapted from https://github.com/axismaps/colorbrewer and
https://github.com/Gnuplotting/gnuplot-palettes
by converting the color codes to RGB `double` values.

The original ColorBrewer color specifications and designs were developed by Cynthia Brewer
(http://colorbrewer.org/), while their gnuplot equivalets are authored by Anna Schneider.
Their work is published under the Apache-2.0 license:

@htmlonly <details><summary>License</summary> @endhtmlonly
>                                 Apache License
>                           Version 2.0, January 2004
>                        http://www.apache.org/licenses/
>
>   TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
>
>   1. Definitions.
>
>      "License" shall mean the terms and conditions for use, reproduction,
>      and distribution as defined by Sections 1 through 9 of this document.
>
>      "Licensor" shall mean the copyright owner or entity authorized by
>      the copyright owner that is granting the License.
>
>      "Legal Entity" shall mean the union of the acting entity and all
>      other entities that control, are controlled by, or are under common
>      control with that entity. For the purposes of this definition,
>      "control" means (i) the power, direct or indirect, to cause the
>      direction or management of such entity, whether by contract or
>      otherwise, or (ii) ownership of fifty percent (50%) or more of the
>      outstanding shares, or (iii) beneficial ownership of such entity.
>
>      "You" (or "Your") shall mean an individual or Legal Entity
>      exercising permissions granted by this License.
>
>      "Source" form shall mean the preferred form for making modifications,
>      including but not limited to software source code, documentation
>      source, and configuration files.
>
>      "Object" form shall mean any form resulting from mechanical
>      transformation or translation of a Source form, including but
>      not limited to compiled object code, generated documentation,
>      and conversions to other media types.
>
>      "Work" shall mean the work of authorship, whether in Source or
>      Object form, made available under the License, as indicated by a
>      copyright notice that is included in or attached to the work
>      (an example is provided in the Appendix below).
>
>      "Derivative Works" shall mean any work, whether in Source or Object
>      form, that is based on (or derived from) the Work and for which the
>      editorial revisions, annotations, elaborations, or other modifications
>      represent, as a whole, an original work of authorship. For the purposes
>      of this License, Derivative Works shall not include works that remain
>      separable from, or merely link (or bind by name) to the interfaces of,
>      the Work and Derivative Works thereof.
>
>      "Contribution" shall mean any work of authorship, including
>      the original version of the Work and any modifications or additions
>      to that Work or Derivative Works thereof, that is intentionally
>      submitted to Licensor for inclusion in the Work by the copyright owner
>      or by an individual or Legal Entity authorized to submit on behalf of
>      the copyright owner. For the purposes of this definition, "submitted"
>      means any form of electronic, verbal, or written communication sent
>      to the Licensor or its representatives, including but not limited to
>      communication on electronic mailing lists, source code control systems,
>      and issue tracking systems that are managed by, or on behalf of, the
>      Licensor for the purpose of discussing and improving the Work, but
>      excluding communication that is conspicuously marked or otherwise
>      designated in writing by the copyright owner as "Not a Contribution."
>
>      "Contributor" shall mean Licensor and any individual or Legal Entity
>      on behalf of whom a Contribution has been received by Licensor and
>      subsequently incorporated within the Work.
>
>   2. Grant of Copyright License. Subject to the terms and conditions of
>      this License, each Contributor hereby grants to You a perpetual,
>      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
>      copyright license to reproduce, prepare Derivative Works of,
>      publicly display, publicly perform, sublicense, and distribute the
>      Work and such Derivative Works in Source or Object form.
>
>   3. Grant of Patent License. Subject to the terms and conditions of
>      this License, each Contributor hereby grants to You a perpetual,
>      worldwide, non-exclusive, no-charge, royalty-free, irrevocable
>      (except as stated in this section) patent license to make, have made,
>      use, offer to sell, sell, import, and otherwise transfer the Work,
>      where such license applies only to those patent claims licensable
>      by such Contributor that are necessarily infringed by their
>      Contribution(s) alone or by combination of their Contribution(s)
>      with the Work to which such Contribution(s) was submitted. If You
>      institute patent litigation against any entity (including a
>      cross-claim or counterclaim in a lawsuit) alleging that the Work
>      or a Contribution incorporated within the Work constitutes direct
>      or contributory patent infringement, then any patent licenses
>      granted to You under this License for that Work shall terminate
>      as of the date such litigation is filed.
>
>   4. Redistribution. You may reproduce and distribute copies of the
>      Work or Derivative Works thereof in any medium, with or without
>      modifications, and in Source or Object form, provided that You
>      meet the following conditions:
>
>      (a) You must give any other recipients of the Work or
>          Derivative Works a copy of this License; and
>
>      (b) You must cause any modified files to carry prominent notices
>          stating that You changed the files; and
>
>      (c) You must retain, in the Source form of any Derivative Works
>          that You distribute, all copyright, patent, trademark, and
>          attribution notices from the Source form of the Work,
>          excluding those notices that do not pertain to any part of
>          the Derivative Works; and
>
>      (d) If the Work includes a "NOTICE" text file as part of its
>          distribution, then any Derivative Works that You distribute must
>          include a readable copy of the attribution notices contained
>          within such NOTICE file, excluding those notices that do not
>          pertain to any part of the Derivative Works, in at least one
>          of the following places: within a NOTICE text file distributed
>          as part of the Derivative Works; within the Source form or
>          documentation, if provided along with the Derivative Works; or,
>          within a display generated by the Derivative Works, if and
>          wherever such third-party notices normally appear. The contents
>          of the NOTICE file are for informational purposes only and
>          do not modify the License. You may add Your own attribution
>          notices within Derivative Works that You distribute, alongside
>          or as an addendum to the NOTICE text from the Work, provided
>          that such additional attribution notices cannot be construed
>          as modifying the License.
>
>      You may add Your own copyright statement to Your modifications and
>      may provide additional or different license terms and conditions
>      for use, reproduction, or distribution of Your modifications, or
>      for any such Derivative Works as a whole, provided Your use,
>      reproduction, and distribution of the Work otherwise complies with
>      the conditions stated in this License.
>
>   5. Submission of Contributions. Unless You explicitly state otherwise,
>      any Contribution intentionally submitted for inclusion in the Work
>      by You to the Licensor shall be under the terms and conditions of
>      this License, without any additional terms or conditions.
>      Notwithstanding the above, nothing herein shall supersede or modify
>      the terms of any separate license agreement you may have executed
>      with Licensor regarding such Contributions.
>
>   6. Trademarks. This License does not grant permission to use the trade
>      names, trademarks, service marks, or product names of the Licensor,
>      except as required for reasonable and customary use in describing the
>      origin of the Work and reproducing the content of the NOTICE file.
>
>   7. Disclaimer of Warranty. Unless required by applicable law or
>      agreed to in writing, Licensor provides the Work (and each
>      Contributor provides its Contributions) on an "AS IS" BASIS,
>      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
>      implied, including, without limitation, any warranties or conditions
>      of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
>      PARTICULAR PURPOSE. You are solely responsible for determining the
>      appropriateness of using or redistributing the Work and assume any
>      risks associated with Your exercise of permissions under this License.
>
>   8. Limitation of Liability. In no event and under no legal theory,
>      whether in tort (including negligence), contract, or otherwise,
>      unless required by applicable law (such as deliberate and grossly
>      negligent acts) or agreed to in writing, shall any Contributor be
>      liable to You for damages, including any direct, indirect, special,
>      incidental, or consequential damages of any character arising as a
>      result of this License or out of the use or inability to use the
>      Work (including but not limited to damages for loss of goodwill,
>      work stoppage, computer failure or malfunction, or any and all
>      other commercial damages or losses), even if such Contributor
>      has been advised of the possibility of such damages.
>
>   9. Accepting Warranty or Additional Liability. While redistributing
>      the Work or Derivative Works thereof, You may choose to offer,
>      and charge a fee for, acceptance of support, warranty, indemnity,
>      or other liability obligations and/or rights consistent with this
>      License. However, in accepting such obligations, You may act only
>      on Your own behalf and on Your sole responsibility, not on behalf
>      of any other Contributor, and only if You agree to indemnify,
>      defend, and hold each Contributor harmless for any liability
>      incurred by, or claims asserted against, such Contributor by reason
>      of your accepting any such warranty or additional liability.
>
>   END OF TERMS AND CONDITIONS
>
>   APPENDIX: How to apply the Apache License to your work.
>
>      To apply the Apache License to your work, attach the following
>      boilerplate notice, with the fields enclosed by brackets "{}"
>      replaced with your own identifying information. (Don't include
>      the brackets!)  The text should be enclosed in the appropriate
>      comment syntax for the file format. We also recommend that a
>      file or class name and description of purpose be included on the
>      same "printed page" as the copyright notice for easier
>      identification within third-party archives.
>
>   Copyright {yyyy} {name of copyright owner}
>
>   Licensed under the Apache License, Version 2.0 (the "License");
>   you may not use this file except in compliance with the License.
>   You may obtain a copy of the License at
>
>       http://www.apache.org/licenses/LICENSE-2.0
>
>   Unless required by applicable law or agreed to in writing, software
>   distributed under the License is distributed on an "AS IS" BASIS,
>   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
>   See the License for the specific language governing permissions and
>   limitations under the License.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/axismaps/colorbrewer/blob/master/LICENCE.txt
on 2018-01-08.

## Nextstrain @anchor supplement_acknowledgements_code_reuse_nextstrain

The color list @link genesis::utils::color_list_nextstrain() color_list_nextstrain()@endlink
is adapted from the Nextstrain build for novel coronavirus (nCoV) at
https://nextstrain.org/ncov, using their color scheme
https://github.com/nextstrain/ncov/blob/master/config/color_schemes.tsv
and converting it to RGB `double` values.
The repository at https://github.com/nextstrain/ncov is published under the
[MIT license](https://opensource.org/licenses/MIT):

@htmlonly <details><summary>License</summary> @endhtmlonly
>    MIT License
>
>    Copyright (c) 2020 Nextstrain
>
>    Permission is hereby granted, free of charge, to any person obtaining a copy
>    of this software and associated documentation files (the "Software"), to deal
>    in the Software without restriction, including without limitation the rights
>    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>    copies of the Software, and to permit persons to whom the Software is
>    furnished to do so, subject to the following conditions:
>
>    The above copyright notice and this permission notice shall be included in all
>    copies or substantial portions of the Software.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
>    SOFTWARE.
@htmlonly </details> @endhtmlonly

The license information above was copied from
https://github.com/nextstrain/ncov/blob/master/LICENSE
on 2020-04-05.
