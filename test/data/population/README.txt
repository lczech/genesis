The sam/bam/cram example files `ex1.*` are based on the example file `ex1.sam.gz` of samtools, published under the MIT license.
See https://github.com/samtools/samtools/tree/develop/examples for the original data,
with the version that we used at https://github.com/samtools/samtools/blob/2ece68ef9d0bd302a74c952b55df1badf9e61aae/examples/ex1.sam.gz
and see https://github.com/samtools/samtools/blob/2ece68ef9d0bd302a74c952b55df1badf9e61aae/LICENSE for the license.

First, we added header info to the file, as described in the htslib example readme at
https://github.com/samtools/samtools/blob/2ece68ef9d0bd302a74c952b55df1badf9e61aae/examples/00README.txt
The resulting file was amended by RG tags for testing purposes:

    samtools addreplacerg -r ID:S1 -r LB:L1 -r SM:SAMPLE1 -o output1.sam ex1.bam
    samtools addreplacerg -r ID:S2 -r LB:L2 -r SM:SAMPLE2 -o output2.sam output1.sam

which results in all reads belonging to S2.
Then randomly assign each read to either of the to read groups:

    while read -r line; do
        # Replace lines 50% of the time with the other RG tag.
        # This is also applied to the header, but is idempotent there.
        R=$((1 + $RANDOM % 2))
        if [[ "$R" -eq 1 ]]; then
            echo "$line" | sed "s/RG:Z:S2/RG:Z:S1/g" >> ex1.sam
        else
            echo "$line" >> ex1.sam
        fi
    done < output2.sam
    gzip ex1.sam.gz

We converted this file also to bam and cram using samtools for our testing,
using the fasta file from the same example of htslib,
at https://github.com/samtools/samtools/blob/2ece68ef9d0bd302a74c952b55df1badf9e61aae/examples/ex1.fa
With this, we can build an index for the fa file, and convert:

    samtools faidx ex1.fa
    samtools view -S -b -t ex1.fa.fai -o ex1.bam ex1.sam.gz
    samtools view -S -C -t ex1.fa.fai -o ex1.cram ex1.sam.gz

for our final three test files in all three formats.

Lastly, because CRAM uses absolute paths for the reference file, we also use a reference cache.
This is to avoid depending on the absoulte paths were we develop genesis, so that our cram tests can be run anywhere.
See https://github.com/samtools/htslib/issues/1401 for the issue.
To solve this, we ran

    ./samtools/misc/seq_cache_populate.pl -root .../genesis/test/data/population/cram_cache -subdirs 0 .../genesis/test/data/population/ex1.fa

to create the cram_cache files, and use these in the gtest setup.
