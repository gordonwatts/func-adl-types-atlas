/// generate_types
///
/// Command line interface to generate a yaml type specification file.
/// Other tools can be used to generate interface files from the yaml file.
///
/// This must run in an environment where everything ROOT and the
/// atlas software is available.
///
#include "translate.hpp"
#include "type_helpers.hpp"
#include "utils.hpp"
#include "xaod_helpers.hpp"
#include "collections_info.hpp"
#include "helper_files.hpp"

#include "TSystem.h"
#include "TROOT.h"
#include "TClassTable.h"

#include "yaml-cpp/yaml.h"

#include <boost/program_options.hpp>

#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
#include <iterator>
#include <fstream>

using namespace std;
using namespace boost::program_options;

// Return true if it is ok to emit this particular class.
bool can_emit_class(const class_info &c_info) {
    if (c_info.name_as_type.type_name == "string") {
        return false;
    }
    if ((c_info.name_as_type.type_name != "vector") && (c_info.name_as_type.type_name != "ElementLink")) {
        if (c_info.name_as_type.template_arguments.size() > 0) {
            return false;
        }
    }
    if (c_info.methods.size() == 0) {
        return false;
    }
    return true;
}

// Return true if any method can be emitted
bool can_emit_any_methods(const vector<method_info> &methods, const set<string> &classes_to_emit) {
    for (auto &&m : methods)
    {
        if (is_understood_method(m, classes_to_emit)) {
            return true;
        }
    }
    return false;
}

// If this has template arguments, see if they are in the list of classes to emit
bool check_template_arguments(const typename_info &info, const set<string> &classes_to_emit) {
    // If this is an element link, we skip a level.
    if (info.type_name == "ElementLink" && info.template_arguments[0].type_name == "DataVector") {
        return check_template_arguments(info.template_arguments[0].template_arguments[0], classes_to_emit);
    }

    // Otherwise, we should look at everything.
    for(auto && ta: info.template_arguments) {
        if (ta.template_arguments.size() > 0) {
            if (!check_template_arguments(ta, classes_to_emit)) {
                return false;
            }
        } else {
            if (classes_to_emit.find(ta.nickname) == classes_to_emit.end()) {
                return false;
            }
        }
    }

    // Everything checks out!
    return true;
}

set<string> _g_bad_root_libraries({
    "RIO.so",
    "Tree.so",
    "Graf.so",
    "Gui.so",
    "Hist.so",
    "TreePlayer.so",
    "Gpad.so",
});

set<string> _g_bad_root_classes({
    "TClonesArray",
    "TArray",
    "TArrayC",
    "TArrayD",
    "TArrayF",
    "TArrayI",
    "TAttAxis",
    "TBrowser",
    "TBroserImp",
    "TBuffer",
    "TBuffer3D",
    "TClass",
    "type_info"
});

// If this is a ROOT class that we do not want to have any part of
// our results...
bool is_root_only_class(const class_info &info) {
    if (_g_bad_root_libraries.find(info.library_name) != _g_bad_root_libraries.end()) {
        return false;
    }
    if (_g_bad_root_classes.find(info.name) != _g_bad_root_classes.end()) {
        return false;
    }
    if (info.library_name.size() == 0 && info.name[0] == 'T') {
        return false;
    }
    return true;
}

// Collections can be actual collections or just single items. Get
// the type correctly in both those cases.
string extract_container_iterator_type(const collection_info &c)
{
    switch (c.iterator_type_info.template_arguments.size()) {
        case 0:
            return c.iterator_type_info.nickname;
            break;
        
        case 1:
            return c.iterator_type_info.template_arguments[0].nickname;
            break;
        
        default:
            throw runtime_error("Do not know how to deal with the collectino of iterator type " + c.iterator_type_info.nickname);
    }
}

// These classes are known to be problematic
set<string> _g_bad_classes {
    "ROOT", "SG", "xAOD", "TObject", "SG::auxid_set_t"
};

// Inspect the class name. There are just some classes we
// should not be writing out under any circumstances.
bool class_name_is_good(const string &c_name) {
    return (_g_bad_classes.find(c_name) == _g_bad_classes.end()
        && (c_name.find("Eigen") == c_name.npos)
        && (c_name.find("SG::") == c_name.npos)
       );
}

// Given a list of arguments, dump them out.
void dump_arguments(const string &arg_list_name, const vector<method_arg> &arguments, YAML::Emitter &out) {
    bool first_argument = true;
    for (auto &&arg : arguments)
    {
        if (first_argument) {
            first_argument = false;
            out << YAML::Key << arg_list_name
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
}

int main(int argc, char**argv) {
    auto app_reference = create_root_app();

    // Parse the command line arguments
    options_description desc{"Options"};
    desc.add_options()
      ("help,h", "This message")
      ("library,l", value<vector<string>>(), "Load shared library")
      ("class,c", value<vector<string>>(), "Translate class");
    command_line_parser parser{argc, argv};
    parser.options(desc);
    auto parsed_options = parser.run();

    variables_map vm;
    store(parsed_options, vm);
    notify(vm);

    if (vm.count("help")) {
        cout << desc << endl;
        return 1;
    }

    if (vm.count("class") == 0) {
        cerr << "ERROR: Can't do class self discovery. You must provide the --class options!" << endl;
        return 1;
    }
    auto cmd_classes = vm["class"].as<vector<string>>();
    queue<string> classes_to_do;
    for (auto &&c_name : cmd_classes)
    {
        if (class_name_is_good(c_name)) {
            classes_to_do.push(c_name);
        }
    }

    if (vm.count("library") > 0) {
        auto libraries = vm["library"].as<vector<string>>();
        for (auto &&l_name : libraries)
        {
            auto status = gSystem->Load(l_name.c_str());
            if (status < 0) {
                cerr << "ERROR: Can't load library " << l_name << " - status: " << status << endl;
            }
        }
    }

    // Translate the classes from the ROOT system to our internal system, starting from
    // a given top level. Add all connected classes below that.
    set<string> classes_done;
    vector<class_info> done_classes;

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

            for (auto &&c_name : referenced_types(c))
            {
                if (class_name_is_good(c_name)) {
                    classes_to_do.push(c_name);
                }
            }        
        }
    }

    // Look at the loaded type defs, and add aliases.
    fixup_type_aliases(done_classes);

    // Fix up type defs. We have to wait to do this b.c. otherwise
    // ROOT won't load the typedefs
    fixup_type_defs(done_classes);

    // Build a class map
    map<string, class_info> class_map;
    for (auto &&c : done_classes)
    {
        class_map[c.name] = c;
    }

    // Get the list of containers from the classes. These will be top level collections
    // stored in the data.
    auto all_collections = find_collections(done_classes);
    auto single_collections = get_single_object_collections(done_classes);
    copy(single_collections.begin(), single_collections.end(),
        back_inserter(all_collections));

    // Start by looking at the classes that are connected to our
    // containers.
    classes_done.clear();
    set<string> classes_to_emit;
    for (auto &&c : all_collections)
    {
        auto c_name (extract_container_iterator_type(c));
        if (class_name_is_good(c_name)) {
            classes_to_do.push(c_name);
        }
    }

    // With that list of classes, lets find everything connected.
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

        // Now, add referenced classes to the queue
        auto reffed_classes = referenced_types(c_info->second);
        for (auto &&c_ref : reffed_classes)
        {
            if (class_name_is_good(c_ref)) {
                classes_to_do.push(c_ref);
            }
        }        
    }

    // Add some of the default types that need no introduction
    classes_to_emit.insert("bool");
    classes_to_emit.insert("double");
    classes_to_emit.insert("float");
    classes_to_emit.insert("short");
    classes_to_emit.insert("unsigned short");
    classes_to_emit.insert("int");
    classes_to_emit.insert("size_t");
    classes_to_emit.insert("unsigned int");
    classes_to_emit.insert("long");
    classes_to_emit.insert("unsigned long");
    classes_to_emit.insert("long long");
    classes_to_emit.insert("unsigned long long");
    classes_to_emit.insert("string");

    // Now, we need to loop through all of these things until we get a stable set of classes that we can emit.
    // This is painful, because there could be classes that look good, but contain no valid methods - so no need
    // to emit them. Cross them off the list, and another class' method is no longer interesting, in which case, that
    // has to be crossed. So we keep looping until we reach a stable set of classes.
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
                if (!check_template_arguments(class_info.name_as_type, classes_to_emit)) {
                    bad_classes.insert(c_name);
                }
                if (!is_root_only_class(class_info)) {
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

    // Finally, go through the collections and keep only the ones where we are
    // dumping out the classes they contain.
    vector<collection_info> collections;
    copy_if(all_collections.begin(), all_collections.end(), back_insert_iterator(collections),
        [&classes_to_emit](const collection_info &c_info) {
            string collection_iterator_typename(extract_container_iterator_type(c_info));
            return find_if(classes_to_emit.begin(), classes_to_emit.end(), [&collection_iterator_typename](const string &cl_info){
                return cl_info == collection_iterator_typename;
            }) != classes_to_emit.end();
        });

    // Dump them all out
    YAML::Emitter out;
    out << YAML::BeginMap
        << YAML::Key << "collections"
        << YAML::Value
        << YAML::BeginSeq;
    for (auto &&c : collections)
    {
        out << YAML::BeginMap
            << YAML::Key << "collection_name" << YAML::Value << c.name
            << YAML::Key << "cpp_item_type" << YAML::Value << extract_container_iterator_type(c)
            << YAML::Key << "python_item_type" << YAML::Value << normalized_type_name(extract_container_iterator_type(c))
            << YAML::Key << "cpp_container_type" << YAML::Value << c.type_info.nickname
            << YAML::Key << "python_container_type" << YAML::Value << normalized_type_name(c.iterator_type_info)
            << YAML::Key << "include_file" << YAML::Value << c.include_file
            << YAML::Key << "link_libraries" << YAML::Value << YAML::BeginSeq;

        for (auto &&lib : c.link_libraries)
        {
            out << lib;
        }
        out << YAML::EndSeq;

        auto meta_data_itr = _g_collection_config.find(c.name);
        if (meta_data_itr != _g_collection_config.end()) {
            auto &&meta_data = meta_data_itr->second;
            out << YAML::Key << "extra_parameters" << YAML::Value;
            out << YAML::BeginSeq;
            for (auto &&p : meta_data.parameters)
            {
                out << YAML::BeginMap;
                out << YAML::Key << "name" << YAML::Value << p.name;
                out << YAML::Key << "type" << YAML::Value << p.p_type;
                out << YAML::Key << "default_value" << YAML::Value << p.p_default;
                out << YAML::Key << "actions" << YAML::BeginSeq;

                for (auto && a: p.variable_actions) {
                    out << YAML::BeginMap;
                    out << YAML::Key << "value" << YAML::Value << a.value;
                    out << YAML::Key << "metadata_names" << YAML::Value << YAML::BeginSeq;
                    for (auto &&md : a.metadata_names) {
                        out << md;
                    }
                    out << YAML::EndSeq;
                    out << YAML::Key << "bank_rename" << YAML::Value << a.bank_rename;
                    out << YAML::EndMap;
                }

                out << YAML::EndSeq;
                out << YAML::EndMap;

            }
            
            out << YAML::EndSeq;
        }

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

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
        // TODO: this is not needed, delete and check.
        if (!can_emit_any_methods(c_info->second.methods, classes_to_emit)) {
            continue;
        }

        // If we can dump the class, then we should!
        out << YAML::BeginMap
            << YAML::Key << "python_name" << YAML::Value << normalized_type_name(c_info->second.name_as_type)
            << YAML::Key << "cpp_name" << YAML::Value << c_info->second.name_as_type.nickname;
                
        if (is_collection(c_info->second)) {
            auto container_typename = container_of(c_info->second);
            out << YAML::Key << "is_container_of_cpp" << YAML::Value << container_typename.nickname;
            out << YAML::Key << "is_container_of_python" << YAML::Value << normalized_type_name(container_typename);
        }
        
        if (c_info->second.include_file.size() > 0) {
            out << YAML::Key << "include_file" << YAML::Value << c_info->second.include_file;
        }

        if (c_info->second.class_behaviors.size() > 0) {
            out << YAML::Key << "also_behaves_like" << YAML::Value << YAML::BeginSeq;
            for(auto &&c : c_info->second.class_behaviors) {
                out << c;
            }
            out << YAML::EndSeq;
        }

        // Now we need to emit the methods.
        bool first_method = true;
        for (auto &&meth : c_info->second.methods)
        {
            if (is_understood_method(meth, classes_to_emit)) {
                if (first_method) {
                    out << YAML::Key << "methods"
                        << YAML::Value
                        << YAML::BeginSeq;
                    first_method = false;
                }

                auto rtn_type = parse_typename(meth.return_type);
                out << YAML::BeginMap
                    << YAML::Key << "name" << YAML::Value << meth.name
                    << YAML::Key << "return_type" << YAML::Value << rtn_type.nickname;

                dump_arguments("arguments", meth.arguments, out);
                dump_arguments("parameter_arguments", meth.parameter_arguments, out);

                if (meth.parameter_type_helper.size() > 0) {
                    out << YAML::Key << "param_helper" << YAML::Value << meth.parameter_type_helper;
                }

                if (meth.param_method_callback.size() > 0) {
                    out << YAML::Key << "param_type_callback" << YAML::Value << meth.param_method_callback;
                }

                out << YAML::EndMap;
            }
        }

        if (!first_method) {
            out << YAML::EndSeq;
        }
        

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    // Do the helper files
    emit_helper_files(out);

    out << YAML::EndMap;

    // Dump to the output
    cout << out.c_str() << endl;

    // Next, append the metadata file onto the end of this
    fstream metadata_in("metadata/r21.yaml");
    const int buf_size = 4096;
    char buf[buf_size];
    do {
        metadata_in.read(&buf[0], buf_size);
        cout.write(&buf[0], metadata_in.gcount());
    } while (metadata_in.gcount() > 0);     
}
