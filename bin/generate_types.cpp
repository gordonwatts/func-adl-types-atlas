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

#include "yaml-cpp/yaml.h"

#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
#include <iterator>

using namespace std;

// Return true if it is ok to emit this particular class.
bool can_emit_class(const class_info &c_info) {
    if (c_info.name_as_type.type_name == "vector") {
        return false;
    }
    if (c_info.name_as_type.type_name == "string") {
        return false;
    }
    if (c_info.name_as_type.template_arguments.size() > 0) {
        return false;
    }
    if (c_info.methods.size() == 0) {
        return false;
    }
    return true;
}

// Return true if this method can actually be emitted.
bool can_emit_method(const method_info &meth, const set<string> &classes_to_emit) {
    // Make sure returns something.
    if (meth.return_type.size() == 0) {
        return false;
    }

    // Get all referenced methods, and make sure we know about those objects.
    // Or we can't support this guy.
    auto method_types = referenced_types(meth);
    set<string> method_types_set(method_types.begin(), method_types.end());
    if (!includes(classes_to_emit.begin(), classes_to_emit.end(), method_types_set.begin(), method_types_set.end())) {
        return false;
    }
    return true;
}

// Return true if any method can be emitted
bool can_emit_any_methods(const vector<method_info> &methods, const set<string> &classes_to_emit) {
    for (auto &&m : methods)
    {
        if (can_emit_method(m, classes_to_emit)) {
            return true;
        }
    }
    return false;
}

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
    map<string, class_info> class_map;

    set<string> bad_classes;
    bad_classes.insert("ROOT");
    bad_classes.insert("xAOD");    
    bad_classes.insert("TObject");

    while (classes_to_do.size() > 0) {
        auto raw_class_name(classes_to_do.front());
        classes_to_do.pop();
        if (classes_done.find(raw_class_name) != classes_done.end())
            continue;
        classes_done.insert(raw_class_name);

        auto class_name = unqualified_type_name(raw_class_name);
        if ((class_name != raw_class_name)
            && (classes_done.find(class_name) != classes_done.end())) {
                continue;
            }
        classes_done.insert(class_name);

        auto c = translate_class(class_name);
        if (c.name.size() > 0) {
            done_classes.push_back(c);
            class_map[c.name] = c;

            for (auto &&c_name : referenced_types(c))
            {
                if (bad_classes.find(c_name) == bad_classes.end()
                    && (c_name.find("Eigen") == c_name.npos)
                    )
                    classes_to_do.push(c_name);
            }        
        }
    }

    // Look at the loaded type defs, and add aliases.
    fixup_type_aliases(done_classes);

    // Get the list of containers from the classes. These will be top level collections
    // stored in the data.
    auto collections = find_collections(done_classes);

    // Dump them all out
    YAML::Emitter out;
    out << YAML::BeginMap
        << YAML::Key << "collections"
        << YAML::Value
        << YAML::BeginSeq;
    for (auto &&c : collections)
    {
        out << YAML::BeginMap
            << YAML::Key << "name" << YAML::Value << c.name
            << YAML::Key << "item_type" << YAML::Value << c.iterator_type_info.template_arguments[0].nickname
            << YAML::Key << "container_type" << YAML::Value << c.type_info.nickname
            << YAML::Key << "include_file" << YAML::Value << c.include_file
            << YAML::EndMap;
    }
    out << YAML::EndSeq;

    // Look through all the classes we know about, tag just the classes
    // we can appropriately write out.
    classes_done.clear();
    set<string> classes_to_emit;
    for (auto &&c : collections)
    {
        classes_to_do.push(c.iterator_type_info.template_arguments[0].nickname);
    }
    while (!classes_to_do.empty()) {
        string c_name(unqualified_type_name(classes_to_do.front()));
        classes_to_do.pop();
        if (classes_done.find(c_name) != classes_done.end()) {
            continue;
        }
        classes_done.insert(c_name);

        // Find the class
        auto c_info = class_map.find(c_name);
        if (c_info == class_map.end()) {
            continue;
        }

        // If we can dump the class, then we should!
        if (can_emit_class(c_info->second)) {
            classes_to_emit.insert(c_name);
        }

        // Now, add referenced classes back to the queue
        auto reffed_classes = referenced_types(c_info->second);
        for (auto &&c_ref : reffed_classes)
        {
            classes_to_do.push(c_ref);
        }        
    }

    // Add some of the default types that need no introduction
    classes_to_emit.insert("double");
    classes_to_emit.insert("short");
    classes_to_emit.insert("int");
    classes_to_emit.insert("float");

    // Now, we need to loop through all of these things until we get a stable set of classes that we can emit.
    // Yes, this is painful.
    bool modified = true;
    while (modified) {
        modified = false;
        set<string> bad_classes;
        for (auto &&c_name : classes_to_emit)
        {
            auto class_info_ptr = class_map.find(c_name);
            if (class_info_ptr != class_map.end()) {
                auto &&class_info = class_info_ptr->second;
                if (!can_emit_any_methods(class_info.methods, classes_to_emit)) {
                    bad_classes.insert(c_name);
                }
            }
        }
        if (bad_classes.size() > 0) {
            modified = true;
            for (auto &&b_c : bad_classes)
            {
                classes_to_emit.erase(b_c);
            }
        }        
    }

    // Finally, we actually emit these.
    out << YAML::Key << "classes"
        << YAML::Value
        << YAML::BeginSeq;

    // Get a list of a list of all classes 
    for (auto &&c : classes_to_emit)
    {
        string c_name(unqualified_type_name(c));

        // Find the class
        auto c_info = class_map.find(c_name);
        if (c_info == class_map.end()) {
            continue;
        }

        // Make sure there is at least one method
        if (!can_emit_any_methods(c_info->second.methods, classes_to_emit)) {
            continue;
        }

        // If we can dump the class, then we should!
        out << YAML::BeginMap
            << YAML::Key << "name" << YAML::Value << normalized_type_name(c_info->second.name_as_type);
        
        if (c_info->second.include_file.size() > 0) {
            out << YAML::Key << "include_file" << YAML::Value << c_info->second.include_file;
        }

        // Now we need to emit the methods.
        bool first_method = true;
        for (auto &&meth : c_info->second.methods)
        {
            if (can_emit_method(meth, classes_to_emit)) {
                if (first_method) {
                    out << YAML::Key << "methods"
                        << YAML::Value
                        << YAML::BeginSeq;
                    first_method = false;
                }

                out << YAML::BeginMap
                    << YAML::Key << "name" << YAML::Value << meth.name
                    << YAML::Key << "return_type" << YAML::Value << normalized_type_name(meth.return_type);

                bool first_argument = true;
                for (auto &&arg : meth.arguments)
                {
                    if (first_argument) {
                        first_argument = false;
                        out << YAML::Key << "arguments"
                            << YAML::Value
                            << YAML::BeginSeq;
                    }
                    out << YAML::BeginMap
                        << YAML::Key << "name" << YAML::Value << arg.name
                        << YAML::Key << "type" << YAML::Value << normalized_type_name(arg.full_typename)
                        << YAML::EndMap;
                }
                if (!first_argument) {
                    out << YAML::EndSeq;
                }
                out << YAML::EndMap;
            }
        }

        if (!first_method) {
            out << YAML::EndSeq;
        }
        

        out << YAML::EndMap;
    }
    out << YAML::EndSeq << YAML::EndMap;

    // Dump to the output
    cout << out.c_str();
}
