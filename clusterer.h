#ifndef CLUSTERER_H
#define CLUSTERER_H
#include <string>
#include <vector>

namespace PLLKIA010
{
    class Clusterer
    {
        private: 
            std::string dataset;
            std::string output;
            int bin;
            int clusters;
            std::vector<std::vector<int>> features;
        public: 
            Clusterer(const std::string dataset, const std::string output, const int bins, const int clusters);
            ~Clusterer(void);
            void generateFeatures(void);
            std::vector<std::vector<int>> & getFeatures(void);
            int getFiles(void);
    };
}

#endif