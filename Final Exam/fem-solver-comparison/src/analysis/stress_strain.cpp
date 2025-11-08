#include "stress_strain.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

StressStrainAnalysis::StressStrainAnalysis(StiffnessMatrix& mesh) : mesh(mesh) {}

std::vector<StressStrainAnalysis::ElementStressStrain> 
StressStrainAnalysis::computeStressStrain(const std::vector<double>& U) {
    std::cout << "\n=== COMPUTING ELEMENT STRESSES AND STRAINS ===" << std::endl;
    
    const auto& elements = mesh.getElements();
    const auto& nodes = mesh.getNodes();
    std::vector<ElementStressStrain> results;
    
    for (const auto& elem : elements) {
        ElementStressStrain ess;
        ess.elementId = elem.id;
        
        // Compute element center
        ess.centerX = 0.0;
        ess.centerY = 0.0;
        for (int i = 0; i < 4; i++) {
            ess.centerX += nodes[elem.nodes[i]].x;
            ess.centerY += nodes[elem.nodes[i]].y;
        }
        ess.centerX /= 4.0;
        ess.centerY /= 4.0;
        
        // Compute strain at element center (xi=0, eta=0)
        computeElementStrain(elem.id, U, ess.strainX, ess.strainY, ess.shearStrainXY);
        
        // Compute stress
        computeStress(ess.strainX, ess.strainY, ess.shearStrainXY,
                     ess.stressX, ess.stressY, ess.shearStressXY);
        
        // Compute von Mises stress
        ess.vonMisesStress = computeVonMises(ess.stressX, ess.stressY, ess.shearStressXY);
        
        results.push_back(ess);
    }
    
    std::cout << "Computed stresses for " << results.size() << " elements" << std::endl;
    
    return results;
}

void StressStrainAnalysis::computeElementStrain(int elemId, const std::vector<double>& U,
                                                double& ex, double& ey, double& gxy) {
    // Evaluate at element center (xi=0, eta=0)
    std::vector<std::vector<double>> B;
    mesh.computeBMatrix(0.0, 0.0, elemId, B);
    
    // Extract element displacement vector
    const auto& elem = mesh.getElements()[elemId];
    std::vector<double> Ue(8);
    for (int i = 0; i < 4; i++) {
        Ue[2*i] = U[2 * elem.nodes[i]];
        Ue[2*i + 1] = U[2 * elem.nodes[i] + 1];
    }
    
    // {strain} = [B]{Ue}
    ex = 0.0;
    ey = 0.0;
    gxy = 0.0;
    
    for (int i = 0; i < 8; i++) {
        ex += B[0][i] * Ue[i];
        ey += B[1][i] * Ue[i];
        gxy += B[2][i] * Ue[i];
    }
}

void StressStrainAnalysis::computeStress(double ex, double ey, double gxy,
                                        double& sx, double& sy, double& txy) {
    // Get constitutive matrix
    auto D = mesh.getConstitutiveMatrix();
    
    // {stress} = [D]{strain}
    sx = D[0][0] * ex + D[0][1] * ey;
    sy = D[1][0] * ex + D[1][1] * ey;
    txy = D[2][2] * gxy;
}

double StressStrainAnalysis::computeVonMises(double sx, double sy, double txy) {
    // von Mises stress: sqrt(sx^2 - sx*sy + sy^2 + 3*txy^2)
    return sqrt(sx*sx - sx*sy + sy*sy + 3.0*txy*txy);
}

void StressStrainAnalysis::printStatistics(const std::vector<ElementStressStrain>& results) {
    if (results.empty()) return;
    
    std::cout << "\n=== STRESS/STRAIN STATISTICS ===" << std::endl;
    
    double maxStressX = -1e100, minStressX = 1e100;
    double maxStressY = -1e100, minStressY = 1e100;
    double maxVonMises = -1e100;
    
    for (const auto& r : results) {
        if (r.stressX > maxStressX) maxStressX = r.stressX;
        if (r.stressX < minStressX) minStressX = r.stressX;
        if (r.stressY > maxStressY) maxStressY = r.stressY;
        if (r.stressY < minStressY) minStressY = r.stressY;
        if (r.vonMisesStress > maxVonMises) maxVonMises = r.vonMisesStress;
    }
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Stress X (σx): " << minStressX/1e6 << " to " << maxStressX/1e6 << " MPa" << std::endl;
    std::cout << "Stress Y (σy): " << minStressY/1e6 << " to " << maxStressY/1e6 << " MPa" << std::endl;
    std::cout << "Max von Mises: " << maxVonMises/1e6 << " MPa" << std::endl;
}

void StressStrainAnalysis::exportResults(const std::vector<ElementStressStrain>& results,
                                         const std::string& filename) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    // Write header
    file << "# Element stress/strain results" << std::endl;
    file << "# ElemID CenterX CenterY StrainX StrainY ShearXY StressX StressY ShearXY VonMises" << std::endl;
    
    for (const auto& r : results) {
        file << r.elementId << " "
             << r.centerX << " " << r.centerY << " "
             << r.strainX << " " << r.strainY << " " << r.shearStrainXY << " "
             << r.stressX << " " << r.stressY << " " << r.shearStressXY << " "
             << r.vonMisesStress << std::endl;
    }
    
    file.close();
    std::cout << "Results exported to: " << filename << std::endl;
}
