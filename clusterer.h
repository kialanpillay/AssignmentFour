#ifndef CLUSTERER_H
#define CLUSTERER_H
#include <string>
#include <vector>

namespace PLLKIA010
{
    class KMeansClusterer
    {   
        private: 
            std::string dataset;
            int bin;
            int k;
            bool color;
            bool hsv;
            int iterations;
            std::vector<std::vector<int>> features;
            std::vector<std::string> files;
            std::vector<std::string> classification;
        public: 
            KMeansClusterer();
            KMeansClusterer(const std::string dataset, const int bins, const int clusters, const bool color, const bool hsv);
            ~KMeansClusterer(void);
            void generate(void);
            void generateFeatures(void);
            void generateRGBFeatures(void);
            void generateHSVFeatures(void);
            void cluster(void);
            std::string results(void) const;
            double featureMean(const std::vector<int> &feature) const;
            double RGBMean(const double r, const double g, const double b) const;
            int assignCluster(const std::vector<int> &feature, const std::vector<std::vector<int>> &means) const;
            int assignRGBCluster(const std::vector<int> &rfeature, const std::vector<int> &gfeature, const std::vector<int> &bfeature, const std::vector<double> &means) const;
            int assignHSVCluster(const std::vector<int> &hfeature, const std::vector<int> &sfeature, const std::vector<int> &vfeature, const std::vector<std::vector<int>> &means) const;
            double distance(const int featureMean, const int mean) const;
            double euclideanDistance(const std::vector<int> &feature, const std::vector<int> &mean) const;
            double similarity(const std::vector<std::vector<int>> feature, const std::vector<std::vector<int>> mean) const;
            bool simpleConvergence(const std::vector<double> &means, const std::vector<double> &centroids) const;
            bool convergence(const std::vector<std::vector<int>> &means, const std::vector<std::vector<int>> &centroids);
            friend std::ostream& operator<<(std::ostream& os, const KMeansClusterer& kt);
    };
    std::ostream& operator<<(std::ostream& os, const KMeansClusterer& kt);  
}

#endif