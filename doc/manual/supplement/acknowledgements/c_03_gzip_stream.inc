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
