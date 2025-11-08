#ifndef BOUNDARY_CONDITIONS_H
#define BOUNDARY_CONDITIONS_H

#include <vector>
#include <string>
#include "../matrix/stiffness_matrix.h"

class BoundaryConditions {
public:
    enum LoadCase {
        UNIFORM_5KN = 1,
        UNIFORM_10KN = 2,
        LINEAR_VARYING = 3,
        CONCENTRATED_15KN = 4,
        ANGLED_10KN = 5
    };
    
    BoundaryConditions(StiffnessMatrix& mesh);
    
    // Apply boundary conditions (fixed left edge)
    void applyFixedBoundary(std::vector<std::vector<double>>& K, 
                           std::vector<double>& F);
    
    // Generate load vector for different cases
    void generateLoadVector(std::vector<double>& F, LoadCase loadCase);
    
    // Get load case description
    static std::string getLoadCaseDescription(LoadCase lc);
    
    // Identify boundary nodes
    void identifyBoundaryNodes();
    
    // Get right edge nodes
    std::vector<int> getRightEdgeNodes() const { return rightEdgeNodes; }
    
    // Get left edge nodes (fixed)
    std::vector<int> getLeftEdgeNodes() const { return leftEdgeNodes; }
    
private:
    StiffnessMatrix& mesh;
    std::vector<int> leftEdgeNodes;
    std::vector<int> rightEdgeNodes;
    double width, height;
};

#endif
