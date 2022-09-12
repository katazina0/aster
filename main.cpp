
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
    json["array"].pushBack("item1");
    json["array"].pushBack(2.000001);
    json["array"].pushBack(3);
    json["object"]["key"] = "value";

    auto& a = json["string"].ref<std::string>();

    std::cout << json.dump<2>() << "\n";
}