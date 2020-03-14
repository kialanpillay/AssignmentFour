#include <iostream>
#include "clusterer.h"
#include "boost/filesystem.hpp" 

using namespace std;

int main(int argc, char* argv[])
{
    string output = "";
    string dataset = argv[1];
    int bin = 10;
    int clusters = 10;

    if(argc < 2){
        cout << "Incorrect number of arguments!" << endl;
        exit(1);
    }

    for(int i = 2; i < argc; i+=2){
        string flag = argv[i];
        string arg = argv[i+1];
        if(flag.find("-o") != std::string::npos){
            output = arg;
        }
        if(flag.find("-k") != std::string::npos){
            clusters = stoi(arg);
        }
        if(flag.find("-b") != std::string::npos){
            bin = stoi(arg);
        }
    }

    PLLKIA010::Clusterer c(dataset, output, bin, clusters);
    cout << "K-Means Image Clustering" << std::endl;
    cout << "================================================" << std::endl;
    c.generateFeatures();
    return 0;
}
