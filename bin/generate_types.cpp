/// generate_types
///
/// Command line interface to generate a yaml type specification file.
/// Other tools can be used to generate interface files from the yaml file.
///
/// This must run in an environment where everything ROOT and the
/// atlas software is availible.
///
#include "translate.hpp"

#include "TSystem.h"

#include <iostream>
using namespace std;

int main(int, char**) {

    // Load a library
    auto status = gSystem->Load("libxAODJet");
    if (status < 0) {
        cout << "ERROR: Can't load library: " << status << endl;
    }
    translate_class("xAOD::Jet_v1");
}
