
# `json_lite` Documentation

## Introduction

`json_lite` is simple and lightweight JSON parser and serializer for embedded systems. It allows you to manage json data in a simple and expressive way.

## Rationale

There are a lots of JSON libraries for C++ and embedded systems. I found that the existing libraries are not well suited for my applications. In particular, I used to link `Arduino_JSON` that relies on cJSON, but I spent a lot of time trying to understand how to use it and circumvent its bugs and limitations.  
Among the existing libraries, I tried to use `nlohmann/json` that I used meny times for its simplicity and flexibility. It is a near perfection library, but it produces a lot of code and it was not suitable for my application since I had only few free kB of Flash after linking all the other needed libraries.

I decided to write my own library using the KISS principles, with particular attention to the final binary size.  
I also had to develop it without spending a lot of time, so I decided to use a simple and straightforward approach with the help of GitHub Copilot. This is the reason the code seems to have a lot repetitive patterns but this is fine since it is extremely simple and the final binary size is very small.

Principles:
 - Simple and easy to use API
 - Small resource footprint
 - KISS
 - Quick development
 - No dependencies
 - Put as much code as possible in the cpp file to reduce the binary size

## Getting Started

### Installation

You can use the PlatformIO library manager, see [INSTALL.md](INSTALL.md) for more details.

### Usage

Once you included `<json_lite.hpp>` in your project, you can use the `json_lite::Json` class to manage json data. It should be very intuitive to use, see [get_started.md](get_started.md) for more details.