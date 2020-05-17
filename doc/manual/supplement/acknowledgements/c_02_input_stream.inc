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
