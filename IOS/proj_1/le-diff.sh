#!/bin/sh
([ -d "./.le" ] && grep "^ignore" ./.le/.config | sed "s,ignore ,," > /tmp/le-ignor) ||
{ echo "nelze nalezt adresar referencni kopie" >&2; exit 1; }
projdir=`grep "^projdir" ./.le/.config | sed "s,projdir ,,"`
if [ $# = 0 ]; then
  files=`find ./ -maxdepth 1 -type f -iname "[^.]*" | sed "s,.*/,,"`"\n"`find "$projdir/" -maxdepth 1 -type f -iname "[^.]*" | sed "s,.*/,,"`
  files=`printf "$files" | sort -u`
else
  for i in "$@"
  do
    files="$files\n$i"
  done
fi
errcode=0
printf "$files" | grep -E -f /tmp/le-ignor -v | grep -v "^$" | grep -v -E "^\..+" |
while read -r line; do
  [ -f "./$line" ]   && [ ! -f "$projdir/$line" ] && echo D: $line
  [ ! -f "./$line" ] && [ -f "$projdir/$line" ]   && echo C: $line
  [ -f "./$line" ]   && [ -f "$projdir/$line" ]   && diff -u "$projdir/$line" "./$line" && echo .: $line; 
  [ ! -f "./$line" ] && [ ! -f "$projdir/$line" ] && (! echo "$line" | grep -E "^-e" >/dev/null 2>&1) && 
  echo "$line: soubor nenalezen" >&2 && errcode=1
done
rm -f /tmp/le-ignor 2>/dev/null
exit $errcode
