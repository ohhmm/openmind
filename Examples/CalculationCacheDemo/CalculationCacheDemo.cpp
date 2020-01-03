//
// Created by Sergej Krivonos on 02.01.2020.
//

#include "Variable.h"


int main()
{
  using namespace omnn::math;
  Variable x;
  Valuable i;
  Valuable equation = 1;
  auto start_time = std::chrono::high_resolution_clock::now();
  auto current_time = std::chrono::high_resolution_clock::now();
  do{
    equation.logic_or(x.Equals(++i));
    std::cout
      << "solving " << equation << std::endl;
    for(auto& s : equation.Solutions()){
      std::cout << s << ' ';
    }
    std::cout << std::endl;
    current_time = std::chrono::high_resolution_clock::now();
  }while(std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() < 60 );
  return 0;
}
