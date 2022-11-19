# json_lite
 A JSON library for esp32

This is a simple JSON library I developed for my Esp32 projects. It has no dependencies only uses some c++ STL features such as `vector`, `map` and `string`.

> :warning: Warning:
> Just a proof of concept, but still a [MVP](https://en.wikipedia.org/wiki/Minimum_viable_product), Will improve in the future.

> Note: This library is not complete, there are many improvements that can be made. to the interface and the parsing is still primitive. I will be adding more features as I need them.

## Usage

Once installed, you can use the library by including `<json_lite.hpp>` in your project, then you can do:

```cpp
#include <iostream>
#include <json_lite.hpp>
using json_lite::Json;

void parse_example()
{
    Json json;
    json = Json::parse(R"(
        {
            "name": "John",
            "age": 30,
            "cars": [
                "Ford",
                "BMW",
                "Fiat"
            ]
        }
    )");

    // print json
    std::cout << json.dump() << std::endl;

    const auto& cars = json["cars"].as_array();
    for (const auto& car : cars)
        std::cout << car.as_string() << std::endl;
}

void create_example()
{
    Json json;
    json["name"] = "John";
    json["age"] = 30;
    json["cars"] = Json::array();
    json["cars"].push_back("Ford");
    json["cars"].push_back("BMW");
    json["cars"].push_back("Fiat");

    // print json
    std::cout << json.dump() << std::endl;
}
```

### Initializing `Json`

You can initialize a `Json` object in several ways:

Create a null json object using the default constructor:
```cpp
Json json;
```

Create a json object from a string:
```cpp
Json json = Json::parse(R"(
    {
        "name": "John",
        "age": 30,
        "cars": [
            "Ford",
            "BMW",
            "Fiat"
        ]
    }
)");
```

Create an empty json object:
```cpp
Json json = Json::make_object();
Json json = Json::make_object_t();
Json json; json.to_object();
```

Create an empty json array:
```cpp
Json json = Json::make_array();
Json json = Json::make_array_t();
Json json; json.to_array();
```

Create a json number:
```cpp
Json json = (Json::Float) 3.14;
Json json = (Json::Int) 42;
```
> :warning: Note:  
> integers and floats are both considered as `Json::Type::number`, but the actual data types are `Json::DataType::integer` and `Json::DataType::floating_point` respectively. This is an implementation detail used to avoid loosing precision on integers.

### Accessing `Json` data

## Installation

See [INSTALL.md](./doc/INSTALL.md)