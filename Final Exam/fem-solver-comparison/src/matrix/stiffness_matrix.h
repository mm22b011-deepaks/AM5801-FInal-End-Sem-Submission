#ifndef STIFFNESS_MATRIX_H
#define STIFFNESS_MATRIX_H

#include <vector>
#include <cmath>

class StiffnessMatrix {
public:
    // Material properties
    struct MaterialProperties {
        double E;  // Young's modulus (Pa)
        double nu; // Poisson's ratio
        double t;  // Thickness (m)
    };

    // Node coordinates
    struct Node {
        double x, y;
        int id;
    };

    // Element definition (4-node quadrilateral)
    struct Element {
        int nodes[4];  // Node IDs (counter-clockwise)
        int id;
    };

    StiffnessMatrix(int nx, int ny, double width, double height, MaterialProperties mat);
    
    // Generate mesh
    void generateMesh();
    
    // Assemble global stiffness matrix
    void assembleGlobalStiffness();
    
    // Get element stiffness matrix (8x8 for 4-node quad)
    std::vector<std::vector<double>> getElementStiffness(int elemId);
    
    // Get constitutive matrix [D] for plane stress
    std::vector<std::vector<double>> getConstitutiveMatrix();
    
    // Shape functions and derivatives
    void shapeFunction(double xi, double eta, double N[4]);
    void shapeFunctionDerivatives(double xi, double eta, double dN_dxi[4], double dN_deta[4]);
    
    // Gauss quadrature integration
    void gaussQuadrature2D(std::vector<std::vector<double>>& Ke, int elemId);
    
    // Getters
    int getNumNodes() const { return nodes.size(); }
    int getNumElements() const { return elements.size(); }
    int getNumDOF() const { return 2 * nodes.size(); }
    std::vector<std::vector<double>>& getGlobalK() { return K_global; }
    const std::vector<Node>& getNodes() const { return nodes; }
    const std::vector<Element>& getElements() const { return elements; }
    
    // Compute B matrix (strain-displacement)
    void computeBMatrix(double xi, double eta, int elemId, std::vector<std::vector<double>>& B);
    
private:
    int nx, ny;  // Number of elements in x and y
    double width, height;
    MaterialProperties material;
    
    std::vector<Node> nodes;
    std::vector<Element> elements;
    std::vector<std::vector<double>> K_global;
    
    // Jacobian matrix
    void computeJacobian(double xi, double eta, int elemId, 
                        std::vector<std::vector<double>>& J);
};

#endif
