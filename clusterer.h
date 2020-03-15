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
            std::string output;
            int bin;
            int k;
            bool color;
            std::vector<std::vector<int>> features;
            std::vector<std::string> classification;
        public: 
            KMeansClusterer(const std::string dataset, const std::string output, const int bins, const int clusters, const bool color);
            ~KMeansClusterer(void);
            void generateFeatures(void);
            void generateRGBFeatures(void);
            void cluster(void);
            std::vector<std::vector<int>> & getFeatures(void);
            int getFiles(void);
            double calcMeanIntensity(const std::vector<int> &feature);
            int calcCluster(const std::vector<int> &feature, const std::vector<double> &means);
            double calcEuclideanDistance(const std::vector<int> &feature, int mean);
            bool convergence(const std::vector<double> &means, const std::vector<double> &centroids);
            friend std::ostream& operator<<(std::ostream& os, const KMeansClusterer& kt);
    };
}

#endif