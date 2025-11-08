#include "stiffness_matrix.h"
#include <iostream>
#include <iomanip>

StiffnessMatrix::StiffnessMatrix(int nx, int ny, double width, double height, MaterialProperties mat)
    : nx(nx), ny(ny), width(width), height(height), material(mat) {
    
    std::cout << "\n=== FEM MESH GENERATION ===" << std::endl;
    std::cout << "Plate dimensions: " << width << " m x " << height << " m" << std::endl;
    std::cout << "Mesh: " << nx << " x " << ny << " elements" << std::endl;
    std::cout << "Material Properties:" << std::endl;
    std::cout << "  E = " << material.E / 1e9 << " GPa" << std::endl;
    std::cout << "  ν = " << material.nu << std::endl;
    std::cout << "  t = " << material.t * 1000 << " mm" << std::endl;
    
    generateMesh();
}

void StiffnessMatrix::generateMesh() {
    // Generate nodes
    double dx = width / nx;
    double dy = height / ny;
    
    int nodeId = 0;
    for (int j = 0; j <= ny; j++) {
        for (int i = 0; i <= nx; i++) {
            Node node;
            node.x = i * dx;
            node.y = j * dy;
            node.id = nodeId++;
            nodes.push_back(node);
        }
    }
    
    // Generate elements
    int elemId = 0;
    for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            Element elem;
            elem.id = elemId++;
            // Counter-clockwise node ordering
            elem.nodes[0] = j * (nx + 1) + i;
            elem.nodes[1] = j * (nx + 1) + i + 1;
            elem.nodes[2] = (j + 1) * (nx + 1) + i + 1;
            elem.nodes[3] = (j + 1) * (nx + 1) + i;
            elements.push_back(elem);
        }
    }
    
    std::cout << "Mesh generated: " << nodes.size() << " nodes, " 
              << elements.size() << " elements" << std::endl;
    std::cout << "Total DOF: " << 2 * nodes.size() << std::endl;
}

std::vector<std::vector<double>> StiffnessMatrix::getConstitutiveMatrix() {
    // Plane stress constitutive matrix [D]
    double factor = material.E / (1.0 - material.nu * material.nu);
    
    std::vector<std::vector<double>> D(3, std::vector<double>(3, 0.0));
    D[0][0] = factor;
    D[0][1] = factor * material.nu;
    D[1][0] = factor * material.nu;
    D[1][1] = factor;
    D[2][2] = factor * (1.0 - material.nu) / 2.0;
    
    return D;
}

void StiffnessMatrix::shapeFunction(double xi, double eta, double N[4]) {
    // Shape functions for 4-node quad in natural coordinates
    N[0] = 0.25 * (1.0 - xi) * (1.0 - eta);
    N[1] = 0.25 * (1.0 + xi) * (1.0 - eta);
    N[2] = 0.25 * (1.0 + xi) * (1.0 + eta);
    N[3] = 0.25 * (1.0 - xi) * (1.0 + eta);
}

void StiffnessMatrix::shapeFunctionDerivatives(double xi, double eta, 
                                               double dN_dxi[4], double dN_deta[4]) {
    // Derivatives with respect to xi
    dN_dxi[0] = -0.25 * (1.0 - eta);
    dN_dxi[1] =  0.25 * (1.0 - eta);
    dN_dxi[2] =  0.25 * (1.0 + eta);
    dN_dxi[3] = -0.25 * (1.0 + eta);
    
    // Derivatives with respect to eta
    dN_deta[0] = -0.25 * (1.0 - xi);
    dN_deta[1] = -0.25 * (1.0 + xi);
    dN_deta[2] =  0.25 * (1.0 + xi);
    dN_deta[3] =  0.25 * (1.0 - xi);
}

void StiffnessMatrix::computeJacobian(double xi, double eta, int elemId, 
                                      std::vector<std::vector<double>>& J) {
    J.resize(2, std::vector<double>(2, 0.0));
    
    double dN_dxi[4], dN_deta[4];
    shapeFunctionDerivatives(xi, eta, dN_dxi, dN_deta);
    
    const Element& elem = elements[elemId];
    
    for (int i = 0; i < 4; i++) {
        const Node& node = nodes[elem.nodes[i]];
        J[0][0] += dN_dxi[i] * node.x;
        J[0][1] += dN_dxi[i] * node.y;
        J[1][0] += dN_deta[i] * node.x;
        J[1][1] += dN_deta[i] * node.y;
    }
}

void StiffnessMatrix::computeBMatrix(double xi, double eta, int elemId, 
                                     std::vector<std::vector<double>>& B) {
    B.resize(3, std::vector<double>(8, 0.0));
    
    double dN_dxi[4], dN_deta[4];
    shapeFunctionDerivatives(xi, eta, dN_dxi, dN_deta);
    
    // Compute Jacobian
    std::vector<std::vector<double>> J;
    computeJacobian(xi, eta, elemId, J);
    
    // Compute inverse Jacobian
    double detJ = J[0][0] * J[1][1] - J[0][1] * J[1][0];
    std::vector<std::vector<double>> invJ(2, std::vector<double>(2));
    invJ[0][0] =  J[1][1] / detJ;
    invJ[0][1] = -J[0][1] / detJ;
    invJ[1][0] = -J[1][0] / detJ;
    invJ[1][1] =  J[0][0] / detJ;
    
    // Compute dN/dx and dN/dy
    std::vector<double> dN_dx(4), dN_dy(4);
    for (int i = 0; i < 4; i++) {
        dN_dx[i] = invJ[0][0] * dN_dxi[i] + invJ[0][1] * dN_deta[i];
        dN_dy[i] = invJ[1][0] * dN_dxi[i] + invJ[1][1] * dN_deta[i];
    }
    
    // Assemble B matrix
    for (int i = 0; i < 4; i++) {
        B[0][2*i]     = dN_dx[i];
        B[0][2*i + 1] = 0.0;
        B[1][2*i]     = 0.0;
        B[1][2*i + 1] = dN_dy[i];
        B[2][2*i]     = dN_dy[i];
        B[2][2*i + 1] = dN_dx[i];
    }
}

void StiffnessMatrix::gaussQuadrature2D(std::vector<std::vector<double>>& Ke, int elemId) {
    // 2x2 Gauss quadrature
    double gp = 1.0 / sqrt(3.0);
    std::vector<double> gauss_points = {-gp, gp};
    std::vector<double> weights = {1.0, 1.0};
    
    auto D = getConstitutiveMatrix();
    
    // Initialize element stiffness matrix
    Ke.assign(8, std::vector<double>(8, 0.0));
    
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            double xi = gauss_points[i];
            double eta = gauss_points[j];
            double w = weights[i] * weights[j];
            
            // Compute B matrix
            std::vector<std::vector<double>> B;
            computeBMatrix(xi, eta, elemId, B);
            
            // Compute Jacobian determinant
            std::vector<std::vector<double>> J;
            computeJacobian(xi, eta, elemId, J);
            double detJ = J[0][0] * J[1][1] - J[0][1] * J[1][0];
            
            // Ke += B^T * D * B * detJ * w * t
            for (int m = 0; m < 8; m++) {
                for (int n = 0; n < 8; n++) {
                    double sum = 0.0;
                    for (int p = 0; p < 3; p++) {
                        for (int q = 0; q < 3; q++) {
                            sum += B[p][m] * D[p][q] * B[q][n];
                        }
                    }
                    Ke[m][n] += sum * detJ * w * material.t;
                }
            }
        }
    }
}

std::vector<std::vector<double>> StiffnessMatrix::getElementStiffness(int elemId) {
    std::vector<std::vector<double>> Ke;
    gaussQuadrature2D(Ke, elemId);
    return Ke;
}

void StiffnessMatrix::assembleGlobalStiffness() {
    std::cout << "\n=== ASSEMBLING GLOBAL STIFFNESS MATRIX ===" << std::endl;
    
    int ndof = 2 * nodes.size();
    K_global.assign(ndof, std::vector<double>(ndof, 0.0));
    
    int progress = 0;
    int total = elements.size();
    
    for (int e = 0; e < elements.size(); e++) {
        // Progress indicator
        int percent = (100 * e) / total;
        if (percent > progress) {
            progress = percent;
            std::cout << "\rProgress: " << progress << "% " << std::flush;
        }
        
        auto Ke = getElementStiffness(e);
        const Element& elem = elements[e];
        
        // Assemble into global matrix
        for (int i = 0; i < 4; i++) {
            int node_i = elem.nodes[i];
            for (int j = 0; j < 4; j++) {
                int node_j = elem.nodes[j];
                
                // u-u coupling
                K_global[2*node_i][2*node_j] += Ke[2*i][2*j];
                // u-v coupling
                K_global[2*node_i][2*node_j + 1] += Ke[2*i][2*j + 1];
                // v-u coupling
                K_global[2*node_i + 1][2*node_j] += Ke[2*i + 1][2*j];
                // v-v coupling
                K_global[2*node_i + 1][2*node_j + 1] += Ke[2*i + 1][2*j + 1];
            }
        }
    }
    
    std::cout << "\rProgress: 100%" << std::endl;
    std::cout << "Global stiffness matrix assembled: " << ndof << " x " << ndof << std::endl;
}
