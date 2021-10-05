/// generate_types
///
/// Command line interface to generate a yaml type specification file.
/// Other tools can be used to generate interface files from the yaml file.
///
/// This must run in an environment where everything ROOT and the
/// atlas software is availible.
///
#include "translate.hpp"
#include "type_helpers.hpp"
#include "utils.hpp"

#include "TSystem.h"
#include "TROOT.h"

#include <iostream>
#include <queue>
#include <set>

using namespace std;

int main(int, char**) {
    auto app_reference = create_root_app();

    // Load a library
    auto status = gSystem->Load("libxAODJet");
    if (status < 0) {
        cout << "ERROR: Can't load library: " << status << endl;
    }

    queue<string> classes_to_do;
    set<string> classes_done;
    classes_to_do.push("xAOD::JetContainer");
    vector<class_info> done_classes;

    while (classes_to_do.size() > 0) {
        auto class_name(classes_to_do.front());
        classes_to_do.pop();
        if (classes_done.find(class_name) != classes_done.end())
            continue;
        classes_done.insert(class_name);

        auto c = translate_class(class_name);
        if (c.name.size() > 0) {
            done_classes.push_back(c);

            for (auto &&c_name : referenced_types(c))
            {
                classes_to_do.push(c_name);
            }        
        }
    }

    // Look at the loaded type defs, and add alaises.
    fixup_type_aliases(done_classes);

    // Dump them all out
    for (auto &&c : done_classes)
    {
        cout << c << endl;
    }
    

}
