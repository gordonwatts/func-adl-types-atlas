#include <iostream>
#include "TSystem.h"

using namespace std;

int main(int, char**) {
    cout << "Hello, world!" << endl;
    cout << gSystem->GetBuildCompilerVersion() << endl;
}
