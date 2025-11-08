#ifndef STRESS_STRAIN_H
#define STRESS_STRAIN_H

#include <vector>
#include <string>
#include "../matrix/stiffness_matrix.h"

class StressStrainAnalysis {
public:
    struct ElementStressStrain {
        int elementId;
        double strainX, strainY, shearStrainXY;
        double stressX, stressY, shearStressXY;
        double vonMisesStress;
        double centerX, centerY;
    };
    
    StressStrainAnalysis(StiffnessMatrix& mesh);
    
    // Compute strains and stresses for all elements
    std::vector<ElementStressStrain> computeStressStrain(const std::vector<double>& U);
    
    // Print statistics
    void printStatistics(const std::vector<ElementStressStrain>& results);
    
    // Export results to file
    void exportResults(const std::vector<ElementStressStrain>& results, 
                      const std::string& filename);
    
private:
    StiffnessMatrix& mesh;
    
    // Compute element strain from displacement
    void computeElementStrain(int elemId, const std::vector<double>& U,
                             double& ex, double& ey, double& gxy);
    
    // Compute stress from strain
    void computeStress(double ex, double ey, double gxy,
                      double& sx, double& sy, double& txy);
    
    // Compute von Mises stress
    double computeVonMises(double sx, double sy, double txy);
};

#endif
