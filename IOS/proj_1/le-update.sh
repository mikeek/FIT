#!/bin/sh
([ -d "./.le" ] && grep "^ignore" ./.le/.config | sed "s,ignore ,," > /tmp/le-ignor) ||
{ echo "nelze nalezt adresar referencni kopie" >&2; exit 1; }
projdir=`grep "^projdir" ./.le/.config | sed "s,projdir ,,"`
excode=0; files="";
if [ $# = 0 ]; then
  files=`find ./ -maxdepth 1 -type f -iname "[^.]*" | sed "s,.*/,,"`"\n"`find "$projdir/" -maxdepth 1 -type f -iname "[^.]*" | sed "s,.*/,,"`
else
  for i in "$@"
  do
    files="$files\n$i"
  done
fi
excode=0
printf "$files" | grep -E -f /tmp/le-ignor -v | grep -v "^$" | grep -v -E "^\..+" | sort -u |
while read -r line; do
  # 6)
  if [ -f "$projdir/$line" ] && [ ! -f "./$line" ]; then
    cp "$projdir/$line" "./" 2>/dev/null
    cp "$projdir/$line" "./.le/" 2>/dev/null
    echo "C: $line"
    continue
  fi
  # 7)
  if [ -f "./.le/$line" ] && [ ! -f "$projdir/$line" ]; then
    rm "./$line" "./.le/$line" -f
    echo "D: $line"
    continue
  fi
  if [ ! -f "./.le/$line" ]; then 
    (! echo "$line" | grep -E "^-e" >/dev/null 2>&1) && 
      echo "$line: soubor nelze nalezt v adresari referencni kopie!" >&2
    excode=1
    continue
  fi
  # 1)
  if cmp -s "$projdir/$line" "./.le/$line" 2>/dev/null && cmp -s "./$line" "./.le/$line" 2>/dev/null; then
    echo ".: $line"
  fi
  # 2)
  if cmp -s "$projdir/$line" "./.le/$line" 2>/dev/null && ! cmp -s "./$line" "./.le/$line" 2>/dev/null; then
    echo "M: $line"
  fi
  # 3)
  if cmp -s "$projdir/$line" "./$line" 2>/dev/null && ! cmp -s "./$line" "./.le/$line" 2>/dev/null; then
    echo "UM: $line"
    cp "$projdir/$line" "./.le/$projdir" 2>/dev/null
  fi
  # 4)
  if cmp -s "./.le/$line" "./$line" 2>/dev/null && ! cmp -s "$projdir/$line" "./.le/$line" 2>/dev/null; then
    echo "U: $line"
    cp "$projdir/$line" "./.le/" 2>/dev/null
    cp "$projdir/$line" "./" 2>/dev/null
  fi
  # 5)
  if ! cmp -s "$projdir/$line" "./.le/$line" 2>/dev/null && ! cmp -s "./$line" "./.le/$line" 2>/dev/null &&     
     ! cmp -s "$projdir/$line" "./$line" 2>/dev/null; then
    diff -u "$projdir/$line" "./.le/$line" | patch "./$line" -f -R >/dev/null 2>&1
    if [ $? = 0 ]; then
      echo "M+: $line"
      cp "$projdir/$line" "./.le/$line" 2>/dev/null
    else
      echo "M!: $line conflict!"
      cp "./$line.orig" "./$line" 2>/dev/null
    fi
    rm -f "./$line.orig" "./$line.rej" 2>/dev/null
  fi
done
rm -f /tmp/le-ignor 2>/dev/null
exit $excode
