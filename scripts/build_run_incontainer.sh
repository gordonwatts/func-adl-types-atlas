#!/bin/bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
p_base="$DIR/.."

touch test.txt
ls -lR /workdir
touch /workdir/output/junk.txt

# Setup the release
source ~/release_setup.sh

# We are going to need to copy over the appropriate files
# This is because sometimes the directories are used to
# do development
mkdir ~/builder
cd ~/builder
cp $p_base/CMakeLists.txt ~/builder
cp -r $p_base/bin ~/builder
cp -r $p_base/include ~/builder
cp -r $p_base/scripts ~/builder
cp -r $p_base/metadata ~/builder
cp -r $p_base/src ~/builder
cp -r $p_base/tests ~/builder

# Configure and build the code
mkdir build
cd build
cmake ..
cmake --build .

# And run everything
cd ..
chmod a+x scripts/run_on_atlas_containers.sh
scripts/run_on_atlas_containers.sh > /workdir/output/$1
# End of junk.sh