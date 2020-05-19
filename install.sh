#!/bin/bash

# array of folders to copy
files=`echo include src`

# ask the HyperNEAT root location
read -p 'HyperNEAT root location: ' dir

# copy files to appropriate places
for f in ${files} ;do cp -r ${f} ${dir}/NE/HyperNEAT/Hypercube_NEAT/ ;done
cp -r out/ ${dir}/NE/HyperNEAT/
cp *.patch ${dir}/NE/HyperNEAT/

# apply patch to recognise newly copied files
cd ${dir}/NE/HyperNEAT/
patch -s -p0 < addRiver.patch