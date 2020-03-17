#ifndef CLUSTERER_H
#define CLUSTERER_H
#include <string>
#include <vector>

namespace PLLKIA010
{
    class KMeansClusterer
    {   
        public:
            std::string output = "";
        private: 
            std::string dataset;
            int bin;
            int k;
            bool color;
            std::vector<std::vector<int>> features;
            std::vector<std::string> classification;
        public: 
            KMeansClusterer(const std::string dataset, const std::string output, const int bins, const int clusters, const bool color);
            ~KMeansClusterer(void);
            void generate(void);
            void generateFeatures(void);
            void generateRGBFeatures(void);
            std::string cluster(void);
            std::vector<std::vector<int>> & getFeatures(void);
            int getFiles(void);
            double calcMeanIntensity(const std::vector<int> &feature);
            double calcRGBMeanIntensity(const double r, const double g, const double b);
            int assignCluster(const std::vector<int> &feature, const std::vector<double> &means);
            int assignRGBCluster(const std::vector<int> &rfeature, const std::vector<int> &gfeature, const std::vector<int> &bfeature, const std::vector<double> &means);
            double calcEuclideanDistance(const int featureIntensity, int mean);
            double scoreClusterer(void);
            bool convergence(const std::vector<double> &means, const std::vector<double> &centroids);
            friend std::ostream& operator<<(std::ostream& os, const KMeansClusterer& kt);
    };
    std::ostream& operator<<(std::ostream& os, const KMeansClusterer& kt);  
}

#endif