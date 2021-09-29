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
#include <queue>
#include <set>

using namespace std;

int main(int, char**) {

    // Load a library
    auto status = gSystem->Load("libxAODJet");
    if (status < 0) {
        cout << "ERROR: Can't load library: " << status << endl;
    }

    queue<string> classes_to_do;
    set<string> classes_done;
    classes_to_do.push("xAOD::JetContainer");

    while (classes_to_do.size() > 0) {
        auto class_name(classes_to_do.front());
        classes_to_do.pop();
        if (classes_done.find(class_name) != classes_done.end())
            continue;
        classes_done.insert(class_name);

        auto c = translate_class(class_name);
        if (c.name.size() > 0) {
            cout << c;
        }

        for (auto &&c_name : referenced_types(c))
        {
            classes_to_do.push(c_name);
        }        
    }
}
