#!/bin/bash -eu

from="$1"
to="$2"

bash -c "cd $from; ls -lTR ." > /tmp/a
bash -c "cd $from; find . -type f -exec md5 {} \; |sort" >> /tmp/a

bash -c "cd $to; ls -lTR ." > /tmp/b
bash -c "cd $to; find . -type f -exec md5 {} \; |sort" >> /tmp/b


diff -u /tmp/a /tmp/b
