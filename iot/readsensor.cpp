#include <iostream>
#include <string>
#include <sstream>
using namespace std;
template <typename T>
    T FromString(const string &Text)
    {
        istringstream ss(Text);
        T result;
        return ss >> result ? result : 0;
    };

int main(int argc, char* argv[])
{
    int data[]= {240,860,468,681,303,565};
    string str=argv[1];
    int v=FromString<int>(str);
    int r=0;
    if(v<6 && v>=0){
        r=data[v];
    }
    cout<<r;
    return 0;
}
