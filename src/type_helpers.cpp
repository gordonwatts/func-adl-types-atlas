#include "type_helpers.hpp"
#include "class_info.hpp"

#include "TROOT.h"

#include <algorithm>
#include <iterator>

using namespace std;

///
// Return a type name without any qualifiers
//
string type_name(const string &qualified_type_name)
{
    string result(qualified_type_name);
    auto i = result.rfind(':');
    if (i != std::string::npos) {
        result = result.substr(i+1);
    }
    return result;
}

///
// Take list of types names and clean them up
//
vector<string> type_name(const vector<std::string> &qualified_type_names)
{
    vector<string> result;
    transform(qualified_type_names.begin(), qualified_type_names.end(),
                back_insert_iterator(result),
                [](const string &a) {return type_name(a);});
    return result;
}

set<string> type_name(const set<string> &qualified_type_names)
{
    auto v = type_name(vector<string>(qualified_type_names.begin(), qualified_type_names.end()));
    return set<string>(v.begin(), v.end());
}

string unqualified_type_name(const string &full_type_name)
{
    string result(full_type_name);
    auto s_idx = result.rfind('*');
    if (s_idx != string::npos) {
        result = result.substr(0, s_idx);
    }
    return result;
}

///
// Look through the list of typedefs, and add aliases for any classes
// we've already seen.
//
void fixup_type_aliases(vector<class_info> &classes)
{    
    // Build a typedef backwards mapping
	TIter i_typedef (gROOT->GetListOfTypes(true));
	int junk = gROOT->GetListOfTypes()->GetEntries();
	TDataType *typedef_spec;
    map<string, vector<string>> typdef_back_map;
	while ((typedef_spec = static_cast<TDataType*>(i_typedef.Next())) != 0)
	{
		string typedef_name = typedef_spec->GetName();
		string base_name = typedef_spec->GetFullTypeName();

        if (typedef_name != base_name) {
            typdef_back_map[base_name].push_back(typedef_name);
        }
    }

    // Loop through all the classes we are looking at to see if there is an alias we should be done.
    for (auto &&c : classes)
    {
        if (typdef_back_map.find(c.name) != typdef_back_map.end()) {
            c.aliases = typdef_back_map[c.name];
        }
    }
}

// Parse a horrendus typename into its various peices.
//
// "int"
// class_name::size_type
// class_name<t1,t2>::class_name2<t3, t4>::size_type
typename_info parse_typename(const string &type_name)
{
    // Go through, pulling out info of this thing.
    size_t index = 0;
    int template_counter = 0;
    string name;
    typename_info result;
    while (index < type_name.size()) {
        if(template_counter == 0) {
            if (type_name[index] == ':' && type_name[index+1] == ':') {
                result.namespace_list.push_back(parse_typename(name));
                index += 2;
            } else if (type_name[index] == '<') {
                template_counter = template_counter + 1;
                index += 1;
            }
        }
    }
}