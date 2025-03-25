#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "Facility.h"
#include "FunctorFacility.h"

namespace omnn {
namespace math {
class Valuable;
class Variable;
}
}

/**
 * @brief HierarchicalKnowledgeMatrix provides a multi-dimensional knowledge representation
 * 
 * This class implements a hierarchical sparse matrix structure where zero values
 * act as pointers to deeper probability matrices, creating a multi-dimensional
 * knowledge representation system.
 */
class HierarchicalKnowledgeMatrix {
public:
    using value_t = double;
    using position_t = std::pair<size_t, size_t>;
    using matrix_t = std::map<position_t, value_t>;
    
    /**
     * @brief Initialize a hierarchical knowledge matrix
     * 
     * @param rows Number of rows in the matrix
     * @param cols Number of columns in the matrix
     * @param default_value Default value for cells (typically a probability)
     */
    HierarchicalKnowledgeMatrix(size_t rows, size_t cols, value_t default_value = 0.5);
    
    /**
     * @brief Get the value at the specified position and depth
     * 
     * @param row Row index
     * @param col Column index
     * @param depth Vector of positions representing the path to follow through deeper matrices
     * @return Value at the specified position
     */
    value_t GetValue(size_t row, size_t col, 
                    const std::vector<position_t>& depth = std::vector<position_t>()) const;
    
    /**
     * @brief Set the value at the specified position and depth
     * 
     * @param row Row index
     * @param col Column index
     * @param value Value to set
     * @param depth Vector of positions representing the path to follow through deeper matrices
     */
    void SetValue(size_t row, size_t col, value_t value,
                 const std::vector<position_t>& depth = std::vector<position_t>());
    
    /**
     * @brief Add a deeper matrix at the specified position
     * 
     * @param row Row index
     * @param col Column index
     * @return Pointer to the newly created deeper matrix
     */
    HierarchicalKnowledgeMatrix* AddDeeperMatrix(size_t row, size_t col);
    
    /**
     * @brief Check if a position has a deeper matrix
     * 
     * @param row Row index
     * @param col Column index
     * @return True if the position has a deeper matrix, False otherwise
     */
    bool HasDeeperMatrix(size_t row, size_t col) const;
    
    /**
     * @brief Get the deeper matrix at the specified position
     * 
     * @param row Row index
     * @param col Column index
     * @return Pointer to the deeper matrix if it exists, nullptr otherwise
     */
    HierarchicalKnowledgeMatrix* GetDeeperMatrix(size_t row, size_t col);
    
    /**
     * @brief Update probabilities based on new evidence using Bayesian update
     * 
     * @param evidence Map of positions to observed probabilities
     * @param depth Vector of positions representing the path to the matrix to update
     */
    void UpdateProbabilities(const std::map<position_t, value_t>& evidence,
                            const std::vector<position_t>& depth = std::vector<position_t>());
    
    /**
     * @brief Get the dimensions of the matrix
     * 
     * @return Pair of (rows, cols)
     */
    std::pair<size_t, size_t> GetDimensions() const;
    
    /**
     * @brief Get the default value for this matrix
     * 
     * @return Default value
     */
    value_t GetDefaultValue() const;
    
    /**
     * @brief Convert the sparse matrix to a dense representation
     * 
     * @param include_deeper If true, mark positions with deeper matrices
     * @return Vector of vectors representing the dense matrix
     */
    std::vector<std::vector<value_t>> ToDense(bool include_deeper = false) const;
    
private:
    size_t rows_;
    size_t cols_;
    value_t default_value_;
    
    // Sparse matrix representation
    matrix_t matrix_;
    
    // Map of deeper matrices
    // Key: (row, col) position in this matrix
    // Value: Pointer to the deeper matrix
    std::map<position_t, std::unique_ptr<HierarchicalKnowledgeMatrix>> deeper_matrices_;
};

/**
 * @brief HierarchicalKnowledgeMatrixFacility provides integration with OpenMind
 * 
 * This facility allows OpenMind to use hierarchical knowledge matrices for
 * representing and reasoning with probabilistic knowledge.
 */
class HierarchicalKnowledgeMatrixFacility : public FunctorFacility {
public:
    using matrix_ptr_t = std::shared_ptr<HierarchicalKnowledgeMatrix>;
    using position_t = std::pair<size_t, size_t>;
    
    /**
     * @brief Constructor for HierarchicalKnowledgeMatrixFacility
     * 
     * @param rows Number of rows in the matrix
     * @param cols Number of columns in the matrix
     * @param default_value Default value for cells
     */
    HierarchicalKnowledgeMatrixFacility(
        std::function<bool()> connector,
        size_t rows, size_t cols, 
        double default_value = 0.5);
    
    /**
     * @brief Get the value at the specified position and depth
     * 
     * @param row Row index
     * @param col Column index
     * @param depth Vector of positions representing the path to follow through deeper matrices
     * @return Value at the specified position
     */
    double GetValue(size_t row, size_t col, 
                   const std::vector<position_t>& depth = std::vector<position_t>()) const;
    
    /**
     * @brief Set the value at the specified position and depth
     * 
     * @param row Row index
     * @param col Column index
     * @param value Value to set
     * @param depth Vector of positions representing the path to follow through deeper matrices
     */
    void SetValue(size_t row, size_t col, double value,
                 const std::vector<position_t>& depth = std::vector<position_t>());
    
    /**
     * @brief Add a deeper matrix at the specified position
     * 
     * @param row Row index
     * @param col Column index
     */
    void AddDeeperMatrix(size_t row, size_t col);
    
    /**
     * @brief Update probabilities based on new evidence using Bayesian update
     * 
     * @param evidence Map of positions to observed probabilities
     * @param depth Vector of positions representing the path to the matrix to update
     */
    void UpdateProbabilities(const std::map<position_t, double>& evidence,
                            const std::vector<position_t>& depth = std::vector<position_t>());
    
    /**
     * @brief Get cells with probability below threshold and confidence above threshold
     * 
     * @param threshold Probability threshold for safe cells
     * @param confidence_threshold Confidence threshold for safe cells
     * @param confidence_position Position in deeper matrices that stores confidence values
     * @return Vector of positions of safe cells
     */
    std::vector<position_t> GetLowProbabilityCells(
        double threshold = 0.1, 
        double confidence_threshold = 0.7,
        position_t confidence_position = {0, 0}) const;
    
    /**
     * @brief Get cells with probability above threshold and confidence above threshold
     * 
     * @param threshold Probability threshold for high probability cells
     * @param confidence_threshold Confidence threshold for high probability cells
     * @param confidence_position Position in deeper matrices that stores confidence values
     * @return Vector of positions of high probability cells
     */
    std::vector<position_t> GetHighProbabilityCells(
        double threshold = 0.9, 
        double confidence_threshold = 0.7,
        position_t confidence_position = {0, 0}) const;
    
    /**
     * @brief Get the underlying knowledge matrix
     * 
     * @return Shared pointer to the knowledge matrix
     */
    matrix_ptr_t GetMatrix() const;
    

    
private:
    matrix_ptr_t matrix_;
};
