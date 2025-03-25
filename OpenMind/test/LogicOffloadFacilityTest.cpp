#include "stdafx.h"
#include "../LogicOffloadFacility.h"
#include <omnn/math/Valuable.h>
#include <omnn/math/Variable.h>
#include <gtest/gtest.h>

// Example of a 3rd-party logic system connector
class MinesweeperLogicSystem {
public:
    // Represents a cell in the Minesweeper grid
    struct Cell {
        bool is_mine = false;
        bool is_revealed = false;
        bool is_flagged = false;
        int adjacent_mines = 0;
    };
    
    // Represents the Minesweeper grid
    class Grid {
    public:
        Grid(int width, int height) : width_(width), height_(height) {
            cells_.resize(height_, std::vector<Cell>(width_));
        }
        
        void SetCell(int x, int y, bool is_mine, bool is_revealed, int adjacent_mines = 0) {
            if (x >= 0 && x < width_ && y >= 0 && y < height_) {
                cells_[y][x].is_mine = is_mine;
                cells_[y][x].is_revealed = is_revealed;
                cells_[y][x].adjacent_mines = adjacent_mines;
            }
        }
        
        Cell& GetCell(int x, int y) {
            return cells_[y][x];
        }
        
        const Cell& GetCell(int x, int y) const {
            return cells_[y][x];
        }
        
        int GetWidth() const { return width_; }
        int GetHeight() const { return height_; }
        
    private:
        int width_;
        int height_;
        std::vector<std::vector<Cell>> cells_;
    };
    
    // Logical solver for Minesweeper
    class Solver {
    public:
        Solver(Grid& grid) : grid_(grid) {}
        
        // Find safe moves using logical deduction
        std::vector<std::pair<int, int>> GetSafeMoves() {
            std::vector<std::pair<int, int>> safe_moves;
            
            // Implement logical deduction
            for (int y = 0; y < grid_.GetHeight(); ++y) {
                for (int x = 0; x < grid_.GetWidth(); ++x) {
                    const Cell& cell = grid_.GetCell(x, y);
                    
                    // Skip cells that aren't revealed or have no adjacent mines
                    if (!cell.is_revealed || cell.adjacent_mines == 0) {
                        continue;
                    }
                    
                    // Count flagged and unrevealed cells around this cell
                    int flagged = 0;
                    std::vector<std::pair<int, int>> unrevealed;
                    
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0) {
                                continue;
                            }
                            
                            int nx = x + dx;
                            int ny = y + dy;
                            if (nx >= 0 && nx < grid_.GetWidth() && ny >= 0 && ny < grid_.GetHeight()) {
                                const Cell& neighbor = grid_.GetCell(nx, ny);
                                if (neighbor.is_flagged) {
                                    flagged++;
                                } else if (!neighbor.is_revealed) {
                                    unrevealed.emplace_back(nx, ny);
                                }
                            }
                        }
                    }
                    
                    // If the number of flags equals the number of adjacent mines,
                    // all remaining unrevealed cells are safe
                    if (flagged == cell.adjacent_mines && !unrevealed.empty()) {
                        safe_moves.insert(safe_moves.end(), unrevealed.begin(), unrevealed.end());
                    }
                }
            }
            
            return safe_moves;
        }
        
        // Find mine locations using logical deduction
        std::vector<std::pair<int, int>> GetMineLocations() {
            std::vector<std::pair<int, int>> mine_locations;
            
            // Implement logical deduction
            for (int y = 0; y < grid_.GetHeight(); ++y) {
                for (int x = 0; x < grid_.GetWidth(); ++x) {
                    const Cell& cell = grid_.GetCell(x, y);
                    
                    // Skip cells that aren't revealed
                    if (!cell.is_revealed) {
                        continue;
                    }
                    
                    // Count flagged and unrevealed cells around this cell
                    int flagged = 0;
                    std::vector<std::pair<int, int>> unrevealed;
                    
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            if (dx == 0 && dy == 0) {
                                continue;
                            }
                            
                            int nx = x + dx;
                            int ny = y + dy;
                            if (nx >= 0 && nx < grid_.GetWidth() && ny >= 0 && ny < grid_.GetHeight()) {
                                const Cell& neighbor = grid_.GetCell(nx, ny);
                                if (neighbor.is_flagged) {
                                    flagged++;
                                } else if (!neighbor.is_revealed) {
                                    unrevealed.emplace_back(nx, ny);
                                }
                            }
                        }
                    }
                    
                    // If the number of unrevealed cells equals the number of adjacent mines minus flags,
                    // all unrevealed cells must be mines
                    if (!unrevealed.empty() && unrevealed.size() == cell.adjacent_mines - flagged) {
                        mine_locations.insert(mine_locations.end(), unrevealed.begin(), unrevealed.end());
                    }
                }
            }
            
            return mine_locations;
        }
        
    private:
        Grid& grid_;
    };
};

// Test fixture for LogicOffloadFacility
class LogicOffloadFacilityTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up a Minesweeper grid for testing
        grid_ = std::make_unique<MinesweeperLogicSystem::Grid>(10, 10);
        
        // Set up some revealed cells with adjacent mine counts
        grid_->SetCell(5, 5, false, true, 1);  // Center cell with 1 adjacent mine
        grid_->SetCell(4, 5, false, false);    // Unrevealed cell
        grid_->SetCell(6, 5, false, false);    // Unrevealed cell
        grid_->SetCell(5, 4, false, false);    // Unrevealed cell
        grid_->SetCell(5, 6, false, true, 0);  // Revealed cell with 0 adjacent mines
        
        // Create a solver
        solver_ = std::make_unique<MinesweeperLogicSystem::Solver>(*grid_);
        
        // Create a LogicOffloadFacility that connects to the Minesweeper solver
        auto connector = [this]() -> bool {
            // Connect to the Minesweeper solver and get results
            auto safe_moves = solver_->GetSafeMoves();
            auto mine_locations = solver_->GetMineLocations();
            
            // Convert results to propositions
            std::vector<omnn::math::Valuable> results;
            
            // Create variables for each cell
            for (const auto& move : safe_moves) {
                int x = move.first;
                int y = move.second;
                // Create a proposition that this cell is safe
                omnn::math::Variable cell_var("cell_" + std::to_string(x) + "_" + std::to_string(y));
                results.push_back(cell_var == 0); // 0 means safe (no mine)
            }
            
            for (const auto& mine : mine_locations) {
                int x = mine.first;
                int y = mine.second;
                // Create a proposition that this cell contains a mine
                omnn::math::Variable cell_var("cell_" + std::to_string(x) + "_" + std::to_string(y));
                results.push_back(cell_var == 1); // 1 means mine
            }
            
            // Call the callback with the results
            if (callback_) {
                callback_(results);
            }
            
            return true;
        };
        
        // Create a callback that stores the results
        auto callback = [this](const std::vector<omnn::math::Valuable>& results) {
            results_ = results;
        };
        
        facility_ = std::make_unique<LogicOffloadFacility>(connector, callback);
    }
    
    std::unique_ptr<MinesweeperLogicSystem::Grid> grid_;
    std::unique_ptr<MinesweeperLogicSystem::Solver> solver_;
    std::unique_ptr<LogicOffloadFacility> facility_;
    std::vector<omnn::math::Valuable> results_;
    std::function<void(const std::vector<omnn::math::Valuable>&)> callback_;
};

// Test that the LogicOffloadFacility can connect to the Minesweeper solver
TEST_F(LogicOffloadFacilityTest, ConnectToMinesweeperSolver) {
    // Invoke the facility to connect to the Minesweeper solver
    EXPECT_TRUE(facility_->Invoke());
    
    // Check that we got some results
    EXPECT_FALSE(results_.empty());
}

// Test that the LogicOffloadFacility can add propositions
TEST_F(LogicOffloadFacilityTest, AddPropositions) {
    // Create some propositions
    omnn::math::Variable x("x");
    omnn::math::Variable y("y");
    
    // Add propositions to the facility
    facility_->AddProposition(x > 0);
    facility_->AddProposition(y < 10);
    
    // Add a rule
    facility_->AddRule(x > 0, y < 10);
    
    // Invoke the facility
    EXPECT_TRUE(facility_->Invoke());
}

// Test that the LogicOffloadFacility can check if a proposition is satisfiable
TEST_F(LogicOffloadFacilityTest, IsSatisfiable) {
    // Create some propositions
    omnn::math::Variable x("x");
    
    // Check if x > 0 is satisfiable
    EXPECT_TRUE(facility_->IsSatisfiable(x > 0));
    
    // Check if x > 0 && x < 0 is satisfiable (it's not)
    EXPECT_FALSE(facility_->IsSatisfiable((x > 0) && (x < 0)));
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
