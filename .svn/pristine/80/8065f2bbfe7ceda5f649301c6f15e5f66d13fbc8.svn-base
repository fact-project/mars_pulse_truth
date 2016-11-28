#include <iostream>

#include <string.h> // strcmp (Ubuntu 8.10)

#include <RVersion.h>

using namespace std;

static void Usage()
{
    cout << endl;
    cout << "Sorry the usage is:" << endl;
    cout << "   mars-config [--root-ver|--mars-ver]" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  --root-ver  Show root version which were used for compilation." << endl;
    cout << "              To be compared with the output of \"root-config --version\"" << endl;
    cout << "  --version   Show mars version." << endl;
    cout << endl;
    cout << "Only one command line option is allowed." << endl;
    cout << endl;
}

int main(int argc, char *const argv[])
{
    if (argc!=2)
    {
        Usage();
        return 2;
    }

    if (!strcmp(argv[1], "--root-ver"))
        cout << ROOT_RELEASE << endl;
    if (!strcmp(argv[1], "--version"))
        cout << MARSVER << endl;

    return 0;
}
