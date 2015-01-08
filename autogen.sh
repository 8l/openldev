#!/bin/sh

echo "Running 'libtoolize --copy --automake'"
libtoolize --copy --automake
echo "Running 'aclocal'"
aclocal
echo "Running 'automake --add-missing --copy'"
automake --add-missing --copy
echo "Running 'autoconf'"
autoconf
echo "Running './configure'"
./configure
