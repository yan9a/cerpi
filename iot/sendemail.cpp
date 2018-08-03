#include <iostream>
#include <stdlib.h>
#include <string>
using namespace std;
int main()
{
    string cmdstr="mpack -s \"Test c++ pi email\" /home/pi/rpi/iot/emailbody.txt yan9aye@gmail.com";
    int r=system(cmdstr.c_str());
    cout<<cmdstr<<endl;
    cout<<"Return: "<<r<<endl;
    return r;
}
