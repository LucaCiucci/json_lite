
#include <Arduino.h>
#include <iostream>

#include <json_lite.hpp>

using json_lite::Json;
using std::cout;
using std::endl;

void print_example() {

    Json json;

    json["name"] = "John";
    json["age"] = 30;
    json["cars"][0] = "Ford";
    json["cars"][1] = "BMW";
    json["cars"][2] = "Fiat";

    // different ways to print the same thing
    cout << json << endl;
    cout << json.dump() << endl;
    Serial.println(json.dump().c_str());
}

void setup() {
    print_example();
}

void loop() {
}