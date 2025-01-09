containers=$(find -L $ROOTCOREDIR/include -name \*Container.h -exec basename {} \; | sed 's/..$//' | sed 's/^/-c xAOD::/')
libraries=$(find -L $ROOTCOREDIR/lib -name \*.so -exec basename {} \; | grep -vi Dict | grep xAOD | sed 's/...$//' | sed 's/^/-l /')
required_classes=$(sed 's/^/-c "/; s/$/"/' scripts/required_classes.txt | tr '\n' ' ')

./build/generate_types $containers $libraries
