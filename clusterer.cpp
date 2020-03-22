#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include "clusterer.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

using namespace PLLKIA010;

KMeansClusterer::KMeansClusterer(std::string d, std::string o, int b, int n, bool c, bool h): dataset(d),output(o), bin(b), k(n), color(c), hsv(h){}

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
    if(hsv){
        generateHSVFeatures();
    }
    else{
        if(color){
            generateRGBFeatures();
        }
        else{
            generateFeatures();
        }
    }
    
}

void KMeansClusterer::generateFeatures(){

    char buffer[128];
    std::string command = "cd " + dataset + " && ls";
    FILE* pipe = popen(command.c_str(), "r");
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL){
            std::string file = buffer;
            file.erase(std::remove(file.begin(), file.end(), '\n'),
            file.end());
            files.push_back(file);
        }
            
    }
    pclose(pipe);

    std::vector<int*> images(0);
    int width, height, rgb;
    int c = 0;
    if(files.size()==0){
        std::cout << "No images to cluster!";
        exit(1);
    }
    for (const auto& file : files) {
        
        std::ifstream in(dataset+"/"+file,std::ios::binary);
        std::string header;
        in >> header;
        in >> width >> height >> rgb;

        images.push_back(new int[height * width]);

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
    features.resize(images.size());
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

    char buffer[128];
    std::string command = "cd " + dataset + " && ls";
    FILE* pipe = popen(command.c_str(), "r");
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL){
            std::string file = buffer;
            file.erase(std::remove(file.begin(), file.end(), '\n'),
            file.end());
            files.push_back(file);
        }
            
    }
    pclose(pipe);
    if(files.size()==0){
        std::cout << "No images to cluster!";
        exit(1);
    }
    std::vector<int*> images(0);
    int width, height, rgb;
    int c = 0;
    for (const auto& file : files) {
        
        std::ifstream in(dataset+"/"+file,std::ios::binary);
        std::string header;
        in >> header;
        in >> width >> height >> rgb;

        images.push_back(new int[height * width]);
        images.push_back(new int[height * width]);
        images.push_back(new int[height * width]);
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
    features.resize(images.size());
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


std::vector<double> convertRGBtoHSV(const int red, const int green, const int blue){

    std::vector<double> hsv(3);
    double r = red/255.0;
    double g = green/255.0;
    double b = blue/255.0;
  
    double cmax = std::max(std::max(r, g), b); 
    double cmin = std::min(std::min(r, g), b);
    double diff = cmax-cmin;
  
    if (cmax == cmin){ 
        hsv[0] = 0;
    }  
    else if (cmax == r){
        hsv[0] = int(60 * ((g - b) / diff) + 360) % 360;
    }  
    else if (cmax == g){
        hsv[0] = int(60 * ((b - r) / diff) + 120) % 360;
    }
    else if (cmax == b){
        hsv[0] = int(60 * ((r - g) / diff) + 240) % 360;
    }
    if (cmax == 0){ 
        hsv[1] = 0;
    }
    else{ 
        hsv[1] = (diff / cmax) * 100;
    }
    hsv[2] = cmax * 100;
    return hsv;
    

}

void KMeansClusterer::generateHSVFeatures(){

    char buffer[128];
    std::string command = "cd " + dataset + " && ls";
    FILE* pipe = popen(command.c_str(), "r");
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL){
            std::string file = buffer;
            file.erase(std::remove(file.begin(), file.end(), '\n'),
            file.end());
            files.push_back(file);
        }
            
    }
    pclose(pipe);
    std::vector<int*> images(0);
    int width, height, rgb;
    int c = 0;
    if(files.size()==0){
        std::cout << "No images to cluster!";
        exit(1);
    }
    for (const auto& file : files) {
 
        std::ifstream in(dataset+"/"+file,std::ios::binary);
        std::string header;
        in >> header;
        in >> width >> height >> rgb;

        images.push_back(new int[height * width]);
        images.push_back(new int[height * width]);
        images.push_back(new int[height * width]);
        unsigned char pixels[3];  
        for (int j = 0; j < width * height; j++) { 
            in.read((char *)pixels, 3); 
            double r = pixels[0]; 
            double g = pixels[1]; 
            double b = pixels[2]; 
            std::vector<double> hsv = convertRGBtoHSV(r,g,b);
            images[c][j] = hsv[0];
            images[c+1][j] = hsv[1];
            images[c+2][j] = hsv[2];
        } 
        in.close();
        c+=3;
     
    }
    
    features.resize(images.size());
    for (int i = 0; i < images.size(); i++){
        if(i%3==0){
            std::vector<int>hist(361); //Hue Histogram
            for (int j = 0; j < width * height; j++){
  
                hist[images[i][j]]++;
            };
            features[i] = hist;
        }
        else{
            std::vector<int>hist(101); //S,V Histogram
            for (int j = 0; j < width * height; j++){
                hist[images[i][j]]++;
            };
            features[i] = hist;
        }
    };


    for (int i = 0; i < images.size(); i++){
        delete [] images[i];
    };
    
    images.clear();
}

bool KMeansClusterer::convergence(const std::vector<std::vector<int>> &means, const std::vector<std::vector<int>> &centroids){

    bool convergence = true;
    
    for(int i = 0; i < int(centroids.size()); i++){

            for(int j = 0; j < int(centroids[i].size()); j++){
                if(abs(means[i][j] - centroids[i][j]) > 0.000001){
                    convergence = false;
                }
            };
    };
    return convergence;
}

bool KMeansClusterer::simpleConvergence(const std::vector<double> &means, const std::vector<double> &centroids){

    bool convergence = true;
    for(int i = 0; i < int(centroids.size()); i++){
            if(abs(means[i] - centroids[i]) > 0.000001){
                convergence = false;
            }
    };
    return convergence;
}

std::string KMeansClusterer::cluster(){

    if(hsv){
        std::vector<double>means(k); 
        std::vector<double>centroids(k); //updated means
        std::vector<std::vector<double>>clusters(k); 
        classification.resize(k);
        srand((unsigned) time(0));
        for(int i = 0; i < k; i++){
            
            int r = (rand() % (features.size() - 3) + 1)-1;
            while(r%3!=0){
                r = (rand() % (features.size() - 3) + 1)-1;
            }
            //Forgy Init Method
            means[i] = HSVMean(featureMean(features[r]),featureMean(features[r+1]),featureMean(features[r+2]));
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
                int cluster = assignHSVCluster(features[i],features[i+1],features[i+2], means);
                int hsvMean = HSVMean(featureMean(features[i]),featureMean(features[i+1]),featureMean(features[i+2]));
                clusters[cluster].push_back(hsvMean);
                classification[cluster] += (files[i/3] + " ");
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
        while(!simpleConvergence(means, centroids));

        std::string results;
        results = "Classification\n";
        for(int i = 0; i < int(classification.size()); i++){
            results += "Cluster " + std::to_string(i) + " : " + classification[i] + "\n";
        };

        return results;
    }
    else if(color){
        std::vector<double>means(k); 
        std::vector<double>centroids(k); //updated means
        std::vector<std::vector<double>>clusters(k); 
        classification.resize(k);
        srand((unsigned) time(0));
        for(int i = 0; i < k; i++){
            
            int r = (rand() % (features.size() - 3) + 1)-1;
            while(r%3!=0){
                r = (rand() % (features.size() - 3) + 1)-1;
            }
            //Forgy Init Method
            means[i] = RGBMean(featureMean(features[r]),featureMean(features[r+1]),featureMean(features[r+2]));
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
                int rgbMean = RGBMean(featureMean(features[i]),featureMean(features[i+1]),featureMean(features[i+2]));
                clusters[cluster].push_back(rgbMean);
                classification[cluster] += (files[i/3] + " ");
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
        while(!simpleConvergence(means, centroids));

        std::string results;
        results = "Classification\n";
        for(int i = 0; i < int(classification.size()); i++){
            results += "Cluster " + std::to_string(i) + " : " + classification[i] + "\n";
        };

        return results;

    }
    else{
        std::vector<std::vector<int>>means(k); 
        std::vector<std::vector<int>>centroids(k); //updated means
        std::vector<std::vector<std::vector<int>>>clusters(k); 
        classification.resize(k);
        srand((unsigned) time(0));
        for(int i = 0; i < k; i++){
            
            int r = (rand() % features.size() + 1) - 1;
            means[i] = features[r];
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
                clusters[cluster].push_back(features[i]);
                classification[cluster] += (files[i] + " ");
            };
            
            for(int i = 0; i < int(centroids.size()); i++){

                if(clusters[i].size() == 0){
                    for(int j = 0; j < centroids[i].size(); j++){
                    
                        centroids[i][j] = 0;
            
                    };
                }
                else{

                    
                    for(int j = 0; j < clusters[i][0].size(); j++){
                        double sumPoints = 0;
                        for(int k = 0; k < clusters[i].size(); k++){
                        
                            sumPoints += clusters[i][k][j];
                
                        };
                        centroids[i][j] = sumPoints/double(clusters[i].size());


                
                    };

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

int KMeansClusterer::assignCluster(const std::vector<int> &feature, const std::vector<std::vector<int>> &means){

    int cluster = 0;
    double distance = 0;
    double min = 10000;
    for(int i = 0; i < int(means.size()); i++){
            distance = euclideanDistance(feature, means[i]);
            //std::cout << distance << std::endl;
            if(distance <= min){
                cluster = i;
                min = distance;
            }
    };
    return cluster;
 
}

double KMeansClusterer::euclideanDistance(const std::vector<int> &feature, const std::vector<int> &mean){
    int ss = 0;
    for(int i = 0; i < int(feature.size()); i++){
            ss += pow(feature[i] - mean[i],2);
    };
    return sqrt(ss);
}


int KMeansClusterer::assignRGBCluster(const std::vector<int> &r, const std::vector<int> &g, const std::vector<int> &b, const std::vector<double> &means){

    int cluster = 0;
    double d = 0;
    double min = 10000;
    for(int i = 0; i < int(means.size()); i++){
            d = distance(RGBMean(featureMean(r),featureMean(g),featureMean(b)), means[i]);
            if(d <= min){
                cluster = i;
                min = d;
            }
    };
    return cluster;
 
}

int KMeansClusterer::assignHSVCluster(const std::vector<int> &h, const std::vector<int> &s, const std::vector<int> &v, const std::vector<double> &means){

    int cluster = 0;
    double d = 0;
    double min = 10000;
    for(int i = 0; i < int(means.size()); i++){
            d = distance(HSVMean(featureMean(h),featureMean(s),featureMean(v)), means[i]);
            if(d <= min){
                cluster = i;
                min = d;
            }
    };
    return cluster;
 
}

double KMeansClusterer::distance(const int featureMean, int mean){
    return abs(featureMean - mean);
}

double KMeansClusterer::featureMean(const std::vector<int>& feature){

    int sum = 0;
    for(int i = 0; i < int(feature.size()); i++){
            sum += feature[i] * (i*bin);
    };
    return sum/feature.size();
 
}



double KMeansClusterer::RGBMean(const double r, const double g, const double b){
    return (r+g+b)/3;
}

double KMeansClusterer::HSVMean(const double h, const double s, const double v){
    return (h+s+v)/3;
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


