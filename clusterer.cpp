#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "clusterer.h"
#include <filesystem>
#include <stdio.h>
#include <math.h>
#include <algorithm>

using namespace PLLKIA010;

KMeansClusterer::KMeansClusterer(std::string o, std::string d, int b, int n, bool c): output(o), dataset(d), bin(b), k(n), color(c){
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

void KMeansClusterer::generate(){
    if(color){
        generateRGBFeatures();
    }
    else{
        generateFeatures();
    }
}

void KMeansClusterer::generateFeatures(){

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
    int entries = ceil(double(rgb+1)/double(bin));

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

    std::vector<int*> images(getFiles()*3);
    int width, height, rgb;
    int c = 0;
    for (const auto& file : std::__fs::filesystem::directory_iterator(dataset)) {
        
        const auto filename = file.path().filename().string();
        
        std::ifstream in(dataset+"/"+filename,std::ios::binary);
        std::string header;
        in >> header;
        in >> width >> height >> rgb;

        images[c] = new int[height * width];
        images[c+1] = new int[height * width];
        images[c+2] = new int[height * width];
        unsigned char pixels[3];  
        for (int j = 0; j < width * height; j++) { 
            in.read((char *)pixels, 3); 
            float r = pixels[0]; 
            float g = pixels[1]; 
            float b = pixels[2]; 
            images[c][j] = r;
            images[c+1][j] = g;
            images[c+2][j] = b;
        } 
        in.close();
        c+=3;
     
    }
    
    int entries = ceil(double(rgb+1)/double(bin));
    
    for (int i = 0; i < images.size(); i++){
        std::vector<int>hist(entries);
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
    for(int i = 0; i < int(centroids.size()); i++){
            if(abs(means[i] - centroids[i]) > 0.000001){
                convergence = false;
            }
    };
    return convergence;
}

std::string KMeansClusterer::cluster(){

    if(color){
        std::vector<double>means(k); 
        std::vector<double>centroids(k); //updated means
        std::vector<std::vector<double>>clusters(k); 
        classification.resize(k);
        srand((unsigned) time(0));
        for(int i = 0; i < k; i++){
            
            int r = (rand() % getFiles()*3 + 1)-1;
            //Forgy Init Method
            means[i] = calcRGBMeanIntensity(calcMeanIntensity(features[r]),calcMeanIntensity(features[r+1]),calcMeanIntensity(features[r+2]));
        };
        std::sort(means.begin(), means.end());
        centroids = means;
        do{
            classification.clear();
            classification.resize(k);
            clusters.clear();
            clusters.resize(k);
            means = centroids;
            
            for(int i = 0; i < int(features.size()); i+=3){
                int cluster = assignRGBCluster(features[i],features[i+1],features[i+2], means);
                int rgbMean = calcRGBMeanIntensity(calcMeanIntensity(features[i]),calcMeanIntensity(features[i+1]),calcMeanIntensity(features[i+2]));
                clusters[cluster].push_back(rgbMean);
                classification[cluster] += (std::to_string(i/3) + " ");
            };
            
            for(int i = 0; i < int(centroids.size()); i++){


                double sumPoints = 0;
                for(int k = 0; k < int(clusters[i].size()); k++){
                    sumPoints += clusters[i][k];
            
                };
                if(int(clusters[i].size()) == 0){
                    centroids[i] = 0;
                }
                else{
                    centroids[i] = sumPoints/double(clusters[i].size());
                }
            
            };

            
        }
        while(!convergence(means, centroids));

        std::string results;
        results = "Classification\n";
        for(int i = 0; i < int(classification.size()); i++){
            results += "Cluster " + std::to_string(i) + " : " + classification[i] + "\n";
        };

        return results;

    }
    else{
        std::vector<double>means(k); 
        std::vector<double>centroids(k); //updated means
        std::vector<std::vector<double>>clusters(k); 
        classification.resize(k);
        srand((unsigned) time(0));
        for(int i = 0; i < k; i++){
            
            int r = (rand() % getFiles() + 1) - 1;
            means[i] = calcMeanIntensity(features[r]);
        };
        centroids = means;
        do{
            classification.clear();
            classification.resize(k);
            clusters.clear();
            clusters.resize(k);
            means = centroids;
            
            for(int i = 0; i < int(features.size()); i++){

                int cluster = assignCluster(features[i], means);
                clusters[cluster].push_back(calcMeanIntensity(features[i]));
                classification[cluster] += (std::to_string(i) + " ");
            };
            
            for(int i = 0; i < int(centroids.size()); i++){


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
            };

            
        }
        while(!convergence(means, centroids));

        std::string results;
        results = "Classification\n";
        for(int i = 0; i < int(classification.size()); i++){
            results += "Cluster " + std::to_string(i) + " : " + classification[i] + "\n";
        };
        return results;

    }
    
}


int KMeansClusterer::assignCluster(const std::vector<int> &feature, const std::vector<double> &means){

    int cluster = 0;
    double distance = 0;
    double min = 1000;
    for(int i = 0; i < int(means.size()); i++){
            distance = calcEuclideanDistance(calcMeanIntensity(feature), means[i]);
            if(distance <= min){
                cluster = i;
                min = distance;
            }
    };
    return cluster;
 
}

int KMeansClusterer::assignRGBCluster(const std::vector<int> &r, const std::vector<int> &g, const std::vector<int> &b, const std::vector<double> &means){

    int cluster = 0;
    double distance = 0;
    double min = 1000;
    for(int i = 0; i < int(means.size()); i++){
            distance = calcEuclideanDistance(calcRGBMeanIntensity(calcMeanIntensity(r),calcMeanIntensity(g),calcMeanIntensity(b)), means[i]);
            if(distance <= min){
                cluster = i;
                min = distance;
            }
    };
    return cluster;
 
}

double KMeansClusterer::calcEuclideanDistance(const int featureIntensity, int mean){
    return abs(featureIntensity - mean);
}

double KMeansClusterer::calcMeanIntensity(const std::vector<int>& feature){

    int sum = 0;
    for(int i = 0; i < int(feature.size()); i++){
            sum += feature[i] * i;
    };
    return sum/feature.size();
 
}

double KMeansClusterer::calcRGBMeanIntensity(const double r, const double g, const double b){

    return (r+g+b)/3;
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

std::ostream& PLLKIA010::operator<<(std::ostream& os, const KMeansClusterer& kt){ 
    KMeansClusterer k = kt;
    k.generate();
    if(k.output == "std"){
        os << k.cluster();
    }
    else{
        std::ofstream out(k.output);
        out << k.cluster();
        os << "Clustering Results Saved!";
    }
    return os;
}


