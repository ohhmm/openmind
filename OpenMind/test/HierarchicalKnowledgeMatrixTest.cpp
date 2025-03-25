#include "stdafx.h"
#include "../HierarchicalKnowledgeMatrix.h"
#include <omnn/math/Valuable.h>
#include <omnn/math/Variable.h>
#include <gtest/gtest.h>

// Test fixture for HierarchicalKnowledgeMatrix
class HierarchicalKnowledgeMatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a basic matrix for testing
        matrix_ = std::make_unique<HierarchicalKnowledgeMatrix>(10, 10, 0.5);
        
        // Set up some test values
        matrix_->SetValue(5, 5, 0.8);
        matrix_->SetValue(3, 7, 0.2);
        
        // Create a deeper matrix at position (4, 4)
        auto deeper = matrix_->AddDeeperMatrix(4, 4);
        deeper->SetValue(1, 1, 0.9);
        
        // Create a facility for testing
        auto connector = []() -> bool {
            // Simple connector function
            return true;
        };
        
        facility_ = std::make_unique<HierarchicalKnowledgeMatrixFacility>(connector, 10, 10, 0.5);
        facility_->SetValue(5, 5, 0.8);
        facility_->AddDeeperMatrix(4, 4);
    }
    
    std::unique_ptr<HierarchicalKnowledgeMatrix> matrix_;
    std::unique_ptr<HierarchicalKnowledgeMatrixFacility> facility_;
};

// Test that the HierarchicalKnowledgeMatrix can store and retrieve values
TEST_F(HierarchicalKnowledgeMatrixTest, StoreAndRetrieveValues) {
    // Check values we set in SetUp
    EXPECT_DOUBLE_EQ(0.8, matrix_->GetValue(5, 5));
    EXPECT_DOUBLE_EQ(0.2, matrix_->GetValue(3, 7));
    
    // Check default value
    EXPECT_DOUBLE_EQ(0.5, matrix_->GetValue(0, 0));
    
    // Set a new value and retrieve it
    matrix_->SetValue(1, 1, 0.7);
    EXPECT_DOUBLE_EQ(0.7, matrix_->GetValue(1, 1));
}

// Test that the HierarchicalKnowledgeMatrix can handle deeper matrices
TEST_F(HierarchicalKnowledgeMatrixTest, DeeperMatrices) {
    // Check that we have a deeper matrix at (4, 4)
    EXPECT_TRUE(matrix_->HasDeeperMatrix(4, 4));
    
    // Check value in deeper matrix
    std::vector<std::pair<size_t, size_t>> depth = {{4, 4}};
    EXPECT_DOUBLE_EQ(0.9, matrix_->GetValue(1, 1, depth));
    
    // Set a new value in the deeper matrix
    matrix_->SetValue(2, 2, 0.6, depth);
    EXPECT_DOUBLE_EQ(0.6, matrix_->GetValue(2, 2, depth));
}

// Test that the HierarchicalKnowledgeMatrixFacility works correctly
TEST_F(HierarchicalKnowledgeMatrixTest, FacilityWorks) {
    // Check the value we set in SetUp
    EXPECT_DOUBLE_EQ(0.8, facility_->GetValue(5, 5));
    
    // Set a new value and retrieve it
    facility_->SetValue(1, 1, 0.7);
    EXPECT_DOUBLE_EQ(0.7, facility_->GetValue(1, 1));
    
    // Check that we have a deeper matrix at (4, 4)
    EXPECT_TRUE(facility_->GetMatrix()->HasDeeperMatrix(4, 4));
    
    // Invoke the facility (should call the connector function)
    EXPECT_TRUE(facility_->Invoke());
}

// Test the probability update functionality
TEST_F(HierarchicalKnowledgeMatrixTest, ProbabilityUpdates) {
    // Set up evidence for a Bayesian update
    std::map<std::pair<size_t, size_t>, double> evidence;
    evidence[{5, 5}] = 0.9;  // Strong evidence
    
    // Current probability is 0.8
    EXPECT_DOUBLE_EQ(0.8, matrix_->GetValue(5, 5));
    
    // Update probabilities
    matrix_->UpdateProbabilities(evidence);
    
    // After update, probability should be higher (exact value depends on the Bayesian formula)
    EXPECT_GT(matrix_->GetValue(5, 5), 0.8);
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
