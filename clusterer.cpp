#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "clusterer.h"
#include <filesystem>
#include <math.h>

using namespace PLLKIA010;

KMeansClusterer::KMeansClusterer(std::string d, std::string o, int b, int n, bool c): dataset(d), output(o), bin(b), k(n), color(c){
    if(color){
        features.resize(getFiles()*3);
    }
    else{
        features.resize(getFiles());
    }

}

KMeansClusterer::~KMeansClusterer(){
    if(!features.empty())
    {
        for (int i = 0; i < features.size(); i++){
            features[i].clear();
        };

        features.clear();
    }
}

std::vector<std::vector<int>> &  KMeansClusterer::getFeatures(){
    return features;
}

void KMeansClusterer::generateFeatures(){

    std::vector<int*> images(getFiles());
    int width, height, rgb;
    int c = 0;
    for (const auto& file : std::__fs::filesystem::directory_iterator(dataset)) {
        
        const auto filename = file.path().filename().string();
        std::cout << c << filename << std::endl;
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
    int entries = ceil(double(rgb)/double(bin));

    for (int i = 0; i < images.size(); i++){
        std::vector<int>hist(0);
        hist.resize(entries);

        for (int j = 0; j < width * height; j++){
            int b = floor(images[i][j]/bin); 

            hist[b]++;
        };
        features[i] = std::move(hist);
        
    };
    for (int i = 0; i < images.size(); i++){
        delete [] images[i];
    };
    
    images.clear();
    
}

void KMeansClusterer::generateRGBFeatures(){

    std::vector<int*> images(getFiles());
    int width, height, rgb;
    int c = 0;
    for (const auto& file : std::__fs::filesystem::directory_iterator(dataset)) {
        
        const auto filename = file.path().filename().string();
        
        std::ifstream in(dataset+"/"+filename,std::ios::binary);
        std::string header;
        in >> header;
        in >> width >> height >> rgb;

        images[c] = new int[height * width * 3];

        unsigned char pixels[3];  
        for (int j = 0; j < width * height * 3; j+=3) { 
            in.read((char *)pixels, 3); 
            float r = pixels[0]; 
            float g = pixels[1]; 
            float b = pixels[2]; 
            images[c][j] = r;
            images[c][j+1] = g;
            images[c][j+2] = b;
        } 
        in.close();
        ++c;
     
    }
    int entries = rgb/bin;
    
    for (int i = 0; i < images.size() * 3; i++){
        std::vector<int>hist(0);
        hist.resize(entries);
        for (int j = 0; j < width * height; j++){
            int b = floor(images[i][j]/bin);   
            hist[b]++;
        };
        features[i] = hist;
    };


    for (int i = 0; i < images.size(); i++){
        delete [] images[i];
    };
    
    images.clear();
    
}

bool KMeansClusterer::convergence(const std::vector<double> &means, const std::vector<double> &centroids){

    bool convergence = true;
    for(int i = 0; i < centroids.size(); i++){
            if(abs(means[i] - centroids[i]) > 0.000001){
                convergence = false;
            }
    };
    return convergence;
}

void KMeansClusterer::cluster(){

    if(color){

    }
    else{
        std::vector<double>means(k); 
        std::vector<double>centroids(k); //updated means
        std::vector<std::vector<double>>clusters(k); 
        classification.resize(k);
        srand((unsigned) time(0));
        for(int i = 0; i < k; i++){
            
            int r = (rand() % getFiles() + 1);
            means[i] = calcMeanIntensity(features[r]);
        };
        centroids = means;
        do{
            classification.clear();
            classification.resize(k);
            clusters.clear();
            clusters.resize(k);
            for(int i = 0; i < k; i++){
                std::cout << means[i] << " " << centroids[i] << std::endl;
            };
            means = centroids;
            
            for(int i = 0; i < features.size(); i++){

                int cluster = calcCluster(features[i], means);
                clusters[cluster].push_back(calcMeanIntensity(features[i]));
                classification[cluster] += (std::to_string(i) + " ");
            };
            
            for(int i = 0; i < centroids.size(); i++){


                double sumPoints = 0;
                for(int k = 0; k < clusters[i].size(); k++){
                    sumPoints += clusters[i][k];
            
                };
                if(clusters[i].size() == 0){
                    centroids[i] = 0;
                }
                else{
                    centroids[i] = sumPoints/double(clusters[i].size());
                }
                
                std::cout << clusters[i].size() << " C SIZE : CEN: "  << centroids[i] << " SP " <<  sumPoints << std::endl;
                

            };

            
        }
        while(!convergence(means, centroids));

        std::cout << "Classification" << std::endl;
        for(int i = 0; i < classification.size(); i++){
            std::cout << "Cluster " << i << " : " << classification[i] << std::endl;
        };
        
        
        

 


    }
    
}

int KMeansClusterer::calcCluster(const std::vector<int> &feature, const std::vector<double> &means){

    int cluster = 0;
    double distance = 0;
    double min = 1000;
    for(int i = 0; i < means.size(); i++){
            distance = calcEuclideanDistance(feature, means[i]);
            if(distance <= min){
                cluster = i;
                min = distance;
            }
    };
    return cluster;
 
}

double KMeansClusterer::calcEuclideanDistance(const std::vector<int> &feature, int mean){
    int featureIntensity = calcMeanIntensity(feature);
    return abs(featureIntensity - mean);
}

double KMeansClusterer::calcMeanIntensity(const std::vector<int>& feature){

    int sum = 0;
    for(int i = 0; i < (256/bin); i++){
            sum += feature[i] * i;
    };
    return sum/bin;
 
}



int KMeansClusterer::getFiles(void){
    
    int fileCount = 0;
    
    for (const auto& file : std::__fs::filesystem::directory_iterator(dataset)) {

        if (file.is_regular_file()) {
            fileCount++;
        }
    }
    return fileCount;
} 

std::ostream& operator<<(std::ostream& os, const KMeansClusterer& kt){

    return os;
}


