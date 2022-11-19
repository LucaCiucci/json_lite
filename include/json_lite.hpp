#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <ostream>

// if we are in the arduino framework and printable is available,
// we make our Json objects printable
#if __has_include(<Arduino.h>)
	#if __has_include(<Printable.h>)
		#include <Printable.h>
		#define JSON_LITE_PRINTABLE
		#define IF_JSON_LITE_PRINTABLE(...) __VA_ARGS__
	#endif
#endif
#ifndef IF_JSON_LITE_PRINTABLE
	#define IF_JSON_LITE_PRINTABLE(...)
#endif

/*
Note:
  - in arduino and esp32, we don't have some c++ features such as std::variant, so we create a custom json_value class just like in nlohmann::json
*/

namespace json_lite
{
	// describes the type of the JSON value
	enum class json_type
	{
		null,
		boolean,
		number,
		string,
		array,
		object
	};

	// describes the internal representation of the JSON value
	// this is about the same as json_type, but with integer and floating point numbers
	// distinguished
	enum class json_data_type
	{
		null,
		boolean,
		integer,
		floating_point,
		string,
		array,
		object
	};

	// the integer type used for JSON numbers
	using json_int = long long;

	// the floating point representation used for JSON numbers
	using json_float = double;

	// Empty struct representing null, this is mainly used in the
	// internal implementation of initialization and conversion functions.
	// You should not need to use this struct directly and use `Json::to_null()`.
	// Example usage of this struct:
	// ```
	// Json j = Json::null_value_t();
	// ```
	struct json_null_value_t {};

	class Json;

	// Provides the actual type given a json_data_type value. It is useful in
	// determining return types of functions since they are declared in the cpp
	// and thus we cannot use `auto` in the header.
	template <json_data_type Ty> struct json_data_type_to_type;
	template <> struct json_data_type_to_type<json_data_type::null> { using type = json_null_value_t; using ref = json_null_value_t&; using cref = const json_null_value_t&; using ret_val = json_null_value_t; };
	template <> struct json_data_type_to_type<json_data_type::boolean> { using type = bool; using ref = bool&; using cref = const bool&; using ret_val = bool; };
	template <> struct json_data_type_to_type<json_data_type::integer> { using type = json_int; using ref = json_int&; using cref = const json_int&; using ret_val = json_int; };
	template <> struct json_data_type_to_type<json_data_type::floating_point> { using type = json_float; using ref = json_float&; using cref = const json_float&; using ret_val = json_float; };
	template <> struct json_data_type_to_type<json_data_type::string> { using type = std::string; using ref = std::string&; using cref = const std::string&; using ret_val = const std::string&; };
	template <> struct json_data_type_to_type<json_data_type::array> { using type = std::vector<Json>; using ref = std::vector<Json>&; using cref = const std::vector<Json>&; using ret_val = const std::vector<Json>&; };
	template <> struct json_data_type_to_type<json_data_type::object> { using type = std::map<std::string, Json>; using ref = std::map<std::string, Json>&; using cref = const std::map<std::string, Json>&; using ret_val = const std::map<std::string, Json>&; };

	// ================================================================
	//                         Json class
	// ================================================================

	// Represents a JSON value, use this class to store and manipulate JSON values.
	class Json
#ifdef JSON_LITE_PRINTABLE
		: public Printable
#endif
	{
	public:

		// ================================
		//             Types
		// ================================

		// ================
		//   Data types
		// ================

		// The type of the JSON value
		using Type = json_type;

		// The type of the internal representation of the JSON value
		using DataType = json_data_type;

		// the integer type used for JSON numbers
		using Int = json_int;

		// the floating point representation used for JSON numbers
		using Float = json_float;

		// the type of the null value
		using null_value_t = json_null_value_t;

		// ================
		//  Utility types
		// ================

		// used to create an array
		struct make_array_t {};

		// used to create an object
		struct make_object_t {};

		// ================================
		//           Constructors
		// ================================

		// Default constructor, creates a null value
		Json() : m_data_type(json_data_type::null), m_value(null_value_t()) {}

		// Null constructor, creates a null value from the null pointer
		// example:
		// ```cpp
		// Json j = nullptr;
		// ```
		explicit Json(std::nullptr_t) : Json() {}

		// Null constructor, creates a null value from the null value type
		explicit Json(null_value_t) : Json() {}

		// Copy constructor
		Json(const Json& other);

		// Move constructor
		Json(Json&& other);

		// Constructor from a boolean value
		// example:
		// ```cpp
		// Json j = true;
		// ```
		explicit Json(bool value) : m_data_type(json_data_type::boolean), m_value(value) {}

		// Constructor from an integer value
		// example:
		// ```cpp
		// Json j = (Json::Int)42; // <- explicit cast is needed to avoid ambiguity with the constructor from a floating point value
		// ```
		explicit Json(Int value) : m_data_type(json_data_type::integer), m_value(value) {}

		// Constructor from a floating point value
		// example:
		// ```cpp
		// Json j = (Json::Float)42.42; // <- explicit cast is needed to avoid ambiguity with the constructor from an integer value
		// ```
		explicit Json(Float value) : m_data_type(json_data_type::floating_point), m_value(value) {}

		// Constructor from a string value (`const char*`)
		// example:
		// ```cpp
		// Json j = "hello world";
		// ```
		explicit Json(const char* value) : m_data_type(json_data_type::string), m_value(value) {}

		// Constructor from a string value (`std::string`)
		// example:
		// ```cpp
		// Json j = std::string("hello world");
		// ```
		explicit Json(const std::string& value) : m_data_type(json_data_type::string), m_value(value) {}
		
		// Constructor from rval string value (`std::string&&`)
		explicit Json(std::string&& value) : m_data_type(json_data_type::string), m_value(value) {}

		// Constructor from a vector of JSON values
		explicit Json(const std::vector<Json>& value) : m_data_type(json_data_type::array), m_value(value) {}

		// Constructor from rval vector of JSON values
		explicit Json(std::vector<Json>&& value) : m_data_type(json_data_type::array), m_value(value) {}

		// Constructor from a map of JSON values
		explicit Json(const std::map<std::string, Json>& value) : m_data_type(json_data_type::object), m_value(value) {}

		// Constructor from rval map of JSON values
		explicit Json(std::map<std::string, Json>&& value) : m_data_type(json_data_type::object), m_value(value) {}

		// Constructor from make_array_t, creates an empty array
		explicit Json(make_array_t) : m_data_type(json_data_type::array), m_value(std::vector<Json>()) {}

		// Constructor from make_object_t, creates an empty object
		explicit Json(make_object_t) : m_data_type(json_data_type::object), m_value(std::map<std::string, Json>()) {}

		// ================================
		//           Destructor
		// ================================

		// Destructor
		~Json() IF_JSON_LITE_PRINTABLE(override);

		// ================================
		//           Assignments
		// ================================

		// Copy assignment
		Json& operator=(const Json& other);

		// Move assignment
		Json& operator=(Json&& other);

		// Assignment to null from the null pointer
		Json& operator=(std::nullptr_t);

		// Assignment to null from the null value type
		Json& operator=(null_value_t);

		// Assignment to a boolean value
		Json& operator=(bool value);

		// Assignment to an integer value
		Json& operator=(Int value);

		// Assignment to a floating point value
		Json& operator=(Float value);

		// Assignment to a string value (`const char*`)
		Json& operator=(const char* value);

		// Assignment to a string value (`std::string`)
		Json& operator=(const std::string& value);

		// Assignment to rval string value (`std::string&&`)
		Json& operator=(std::string&& value);

		// Assignment to a vector of JSON values
		Json& operator=(const std::vector<Json>& value);

		// Assignment to rval vector of JSON values
		Json& operator=(std::vector<Json>&& value);

		// Assignment to a map of JSON values
		Json& operator=(const std::map<std::string, Json>& value);

		// Assignment to rval map of JSON values
		Json& operator=(std::map<std::string, Json>&& value);

		// Assignment to empty array
		Json& operator=(make_array_t);

		// Assignment to empty object
		Json& operator=(make_object_t);

		// ================================
		//              Info
		// ================================

		// get the type of the JSON value
		json_type type() const;

		// get the internal data type of the JSON value
		json_data_type data_type() const { return m_data_type; }

		// checks if the JSON type is the given type
		bool is(json_type type) const { return this->type() == type; }

		// checks if the JSON internal data type is the given data type
		bool is(json_data_type type) const { return m_data_type == type; }

		// checks if the JSON value is null
		bool is_null() const { return is(json_type::null); }

		// ================================
		//           Conversions
		// ================================

		// conversion to null, same as `json = nullptr;`
		void to_null();

		// conversion to Array, same as `json = Json::make_array();` or `json = std::vector<Json>();` or `json = Json::make_array_t();`
		void to_array();

		// conversion to Object, same as `json = Json::make_object();` or `json = std::map<std::string, Json>();` or `json = Json::make_object_t();`
		void to_object();

		// creates a null value
		static Json make_null() { return Json(); }

		// creates an empty array
		static Json make_array() { Json j; j.to_array(); return j; }

		// creates an empty object
		static Json make_object() { Json j; j.to_object(); return j; }

		// ================================
		//             Access
		// ================================

		// get the value reference as the given type
		// The type must match the internal data type of the JSON value.
		// TODO make a default inline implementation that uses static_assert(false, "invalid type") to give a better error message
		template <json_data_type Ty> typename json_data_type_to_type<Ty>::type& get();

		// get the const value reference as the given type
		// The type must match the internal data type of the JSON value.
		// TODO make a default inline implementation that uses static_assert(false, "invalid type") to give a better error message
		template <json_data_type Ty> const typename json_data_type_to_type<Ty>::type& get() const;

		// get the reference to the array, if the JSON value is not an array, it will be converted to an array
		// TODO maybe this is an unwanted behavior
		std::vector<Json>& as_array();

		// get the const reference to the array
		// The current JSON value must be an array.
		const std::vector<Json>& as_array() const;

		// get the reference to the object, if the JSON value is not an object, it will be converted to an object
		// TODO maybe this is an unwanted behavior
		std::map<std::string, Json>& as_object();

		// get the const reference to the object
		// The current JSON value must be an object.
		const std::map<std::string, Json>& as_object() const;

		// ================================
		//           Array Access
		// ================================

		// get the reference to the element at the given index
		// if the JSON value is not an array, it will be converted to an array
		// if the index is out of bounds, the array will be resized to fit the index
		Json& operator[](size_t index);

		// get the const reference to the element at the given index
		// The current JSON value must be an array.
		// Index must be in bounds, no bounds checking is done.
		const Json& operator[](size_t index) const;

		// alisa for `operator[](size_t)`, used to avoid ambiguity with `operator[](const std::string&)`
		Json& operator[](int index) { return (*this)[static_cast<size_t>(index)]; }

		// get the reference to the element at the given index
		// if the JSON value is not an array, it will be converted to an array
		// if the index is out of bounds, an exception will be thrown
		Json& at(size_t index);

		// get the const reference to the element at the given index
		// The current JSON value must be an array.
		// if the index is out of bounds, an exception will be thrown
		const Json& at(size_t index) const;

		// alisa for `at(size_t)`, used to avoid ambiguity with `at(const std::string&)`
		Json& at(int index) { return at(static_cast<size_t>(index)); }

		// alias for `at(size_t)`, used to avoid ambiguity with `at(const std::string&)`
		const Json& at(int index) const { return at(static_cast<size_t>(index)); }

		// ================================
		//           Object Access
		// ================================

		// get the reference to the element with the given key
		// if the JSON value is not an object, it will be converted to an object
		// if the key does not exist, it will be created
		Json& operator[](const std::string& key);

		// get the const reference to the element with the given key
		// The current JSON value must be an object.
		// if the key does not exist, an exception will be thrown
		const Json& operator[](const std::string& key) const;

		// get the reference to the element with the given key
		// if the JSON value is not an object, it will be converted to an object
		// if the key does not exist, it will be created
		Json& operator[](const char* key);

		// get the const reference to the element with the given key
		// The current JSON value must be an object.
		// if the key does not exist, an exception will be thrown
		const Json& operator[](const char* key) const;

		// get the reference to the element with the given key
		// if the JSON value is not an object, it will be converted to an object
		// if the key does not exist, an exception will be thrown
		Json& at(const std::string& key);

		// get the const reference to the element with the given key
		// The current JSON value must be an object.
		// if the key does not exist, an exception will be thrown
		const Json& at(const std::string& key) const;

		// get the reference to the element with the given key
		// if the JSON value is not an object, it will be converted to an object
		// if the key does not exist, an exception will be thrown
		Json& at(const char* key);

		// get the const reference to the element with the given key
		// The current JSON value must be an object.
		// if the key does not exist, an exception will be thrown
		const Json& at(const char* key) const;

		bool has_key(const std::string& key) const;

		// ================================
		//           Cast Operators
		// ================================

		// cast to `bool`
		explicit operator bool() const;

		// cast to `bool&`
		explicit operator bool&();

		// cast to `int`
		explicit operator Int() const;

		// cast to `int&`
		explicit operator Int&();

		// cast to `Float`
		explicit operator Float() const;

		// cast to `Float&`
		explicit operator Float&();

		// cast to `string`
		explicit operator std::string() const;

		// cast to `const string&`
		explicit operator const std::string&() const;

		// cast to `string&`
		explicit operator std::string&();

		// cast to `const char*`
		explicit operator const char*() const;

		// ================================
		//            Parsing
		// ================================

		// The exception thrown when parsing fails.
		// we use a single exception class for all parsing errors
		// to reduce the final binary size
		// TODO maybe this is not perceptible...
		struct parsing_error : public std::runtime_error { using std::runtime_error::runtime_error; };

		// parse a JSON value from a string
		static Json parse(const char* str);

		// parse a JSON value from a string
		static Json parse(const char* begin, const char* end);

		// parse a JSON value from a string
		static Json parse(const std::string& str);

		// dump to a string, no indentation (lighter)
		std::string dump() const;

		// TODO std::string dump(size_t indent) const;

#ifdef JSON_LITE_PRINTABLE
		// implements the Printable interface
		size_t printTo(Print& p) const override;
#endif

	private:

		// ================================
		//             Data
		// ================================

		json_data_type m_data_type;

		union value_union_t
		{
			null_value_t null_value;
			bool boolean;
			Int integer;
			Float floating;
			std::string string;
			std::vector<Json> array;
			std::map<std::string, Json> object;

			value_union_t();
			value_union_t(const value_union_t& other) = delete;
			value_union_t(value_union_t&& other) = delete;

			value_union_t(null_value_t value);
			value_union_t(bool value);
			value_union_t(Int value);
			value_union_t(Float value);
			value_union_t(const char* value);
			value_union_t(const std::string& value);
			value_union_t(std::string&& value);
			value_union_t(const std::vector<Json>& value);
			value_union_t(std::vector<Json>&& value);
			value_union_t(const std::map<std::string, Json>& value);
			value_union_t(std::map<std::string, Json>&& value);

			~value_union_t() noexcept {};
		} m_value;
	};

	// ================================================================
	//                       External functions
	// ================================================================
}

////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, const json_lite::Json& json) {
	os << json.dump();
	return os;
}