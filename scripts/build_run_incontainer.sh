#!/bin/bash

# Setup the release
source ~/release_setup.sh

# We are going to need to copy over the appropriate files
# This is because sometimes the directories are used to
# do development
mkdir ~/builder
cd ~/builder
cp /func_adl_xaod_types/CMakeLists.txt ~/builder
cp -r /func_adl_xaod_types/bin ~/builder
cp -r /func_adl_xaod_types/include ~/builder
cp -r /func_adl_xaod_types/scripts ~/builder
cp -r /func_adl_xaod_types/metadata ~/builder
cp -r /func_adl_xaod_types/src ~/builder
cp -r /func_adl_xaod_types/tests ~/builder

# Configure and build the code
mkdir build
cd build
cmake ..
cmake --build .

# And run everything
cd ..
scripts/run_on_atlas_containers.sh > /output/$1
# End of junk.sh