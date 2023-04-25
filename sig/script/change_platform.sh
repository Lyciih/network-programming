#! /bin/sh

make clean
cd lib
rm -f libdouble_link_list.a

cd double_link_list
make clean
make
cp libdouble_linked_list.a ../

cd ../..
make
