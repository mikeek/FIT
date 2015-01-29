#!/bin/sh
if [ $# != 1 ]; then
  echo 'spatny pocer argumentu' >&2
  exit 1;
fi
mkdir .le 2>/dev/null
touch /tmp/le-ignore

if [ -f "./.le/.config" ]; then
  (grep "^projdir" ./.le/.config >/dev/null && sed -i -e "s,projdir .*,projdir $1," ./.le/.config) ||
  echo "projdir $1" >> ./.le/.config
  grep "^ignore" ./.le/.config | sed "s,ignore ,," > /tmp/le-ignore 2>/dev/null
  find "./.le/" -maxdepth 1 -type f -iname '[^.]*' |  grep -E -f /tmp/le-ignore -v |
  while read line
  do
    rm -f "$line" 2>/dev/null
  done
else
  echo "projdir $1" > ./.le/.config
fi
find "$1/" -maxdepth 1 -type f -iname '[^.]*' | grep -E -f /tmp/le-ignore -v | 
while read line
do
  cp -f "$line" "./"
  cp -f "$line" "./.le"
done
rm -f /tmp/le-ignore 2>/dev/null
exit 0