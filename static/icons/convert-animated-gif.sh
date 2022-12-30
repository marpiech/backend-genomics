#!/bin/bash
WORKDIR="/tmp/gif"
mkdir -p $WORKDIR
GIF=$1
GIF_NAME=$(basename $GIF)
i=0
FROM=$GIF; i=$((i+1)); TO=$WORKDIR/$GIF_NAME.$i.gif
cp $FROM $TO
FROM=$WORKDIR/$GIF_NAME.$i.gif; i=$((i+1)); TO=$WORKDIR/$GIF_NAME.$i.gif
gifsicle $FROM --colors 6 > $TO
FROM=$WORKDIR/$GIF_NAME.$i.gif; i=$((i+1)); TO=$WORKDIR/$GIF_NAME.$i.gif
convert $FROM -negate $TO
FROM=$WORKDIR/$GIF_NAME.$i.gif; i=$((i+1)); TO=$WORKDIR/$GIF_NAME.$i.gif
convert $FROM -fill "#222222" -opaque "#000000" $TO
COLORS=$(convert $TO -unique-colors txt:- | \
   cut -d ' ' -f 4 | \
   sort | \
   uniq -c | \
   sort -n | \
   grep '#......FF' | \
   grep -o '#......' | \
   awk '{if (substr($1, 2, 1) != substr($1, 4, 1)) {print $0}}')
for COLOR in $COLORS;
do
    FROM="$WORKDIR/$GIF_NAME.$i.gif"
    i=$((i+1))
    TO="$WORKDIR/$GIF_NAME.$i.gif"
    convert "$FROM" -fill "#55bb55" -opaque "$COLOR" "$TO"
done
