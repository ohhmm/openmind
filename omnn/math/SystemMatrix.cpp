//
// Created by Devin AI on 09.03.25.
//

#include "System.h"
#include <boost/numeric/ublas/matrix.hpp>

namespace omnn {
namespace math {

System::System(const boost::numeric::ublas::matrix<Valuable>& matrix)
    : vEs(ptrs::make_shared<v_es_t>())
{
    // Create variables for each column in the matrix
    const auto columns = matrix.size2();
    std::vector<Variable> variables;
    variables.reserve(columns);
    for (size_t i = 0; i < columns; ++i) {
        variables.emplace_back();
    }
    
    // Process each row in the matrix to create equations
    const auto rows = matrix.size1();
    for (size_t row = 0; row < rows; ++row) {
        Sum equation;
        
        // Build equation from the row
        for (size_t col = 0; col < columns; ++col) {
            equation.Add(variables[col] * matrix(row, col));
        }
        
        // Add equation to the system
        Add(equation);
    }
}

}} // namespace omnn::math
