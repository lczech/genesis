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

## MD5 @anchor supplement_acknowledgements_code_reuse_md

Our implementation of the @link genesis::utils::MD5 MD5 hashing function@endlink is based on
http://www.zedwood.com/article/cpp-md5-function,
which itself was converted to C++ class by Frank Thilo (thilo@unix-ag.org) for bzflag
(http://www.bzflag.org), and is based on the reference implementation of `RFC 1321`:

<blockquote>
Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
</blockquote>

## SHA1 @anchor supplement_acknowledgements_code_reuse_sha

Our implementation of the @link genesis::utils::SHA1 SHA1 hashing function@endlink is based on
[https://github.com/vog/sha1](https://github.com/vog/sha1), which is 100% Public Domain.

## SHA256 @anchor supplement_acknowledgements_code_reuse_sha256

Our implementation of the @link genesis::utils::SHA256 SHA256 hashing function@endlink
is based on http://www.zedwood.com/article/cpp-sha256-function,
which itself is based on Olivier Gay's version, and was published with a Modified BSD License:

<blockquote>
Updated to C++, zedwood.com 2012
Based on Olivier Gay's version
See Modified BSD License below:

FIPS 180-2 SHA-224/256/384/512 implementation
Issue date:  04/30/2005
http://www.ouah.org/ogay/sha2/

Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the project nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
</blockquote>

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

The license information above was copied from
https://github.com/axismaps/colorbrewer/blob/master/LICENCE.txt
on 2018-01-08.
