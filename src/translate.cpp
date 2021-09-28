#include "translate.hpp"
#include "normalize.hpp"

#include "TSystem.h"
#include "TClass.h"

#include <iostream>

void translate_class(const std::string &class_name)
{
    auto c_info = TClass::GetClass(class_name.c_str());
    if (c_info == nullptr) {
        std::cout << "ERROR: Cannot translate class " << class_name << ": ROOT's type system doesn't have it loaded." << std::endl;
        return;
    }
    auto n_classname = normalize_typename(c_info->GetName());
    std::cout << "INFO: Translating " << n_classname << std::endl;
}