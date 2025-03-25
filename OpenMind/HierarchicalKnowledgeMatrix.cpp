#include "stdafx.h"
#include "HierarchicalKnowledgeMatrix.h"
#include <algorithm>
#include <cmath>

// HierarchicalKnowledgeMatrix implementation

HierarchicalKnowledgeMatrix::HierarchicalKnowledgeMatrix(
    size_t rows, size_t cols, value_t default_value)
    : rows_(rows), cols_(cols), default_value_(default_value) {
}

HierarchicalKnowledgeMatrix::value_t HierarchicalKnowledgeMatrix::GetValue(
    size_t row, size_t col, const std::vector<position_t>& depth) const {
    
    // Check bounds
    if (row >= rows_ || col >= cols_) {
        return default_value_;
    }
    
    // If depth is empty, get value from this matrix
    if (depth.empty()) {
        position_t pos = {row, col};
        auto it = matrix_.find(pos);
        if (it != matrix_.end()) {
            return it->second;
        }
        return default_value_;
    }
    
    // Otherwise, follow the path through deeper matrices
    position_t current_pos = depth.front();
    auto deeper_it = deeper_matrices_.find(current_pos);
    if (deeper_it == deeper_matrices_.end()) {
        return default_value_;
    }
    
    // Create a new depth vector without the first element
    std::vector<position_t> new_depth(depth.begin() + 1, depth.end());
    return deeper_it->second->GetValue(row, col, new_depth);
}

void HierarchicalKnowledgeMatrix::SetValue(
    size_t row, size_t col, value_t value, const std::vector<position_t>& depth) {
    
    // Check bounds
    if (row >= rows_ || col >= cols_) {
        return;
    }
    
    // If depth is empty, set value in this matrix
    if (depth.empty()) {
        position_t pos = {row, col};
        if (std::abs(value - default_value_) < 1e-10) {
            // If value is close to default, remove it from the sparse matrix
            matrix_.erase(pos);
        } else {
            matrix_[pos] = value;
        }
        return;
    }
    
    // Otherwise, follow the path through deeper matrices
    position_t current_pos = depth.front();
    auto deeper_it = deeper_matrices_.find(current_pos);
    if (deeper_it == deeper_matrices_.end()) {
        // Create a new deeper matrix if it doesn't exist
        position_t pos = current_pos;
        matrix_[pos] = 0.0;  // Mark as pointer to deeper matrix
        deeper_matrices_[pos] = std::make_unique<HierarchicalKnowledgeMatrix>(
            rows_, cols_, default_value_);
        deeper_it = deeper_matrices_.find(pos);
    }
    
    // Create a new depth vector without the first element
    std::vector<position_t> new_depth(depth.begin() + 1, depth.end());
    deeper_it->second->SetValue(row, col, value, new_depth);
}

HierarchicalKnowledgeMatrix* HierarchicalKnowledgeMatrix::AddDeeperMatrix(
    size_t row, size_t col) {
    
    // Check bounds
    if (row >= rows_ || col >= cols_) {
        return nullptr;
    }
    
    position_t pos = {row, col};
    matrix_[pos] = 0.0;  // Mark as pointer to deeper matrix
    
    auto& deeper_matrix = deeper_matrices_[pos];
    if (!deeper_matrix) {
        deeper_matrix = std::make_unique<HierarchicalKnowledgeMatrix>(
            rows_, cols_, default_value_);
    }
    
    return deeper_matrix.get();
}

bool HierarchicalKnowledgeMatrix::HasDeeperMatrix(size_t row, size_t col) const {
    position_t pos = {row, col};
    return deeper_matrices_.find(pos) != deeper_matrices_.end();
}

HierarchicalKnowledgeMatrix* HierarchicalKnowledgeMatrix::GetDeeperMatrix(
    size_t row, size_t col) {
    
    position_t pos = {row, col};
    auto it = deeper_matrices_.find(pos);
    if (it != deeper_matrices_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void HierarchicalKnowledgeMatrix::UpdateProbabilities(
    const std::map<position_t, value_t>& evidence,
    const std::vector<position_t>& depth) {
    
    // If depth is empty, update this matrix
    if (depth.empty()) {
        for (const auto& ev : evidence) {
            position_t pos = ev.first;
            value_t prob = ev.second;
            
            if (HasDeeperMatrix(pos.first, pos.second)) {
                // If this is a pointer to a deeper matrix, update that matrix
                deeper_matrices_[pos]->UpdateProbabilities(evidence);
            } else {
                // Simple Bayesian update
                value_t current_prob = GetValue(pos.first, pos.second);
                
                // P(A|B) = P(B|A) * P(A) / P(B)
                // Here we use a simplified update rule
                value_t updated_prob = (prob * current_prob) / 
                    (prob * current_prob + (1 - prob) * (1 - current_prob));
                
                SetValue(pos.first, pos.second, updated_prob);
            }
        }
        return;
    }
    
    // Otherwise, follow the path through deeper matrices
    position_t current_pos = depth.front();
    auto deeper_it = deeper_matrices_.find(current_pos);
    if (deeper_it != deeper_matrices_.end()) {
        std::vector<position_t> new_depth(depth.begin() + 1, depth.end());
        deeper_it->second->UpdateProbabilities(evidence, new_depth);
    }
}

std::pair<size_t, size_t> HierarchicalKnowledgeMatrix::GetDimensions() const {
    return {rows_, cols_};
}

HierarchicalKnowledgeMatrix::value_t HierarchicalKnowledgeMatrix::GetDefaultValue() const {
    return default_value_;
}

std::vector<std::vector<HierarchicalKnowledgeMatrix::value_t>> 
HierarchicalKnowledgeMatrix::ToDense(bool include_deeper) const {
    std::vector<std::vector<value_t>> dense(rows_, std::vector<value_t>(cols_, default_value_));
    
    // Fill in non-default values
    for (const auto& entry : matrix_) {
        position_t pos = entry.first;
        dense[pos.first][pos.second] = entry.second;
    }
    
    // Mark positions with deeper matrices
    if (include_deeper) {
        for (const auto& entry : deeper_matrices_) {
            position_t pos = entry.first;
            dense[pos.first][pos.second] = -1.0;  // Use -1 to indicate deeper matrix
        }
    }
    
    return dense;
}

// HierarchicalKnowledgeMatrixFacility implementation

HierarchicalKnowledgeMatrixFacility::HierarchicalKnowledgeMatrixFacility(
    std::function<bool()> connector,
    size_t rows, size_t cols, double default_value)
    : FunctorFacility(connector) {
    
    matrix_ = std::make_shared<HierarchicalKnowledgeMatrix>(rows, cols, default_value);
}

double HierarchicalKnowledgeMatrixFacility::GetValue(
    size_t row, size_t col, const std::vector<position_t>& depth) const {
    
    return matrix_->GetValue(row, col, depth);
}

void HierarchicalKnowledgeMatrixFacility::SetValue(
    size_t row, size_t col, double value, const std::vector<position_t>& depth) {
    
    matrix_->SetValue(row, col, value, depth);
}

void HierarchicalKnowledgeMatrixFacility::AddDeeperMatrix(size_t row, size_t col) {
    matrix_->AddDeeperMatrix(row, col);
}

void HierarchicalKnowledgeMatrixFacility::UpdateProbabilities(
    const std::map<position_t, double>& evidence, const std::vector<position_t>& depth) {
    
    matrix_->UpdateProbabilities(evidence, depth);
}

std::vector<HierarchicalKnowledgeMatrixFacility::position_t>
HierarchicalKnowledgeMatrixFacility::GetLowProbabilityCells(
    double threshold, double confidence_threshold, position_t confidence_position) const {
    
    std::vector<position_t> low_prob_cells;
    auto dims = matrix_->GetDimensions();
    
    // Convert to dense for easier processing
    auto dense = matrix_->ToDense();
    
    // Find cells with probability below threshold
    for (size_t row = 0; row < dims.first; ++row) {
        for (size_t col = 0; col < dims.second; ++col) {
            double prob = dense[row][col];
            
            if (prob <= threshold) {
                // Check confidence if there's a deeper matrix
                if (matrix_->HasDeeperMatrix(row, col)) {
                    auto deeper = matrix_->GetDeeperMatrix(row, col);
                    double confidence = deeper->GetValue(
                        confidence_position.first, confidence_position.second);
                    
                    if (confidence >= confidence_threshold) {
                        low_prob_cells.push_back({row, col});
                    }
                } else {
                    // No confidence information, use probability only
                    low_prob_cells.push_back({row, col});
                }
            }
        }
    }
    
    return low_prob_cells;
}

std::vector<HierarchicalKnowledgeMatrixFacility::position_t>
HierarchicalKnowledgeMatrixFacility::GetHighProbabilityCells(
    double threshold, double confidence_threshold, position_t confidence_position) const {
    
    std::vector<position_t> high_prob_cells;
    auto dims = matrix_->GetDimensions();
    
    // Convert to dense for easier processing
    auto dense = matrix_->ToDense();
    
    // Find cells with probability above threshold
    for (size_t row = 0; row < dims.first; ++row) {
        for (size_t col = 0; col < dims.second; ++col) {
            double prob = dense[row][col];
            
            if (prob >= threshold) {
                // Check confidence if there's a deeper matrix
                if (matrix_->HasDeeperMatrix(row, col)) {
                    auto deeper = matrix_->GetDeeperMatrix(row, col);
                    double confidence = deeper->GetValue(
                        confidence_position.first, confidence_position.second);
                    
                    if (confidence >= confidence_threshold) {
                        high_prob_cells.push_back({row, col});
                    }
                } else {
                    // No confidence information, use probability only
                    high_prob_cells.push_back({row, col});
                }
            }
        }
    }
    
    return high_prob_cells;
}

HierarchicalKnowledgeMatrixFacility::matrix_ptr_t
HierarchicalKnowledgeMatrixFacility::GetMatrix() const {
    return matrix_;
}
