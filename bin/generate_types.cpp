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
#include "xaod_helpers.hpp"

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
    status = gSystem->Load("libxAODMissingET");
    if (status < 0) {
        cout << "ERROR: Can't load library: " << status << endl;
    }    

    queue<string> classes_to_do;
    set<string> classes_done;
    classes_to_do.push("xAOD::JetContainer");
    classes_to_do.push("xAOD::MissingETContainer");
    vector<class_info> done_classes;

    // auto some_stuff = all_that_inherrit_from("SG::AuxVectorBase");
    // cout << "Found " << some_stuff.size() << " classes." << endl;
    // for (auto &&i : some_stuff)
    // {
    //     cout << "  " << i << endl;
    // }
    // auto my_typedefs = root_typedef_map();
    // for (auto &&m_td : my_typedefs)
    // {
    //     cout << "mapping " << m_td.first << endl;
    //     for (auto &&m_r : m_td.second)
    //     {
    //         cout << "  -> " << m_r << endl;
    //     }        
    // }

    set<string> bad_classes;
    bad_classes.insert("ROOT");
    bad_classes.insert("xAOD");    
    bad_classes.insert("TObject");

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
                if (bad_classes.find(c_name) == bad_classes.end())
                    classes_to_do.push(c_name);
            }        
        }
    }

    // Look at the loaded type defs, and add alaises.
    fixup_type_aliases(done_classes);

    // Get the list of containers from the classes. These will be top level collections
    // stored in the data.
    auto collections = find_collections(done_classes);


    // Dump them all out
    for (auto &&c : collections)
    {
        cout << c << endl;
    }
    
    for (auto &&c : done_classes)
    {
        cout << c << endl;
    }
    
    // auto my_typedefs = root_typedef_map();
    // for (auto &&m_td : my_typedefs)
    // {
    //     cout << "mapping " << m_td.first << endl;
    //     for (auto &&m_r : m_td.second)
    //     {
    //         cout << "  -> " << m_r << endl;
    //     }
        
    // }
}
