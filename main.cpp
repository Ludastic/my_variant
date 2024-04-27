#include <iostream>
#include <variant>
#include <string>

int main() {
    std::variant<int, double, std::string> my_variant=5;
    std::cout<<std::get<int>(my_variant);
}
