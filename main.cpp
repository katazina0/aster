
#include <iostream>
#include <aster/json.hpp>

using namespace aster;

int main()
{
    JSON json;
    json["null"] = nullptr;
    json["bool"] = true;
    json["int"] = 123;
    json["double"] = 456.789;
    json["string"] = "utf8 😋";
    json["array"].push_back("item1");
    json["array"].push_back(2.000001);
    json["array"].push_back(3);
    json["object"]["key"] = "value";

    std::cout << json.dump<2>() << "\n";
}