
#include <iostream>

#include <json_lite.hpp>

using json_lite::Json;
using std::cout;
using std::endl;

void parse_example() {

    const Json json = Json::parse(R"(
    {
        "name": "John",
        "age": 30,
        "cars": [
            "Ford",
            "BMW",
            "Fiat"
        ],
    }
    )");

    cout << (const std::string&)json["name"] << endl;
    cout << (Json::Int)json["age"] << endl;
    for (const Json& car : json["cars"].as_array()) {
        cout << (const std::string&)car << endl;
    }
}

void setup() {
    parse_example();
}

void loop() {
}