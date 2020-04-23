# Assignment Four: PLLKIA010

## Makefile
The purpose of the Makefile is to automatically compile and link the C++ source files into a binary executable that can be run locally. Run these commands in sequence to interact with the program.
```make``` will compile the files into an executable called ```clusterer```
\\
```make run``` will run the executable with the default arguments (Greyscale features, std::cout, clusters = 10, binSize = 1)\\
This will cluster the provided image files and write the results to standard output.
\\
```make clean``` will delete all object files, text files, and the executable. 
\\
This is the format of arguments that can be passed into the program, where n is the number of clusters and b is the size of the histogram feature bin. 
\\
```./clusterer <dataset> [-o output] [-k n] [-bin b]```
\\
To specify the that color (RGB) histogram features should be used, invoke the program as follows:\\
```./clusterer <dataset> [-o output] [-k n] [-bin b] -color```
\\
To specify the that HSV features should be used, invoke the program as follows:\\
```./clusterer <dataset> [-o output] [-k n] -hsv```\\
Note that bin size cannot be specified for these features.\\
The implementation of these features will be detailed below. 

## clusterer.h

This is a header file that contains the ```KMeansClusterer``` Class declaration. This class contains all the required methods and data members to create a fully functional clusterer. 
\\
The class contains helper variables that store the number of clusters (K), histogram feature bin size, dataset name, output format, and feature type (color/hsv). Additionally, the clusterer stores the maximum number of iterations, to force completion if the algorithm does not converge. 
\\The class also contains three vectors, one to store the features to be clustered, the second to store the clustered filenames, and the final vector to store the image classification results as the algorithm executes.
\\ Each class implements several methods that constitute it's functionality. The clusterer is designed in a intentionally modular fashion. Each type of feature (Greyscale/RGB/HSV) has its own generation method. There is a single clustering method, that uses conditional logic to cluster the generated features correctly based on type. There are specific cluster assignment methods for each feature type, along with distance measure functions (Greyscale/RGB features use a standard Euclidean distance measure, while HSV features use a formulated similarity score. See HSV Feature Implementation for more details). There are also two different helper functions to check for convergence (used when clustering Greyscale/HSV or RB features). Finally, the class friends the `<<` operator overload, in order to make clusterer results viewing simple. 
\\
This header file also contains the method decleration for the overloaded operator, which returns a reference to a ostream object.

## clusterer.cpp

This file is the actual implementation of all the methods declared in the ```clusterer.h``` file. The implementation of some methods is explained below. \\
The Constructor and Destructor are responsible for initialising the member variables, and clearing the vectors respectively.
\\
generateFeatures: This method iterates through the dataset (directory of files), and reads the image as binary into an unsigned_char array, Each byte triple is extracted and used to compute the greyscale value, or stored as seperate image features (RGB/HSV Features). These images are then processed into histogram features, with each bin containing the frequency of that pixel/range of pixels.
\\
cluster: Regardless on the type of feature, the cluster method functions using the same principle. It implements a naive K-Means algorithm, also known as Lloyd's algorithm. The chosen centroid initialization method is Forgy, which the literature has shown to be more accurate when using Lloyd's algorithm, compared to other methods. Forgy's method involves selecting K random observations from the dataset, and using these as the initial means. For each feature (or set of features that represents the image (RGB/HSV)), the feature is assigned to a cluster, and the classification results are stored. Based on these clusters, the initial means/centroids are recomputed, by summing the observations and dividing by the size of the cluster, and the exisiting centroids become the old centroids. This process iterates until convergence is reached, i.e. centroids in the ith iteration are equal to the centroids in the i-1th iteration. For RGB and HSV features, each mean consits of a feature triple that represents a single observation/image. Each of these features are recomputed to create the updated K means after each iteration.
\\
assignCluster: This method compares the distance measure of a feature to each of the means, to determine the best cluster for the observation to be assigned to.
\\
euclideanDistance: This helper functions calculates the euclidean distance between two sets of points (two observations).


## driver.cpp

This file contains the main method, and provides an entry point into the program. All interaction with the program is done via a Command Line Interface, with several supported commands;  users pass a set of arguments. It imports the ```clusterer.h``` file in order to provide the K-means Clustering functionality. The main method has an ```KMeansClusterer``` object, and uses the overloaded operator to view the results of the clusterer.

## HSV Feature Implementation
This idea for this image feature is adapted from the [Local vs. Global Histogram-Based Color Image Clustering](https://pdfs.semanticscholar.org/2dd3/e875bc2141e68bbf2bd9ebb9160c2a76abea.pdf) article, authored by
Bongani Malinga, Daniela Raicu, Jacob Furst at DePaul University. I found using HSV values to be intriguing and decided to imeplement it in order to see if it improved the accuracy of the clustering. This clusterer does not implemented the clustering in the same way (the original experiment partitions the image first), but the feature generation and similarity measure is closely inspired. For feature generation, as with RGB features, three image features are generated per image (three channels). However, the RGB triple is converted to the HSV equivalanent using a helper function. 
\\
Once these image features are computed, for each observation, three histogram features are generated. For my implementation, you cannot specify a bin size, which mirrors the source work, however the histogram widths differ. The H histogram has a width of 361, to account for 361 Hue values (Hue is measured as an angle), and the Saturation and Value histograms have a width of 101, since their values are scaled in this range. 
\\
The clustering process is the same as defined above, with each feature triple that represents the image being compared to each of the K means to determine the best cluster, and the process iterating until the centroids/means converge.
\\However, the key difference is that Euclidean distance is not used as the measure of similarity/closeness, a custom similarity index is used instead. This is defined in the above article, with the only difference in implemention being that means of the histograms are used when working out the individual similarity components. Addtionally since the image is not partioned into blocks, only one overall similarity measure is required. 
\\
The HSV feature triple is also weighted using the same weights as the article, with 0.25 for H, 0.05 for S, and V being excluded. The idea behind this is that humans are more perceptive to hue. 
\\
This feature implementation was found not to converge, so the clusterer runs for a set number of iterations (50). The results also indicate that there is no real increase in clustering accuracy, with runs often producing poorer clustering results. The computation time is also much longer than using the Greyscale/RGB features.
