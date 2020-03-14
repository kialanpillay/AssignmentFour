#include <iostream>
#include <fstream>
#include <sstream>
#include "clusterer.h"
#include <filesystem>
#include <math.h>

using namespace PLLKIA010;

Clusterer::Clusterer(std::string d, std::string o, int b, int n): dataset(d), output(o), bin(b), clusters(n){
    features.resize(getFiles());
}

Clusterer::~Clusterer(){
    /*if(!features.empty())
    {
        for (int i = 0; i < features.size(); i++)
        {
            for (int j = 0; j < 32; j++)
            {
                delete [] features[i][j];
            }
            delete [] features[i];
        };

        features.clear();
    };*/
}

std::vector<std::vector<int>> &  Clusterer::getFeatures(){
    return features;
}

void Clusterer::generateFeatures(){

    std::vector<int*> images(getFiles());
    int width, height, rgb;
    int c = 0;
    for (const auto& file : std::__fs::filesystem::directory_iterator(dataset)) {
        
        const auto filename = file.path().filename().string();
        
        std::ifstream in(dataset+"/"+filename,std::ios::binary);
        std::string header;
        in >> header;
        in >> width >> height >> rgb;

        images[c] = new int[height * width];

        unsigned char pixels[3];  
        for (int j = 0; j < width * height; j++) { 
            in.read((char *)pixels, 3); 
            float r = pixels[0]; 
            float g = pixels[1]; 
            float b = pixels[2]; 
            images[c][j] = 0.21 * r + 0.72 * g + 0.07 * b;
        } 
        in.close();
        ++c;
     
    }
    int entries = rgb/bin;
    
    for (int i = 0; i < images.size(); i++){
        std::vector<int>hist(0);
        hist.resize(entries);
        for (int j = 0; j < width * height; j++){
            int b = floor(images[i][j]/bin);   
            hist[b]++;
        };
        features[i] = hist;
    };
    
    for (int i = 0; i < 1; i++){
        std::cout << features[0][i] << " ";
    };
    for (int i = 0; i < images.size(); i++){
        delete [] images[i];
    };
    
    images.clear();
    
}

int Clusterer::getFiles(void){
    
    int fileCount = 0;
    
    for (const auto& file : std::__fs::filesystem::directory_iterator(dataset)) {

        if (file.is_regular_file()) {
            fileCount++;
        }
    }
    return fileCount;
} 


