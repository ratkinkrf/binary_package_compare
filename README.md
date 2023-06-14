# binary_package_compare

 Compares info from two branches of binary packages.
 Packages request with REST api and then added into my json structure.
 Result is 3 .json files:
 1 - packages which are in 1st branch but not in 2nd
 2 - packages which are in 2nd branch but not in 1st
 3 - packages from 1st branch where version is higher then in 2nd branch
 
 cd to directory where CMakeLists and /src /include located 
 to seprate build files from source files make new directory and run cmake from there
 example
 mkdir build
 cd build
 cmake ..
 make
./BinaryCompare p9 p10 x86_64

arguments are:
  1 - 1st branch
  2 - 2nd branch
  3- arch
  
 Available arch :
 x86_64
 i586
 noarch
 aarch64
 armh
 ppc64le
