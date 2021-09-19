#!/bin/bash

# We use this helper script to concatenate the files in the `acknowledgements` sub directory.
# This keeps these files separated, which gives better overview and maintainability.
# Not a big thing, but helps a bit.

rm -f acknowledgements.md
for f in `ls acknowledgements/*.inc` ; do
    echo "Adding $f"

    # Concat all files, followed by an empty line
    cat $f >> acknowledgements.md
    echo "" >> acknowledgements.md
done

# Remove excess blank line at end of the file
stripped=$(<acknowledgements.md)
printf '%s\n' "$stripped" > acknowledgements.md
