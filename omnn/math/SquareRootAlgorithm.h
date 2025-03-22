//
// Created by Devin AI on 03.17.25.
//
#pragma once
#include <omnn/math/Integer.h>
#include <string>
#include <vector>

namespace omnn::math {

/**
 * Implementation of the square root algorithm as described in
 * https://www.basic-mathematics.com/square-root-algorithm.html
 * 
 * This algorithm calculates square roots using a manual long division-like method
 * without relying on floating-point operations.
 */
class SquareRootAlgorithm {
public:
    /**
     * Calculate the square root of a number using the manual algorithm
     * 
     * @param number The number to calculate the square root of
     * @param precision The number of decimal places to calculate
     * @return The square root as a string with the specified precision
     */
    static std::string calculate(const a_int& number, size_t precision = 6) {
        // Convert number to string for digit manipulation
        std::string numStr = number.str();
        
        // Group digits in pairs from right to left
        std::vector<std::string> pairs;
        for (int i = numStr.length() - 1; i >= 0; i -= 2) {
            if (i == 0) {
                pairs.insert(pairs.begin(), numStr.substr(0, 1));
            } else if (i == 1) {
                pairs.insert(pairs.begin(), numStr.substr(0, 2));
            } else {
                pairs.insert(pairs.begin(), numStr.substr(i - 1, 2));
            }
        }
        
        // Add decimal pairs for precision
        for (size_t i = 0; i < precision; ++i) {
            pairs.push_back("00");
        }
        
        std::string result;
        a_int remainder = 0;
        a_int quotient = 0;
        
        // Process each pair
        for (size_t i = 0; i < pairs.size(); ++i) {
            // Add decimal point after the integer part
            if (i == pairs.size() - precision && precision > 0) {
                result += ".";
            }
            
            // Bring down the next pair
            a_int currentValue = remainder * 100 + a_int(pairs[i]);
            
            // Double the quotient so far
            a_int doubledQuotient = quotient * 2;
            
            // Find the next digit
            int nextDigit = 0;
            for (int d = 9; d >= 0; --d) {
                a_int trial = (doubledQuotient * 10 + d) * d;
                if (trial <= currentValue) {
                    nextDigit = d;
                    remainder = currentValue - trial;
                    break;
                }
            }
            
            // Update the quotient and result
            quotient = quotient * 10 + nextDigit;
            result += std::to_string(nextDigit);
        }
        
        return result;
    }
};

} // namespace omnn::math
