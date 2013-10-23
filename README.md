# qseq2fq -- Simple C++ qseq to FASTQ converter

This program converts a qseq file to a FASTQ file. It will filter out
failing reads unless `-d`/`--dont-filter` are specified.

    qseq2fq in_qseq.txt > out.fq

That's pretty much it.
