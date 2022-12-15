#include "any_range.hpp"
#include <vector>
#include <array>
#include <iostream>

void print_container(const any_range<int>);

int main(){
    const std::vector<int> v = {123,4, 0};
    std::array<int, 4> a = {4,3,2,1};
    std::cout << "vector" << std::endl;
    print_container(v);
    std::cout << "array" << std::endl;
    print_container(a);
    std::cout << "rvalue vector" << std::endl;
    print_container(std::vector<int>{0,1,23,4});
}

void print_container(const any_range<int> int_range){
    for(const auto& val : int_range){
        std::cout << val << ",";
    }
    std::endl(std::cout);
}