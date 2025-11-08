#include "boundary_conditions.h"
#include <iostream>
#include <cmath>
#include <algorithm>

BoundaryConditions::BoundaryConditions(StiffnessMatrix& mesh) : mesh(mesh) {
    identifyBoundaryNodes();
    
    // Get plate dimensions from first and last nodes
    const auto& nodes = mesh.getNodes();
    width = nodes.back().x;
    height = nodes.back().y;
}

void BoundaryConditions::identifyBoundaryNodes() {
    const auto& nodes = mesh.getNodes();
    
    leftEdgeNodes.clear();
    rightEdgeNodes.clear();
    
    double tolerance = 1e-9;
    double maxX = 0.0;
    
    // Find max x coordinate
    for (const auto& node : nodes) {
        if (node.x > maxX) maxX = node.x;
    }
    
    // Identify left and right edge nodes
    for (const auto& node : nodes) {
        if (std::abs(node.x) < tolerance) {
            leftEdgeNodes.push_back(node.id);
        }
        if (std::abs(node.x - maxX) < tolerance) {
            rightEdgeNodes.push_back(node.id);
        }
    }
    
    // Sort by y-coordinate
    const auto& nodeList = nodes;
    std::sort(rightEdgeNodes.begin(), rightEdgeNodes.end(), 
              [&nodeList](int a, int b) { return nodeList[a].y < nodeList[b].y; });
    
    std::cout << "\n=== BOUNDARY CONDITIONS ===" << std::endl;
    std::cout << "Left edge (fixed): " << leftEdgeNodes.size() << " nodes" << std::endl;
    std::cout << "Right edge (loaded): " << rightEdgeNodes.size() << " nodes" << std::endl;
}

void BoundaryConditions::applyFixedBoundary(std::vector<std::vector<double>>& K, 
                                            std::vector<double>& F) {
    // Fix left edge (u = 0, v = 0)
    for (int nodeId : leftEdgeNodes) {
        int dof_u = 2 * nodeId;
        int dof_v = 2 * nodeId + 1;
        
        // Set row to zero with 1 on diagonal
        for (size_t j = 0; j < K[dof_u].size(); j++) {
            K[dof_u][j] = 0.0;
            K[dof_v][j] = 0.0;
        }
        K[dof_u][dof_u] = 1.0;
        K[dof_v][dof_v] = 1.0;
        
        // Set column to zero
        for (size_t i = 0; i < K.size(); i++) {
            if (i != dof_u) K[i][dof_u] = 0.0;
            if (i != dof_v) K[i][dof_v] = 0.0;
        }
        
        F[dof_u] = 0.0;
        F[dof_v] = 0.0;
    }
}

void BoundaryConditions::generateLoadVector(std::vector<double>& F, LoadCase loadCase) {
    const auto& nodes = mesh.getNodes();
    int ndof = 2 * nodes.size();
    F.assign(ndof, 0.0);
    
    std::cout << "\n=== LOAD CASE: " << getLoadCaseDescription(loadCase) << " ===" << std::endl;
    
    int numEdgeNodes = rightEdgeNodes.size();
    double edgeLength = height;
    double segmentLength = edgeLength / (numEdgeNodes - 1);
    
    switch (loadCase) {
        case UNIFORM_5KN: {
            // Uniform 5 kN tensile load on right edge
            double totalLoad = 5000.0;  // 5 kN in Newtons
            double loadPerNode = totalLoad / numEdgeNodes;
            
            // Distribute load uniformly (trapezoidal rule)
            for (size_t i = 0; i < rightEdgeNodes.size(); i++) {
                int nodeId = rightEdgeNodes[i];
                double factor = (i == 0 || i == rightEdgeNodes.size() - 1) ? 0.5 : 1.0;
                F[2 * nodeId] = loadPerNode * factor;  // x-direction (tensile)
            }
            
            std::cout << "Applied uniform tensile load: 5 kN" << std::endl;
            std::cout << "Load per node: " << loadPerNode << " N" << std::endl;
            break;
        }
        
        case UNIFORM_10KN: {
            // Uniform 10 kN tensile load on right edge
            double totalLoad = 10000.0;  // 10 kN
            double loadPerNode = totalLoad / numEdgeNodes;
            
            for (size_t i = 0; i < rightEdgeNodes.size(); i++) {
                int nodeId = rightEdgeNodes[i];
                double factor = (i == 0 || i == rightEdgeNodes.size() - 1) ? 0.5 : 1.0;
                F[2 * nodeId] = loadPerNode * factor;
            }
            
            std::cout << "Applied uniform tensile load: 10 kN" << std::endl;
            std::cout << "Load per node: " << loadPerNode << " N" << std::endl;
            break;
        }
        
        case LINEAR_VARYING: {
            // Linearly varying load from 0 to 10 kN
            double maxLoad = 10000.0;  // 10 kN at top
            
            for (size_t i = 0; i < rightEdgeNodes.size(); i++) {
                int nodeId = rightEdgeNodes[i];
                double y = nodes[nodeId].y;
                double loadAtNode = maxLoad * (y / height) / numEdgeNodes;
                double factor = (i == 0 || i == rightEdgeNodes.size() - 1) ? 0.5 : 1.0;
                F[2 * nodeId] = loadAtNode * factor;
            }
            
            std::cout << "Applied linearly varying load: 0 to 10 kN" << std::endl;
            break;
        }
        
        case CONCENTRATED_15KN: {
            // 15 kN concentrated at midpoint of right edge
            double load = 15000.0;  // 15 kN
            
            // Find node closest to midpoint
            int midNodeIdx = numEdgeNodes / 2;
            int midNodeId = rightEdgeNodes[midNodeIdx];
            
            F[2 * midNodeId] = load;
            
            std::cout << "Applied concentrated load: 15 kN at midpoint" << std::endl;
            std::cout << "Node ID: " << midNodeId << " at y = " 
                      << nodes[midNodeId].y << " m" << std::endl;
            break;
        }
        
        case ANGLED_10KN: {
            // 10 kN at 30° to horizontal at top-right node
            double load = 10000.0;  // 10 kN
            double angle = 30.0 * M_PI / 180.0;  // 30 degrees in radians
            
            int topNodeId = rightEdgeNodes.back();
            
            F[2 * topNodeId] = load * cos(angle);      // x-component
            F[2 * topNodeId + 1] = load * sin(angle);  // y-component
            
            std::cout << "Applied 10 kN load at 30° to horizontal" << std::endl;
            std::cout << "Fx = " << F[2 * topNodeId] << " N" << std::endl;
            std::cout << "Fy = " << F[2 * topNodeId + 1] << " N" << std::endl;
            break;
        }
    }
}

std::string BoundaryConditions::getLoadCaseDescription(LoadCase lc) {
    switch (lc) {
        case UNIFORM_5KN: return "5 kN Uniform Tensile Load";
        case UNIFORM_10KN: return "10 kN Uniform Tensile Load";
        case LINEAR_VARYING: return "Linearly Varying Load (0 to 10 kN)";
        case CONCENTRATED_15KN: return "15 kN Concentrated Load at Midpoint";
        case ANGLED_10KN: return "10 kN Load at 30° (Top-Right Node)";
        default: return "Unknown Load Case";
    }
}
