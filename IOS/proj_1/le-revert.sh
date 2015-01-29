#!/bin/sh
([ -d "./.le" ] && grep "^ignore" ./.le/.config | sed "s,ignore ,," > /tmp/le-ignor) ||
{ echo "nelze nalezt adresar referencni kopie" >&2; exit 1; }
if [ $# = 0 ]; then
  files=`find ./.le/ -maxdepth 1 -type f -iname "[^.]*" | sed "s,.*/,,"` 2>/dev/null
else
  for i in "$@"
  do
    files="$files\n$i"
  done
fi
errcode=0
printf "$files" | grep -E -f /tmp/le-ignor -v | grep -v "^$" | grep -v -E "^\..+" |
while read line
do
  ([ ! -f "./.le/$line" ] && (! echo "$line" | grep -E "^-e" >/dev/null 2>&1) && 
  echo "$line: soubor nenalezen" >&2 && errcode=1) ||  cp -f "./.le/$line" "./" 2>/dev/null
done
rm -f /tmp/le-ignor 2>/dev/null
exit $errcode