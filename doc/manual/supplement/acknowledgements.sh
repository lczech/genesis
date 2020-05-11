#!/bin/bash

rm -f acknowledgements.md

for f in `ls acknowledgements/*.md` ; do
    echo "Adding $f"

    # Concat all files, followed by an empty line
    cat $f >> acknowledgements.md
    echo "" >> acknowledgements.md
done
