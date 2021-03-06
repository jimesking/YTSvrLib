#!/bin/bash
base_dir=`pwd`
echo $base_dir
cd $base_dir"/src"
aclocal && autoheader && autoreconf -f -i -Wall,no-obsolete && automake --add-missing && autoconf
chmod 755 configure
mkdir -p linux_debug
mkdir -p linux_release
cd $base_dir"/src/linux_debug"
../configure
cd $base_dir"/src/linux_release"
../configure