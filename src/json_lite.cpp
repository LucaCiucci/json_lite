
#ifdef ESP32
// ! very bad hack
// https://esp32.com/viewtopic.php?t=2660
// https://www.esp32.com/viewtopic.php?t=3395
#define _GLIBCXX_USE_C99
#endif

#include "json_lite.hpp"

#ifdef JSON_LITE_PRINTABLE
	#include <Print.h>
#endif

#include <cassert>

namespace json_lite
{
	// ================================================================
	//                         Json class
	// ================================================================

	// ================================
	//             Types
	// ================================

	// ================
	//   Data types
	// ================

	// ================
	//  Utility types
	// ================

	// ================================
	//           Constructors
	// ================================

	////////////////////////////////////////////////////////////////
	Json::Json(const Json& other) :
		m_data_type(other.m_data_type), // copy the data type
		m_value(null_value_t())         // initialize the to empty, copy it later in the switch
	{
		switch (m_data_type)
		{
		case json_data_type::null:
			m_value.null_value = null_value_t{};
			break;
		case json_data_type::boolean:
			m_value.boolean = other.m_value.boolean;
			break;
		case json_data_type::integer:
			m_value.integer = other.m_value.integer;
			break;
		case json_data_type::floating_point:
			m_value.floating = other.m_value.floating;
			break;
		case json_data_type::string:
			new (&m_value.string) std::string(other.m_value.string);
			break;
		case json_data_type::array:
			new (&m_value.array) std::vector<Json>(other.m_value.array);
			break;
		case json_data_type::object:
			new (&m_value.object) std::map<std::string, Json>(other.m_value.object);
			break;
		default:
			throw std::runtime_error("Json::json_value(const json_value& other): unknown json_data_type");
			break;
		};
	}

	////////////////////////////////////////////////////////////////
	Json::Json(Json&& other) :
		m_data_type(other.m_data_type), // copy the data type
		m_value(null_value_t())         // initialize the to empty, move it later in the switch
	{
		switch (m_data_type)
		{
		case json_data_type::null:
			m_value.null_value = null_value_t{};
			break;
		case json_data_type::boolean:
			m_value.boolean = other.m_value.boolean;
			break;
		case json_data_type::integer:
			m_value.integer = other.m_value.integer;
			break;
		case json_data_type::floating_point:
			m_value.floating = other.m_value.floating;
			break;
		case json_data_type::string:
			new (&m_value.string) std::string(std::move(other.m_value.string));
			break;
		case json_data_type::array:
			new (&m_value.array) std::vector<Json>(std::move(other.m_value.array));
			break;
		case json_data_type::object:
			new (&m_value.object) std::map<std::string, Json>(std::move(other.m_value.object));
			break;
		default:
			throw std::runtime_error("Json::json_value(json_value&& other): unknown json_data_type");
			break;
		};
	}

	// ================================
	//           Destructor
	// ================================

	////////////////////////////////////////////////////////////////
	Json::~Json()
	{
		switch (m_data_type)
		{
		case json_data_type::null:
		case json_data_type::boolean:
		case json_data_type::integer:
		case json_data_type::floating_point:
			// nothing to do
			break;
		case json_data_type::string:
			m_value.string.~basic_string();
			break;
		case json_data_type::array:
			m_value.array.~vector();
			break;
		case json_data_type::object:
			m_value.object.~map();
			break;
		default:
			throw std::runtime_error("Json::~json_value() - unknown json_data_type");
			break;
		};

		// TODO our constructors are not noexcept, we should guarantee that m_value is always destroyed
		// by incapsulating it in another class
	}

	// ================================
	//           Assignments
	// ================================

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(const Json& other)
	{
		if (this != &other)
		{
			// the simplest way to implement this is destroy the current value and
			// re-create it by calling the copy constructor
			this->~Json();
			new (this) Json(other); // in place copy constructor
		}
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(Json&& other)
	{
		if (this != &other)
		{
			// the simplest way to implement this is destroy the current value and
			// re-create it by calling the move constructor
			this->~Json();
			new (this) Json(std::move(other)); // in place move constructor
		}
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(std::nullptr_t)
	{
		this->~Json();
		new (this) Json();
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(null_value_t)
	{
		// defer to the nullptr_t overload
		*this = nullptr;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(bool value)
	{
		this->~Json();
		new (this) Json(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(Int value)
	{
		this->~Json();
		new (this) Json(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(double value)
	{
		this->~Json();
		new (this) Json(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(const char* value)
	{
		this->~Json();
		new (this) Json(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(const std::string& value)
	{
		this->~Json();
		new (this) Json(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(std::string&& value)
	{
		this->~Json();
		new (this) Json(std::move(value));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(const std::vector<Json>& value)
	{
		this->~Json();
		new (this) Json(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(std::vector<Json>&& value)
	{
		this->~Json();
		new (this) Json(std::move(value));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(const std::map<std::string, Json>& value)
	{
		this->~Json();
		new (this) Json(value);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(std::map<std::string, Json>&& value)
	{
		this->~Json();
		new (this) Json(std::move(value));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(make_array_t)
	{
		this->~Json();
		new (this) Json(make_array_t{});
		return *this;
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator=(make_object_t)
	{
		this->~Json();
		new (this) Json(make_object_t{});
		return *this;
	}

	////////////////////////////////////////////////////////////////
	json_type Json::type() const
	{
		switch (m_data_type)
		{
		case json_data_type::null:
			return json_type::null;
		case json_data_type::boolean:
			return json_type::boolean;
		case json_data_type::integer:
			return json_type::number;
		case json_data_type::floating_point:
			return json_type::number;
		case json_data_type::string:
			return json_type::string;
		case json_data_type::array:
			return json_type::array;
		case json_data_type::object:
			return json_type::object;
		default:
			throw std::runtime_error("Json::type() - unknown json_data_type");
			break;
		};
	}

	////////////////////////////////////////////////////////////////
	void Json::to_null()
	{
		*this = nullptr;
	}

	////////////////////////////////////////////////////////////////
	void Json::to_array()
	{
		*this = make_array_t();
	}

	////////////////////////////////////////////////////////////////
	void Json::to_object()
	{
		*this = make_object_t();
	}

	////////////////////////////////////////////////////////////////
	template <> typename json_data_type_to_type<json_data_type::null>::type& Json::get<json_data_type::null>()
	{
		if (m_data_type != json_data_type::null)
			// note: same string for all getters to save space
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.null_value;
	}

	////////////////////////////////////////////////////////////////
	template <> typename json_data_type_to_type<json_data_type::boolean>::type& Json::get<json_data_type::boolean>()
	{
		if (m_data_type != json_data_type::boolean)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.boolean;
	}

	////////////////////////////////////////////////////////////////
	template <> typename json_data_type_to_type<json_data_type::integer>::type& Json::get<json_data_type::integer>()
	{
		if (m_data_type != json_data_type::integer)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.integer;
	}

	////////////////////////////////////////////////////////////////
	template <> typename json_data_type_to_type<json_data_type::floating_point>::type& Json::get<json_data_type::floating_point>()
	{
		if (m_data_type != json_data_type::floating_point)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.floating;
	}

	////////////////////////////////////////////////////////////////
	template <> typename json_data_type_to_type<json_data_type::string>::type& Json::get<json_data_type::string>()
	{
		if (m_data_type != json_data_type::string)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.string;
	}

	////////////////////////////////////////////////////////////////
	template <> typename json_data_type_to_type<json_data_type::array>::type& Json::get<json_data_type::array>()
	{
		if (m_data_type != json_data_type::array)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.array;
	}

	////////////////////////////////////////////////////////////////
	template <> typename json_data_type_to_type<json_data_type::object>::type& Json::get<json_data_type::object>()
	{
		if (m_data_type != json_data_type::object)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.object;
	}

	////////////////////////////////////////////////////////////////
	template <> const typename json_data_type_to_type<json_data_type::null>::type& Json::get<json_data_type::null>() const
	{
		if (m_data_type != json_data_type::null)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.null_value;
	}

	////////////////////////////////////////////////////////////////
	template <> const typename json_data_type_to_type<json_data_type::boolean>::type& Json::get<json_data_type::boolean>() const
	{
		if (m_data_type != json_data_type::boolean)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.boolean;
	}

	////////////////////////////////////////////////////////////////
	template <> const typename json_data_type_to_type<json_data_type::integer>::type& Json::get<json_data_type::integer>() const
	{
		if (m_data_type != json_data_type::integer)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.integer;
	}

	////////////////////////////////////////////////////////////////
	template <> const typename json_data_type_to_type<json_data_type::floating_point>::type& Json::get<json_data_type::floating_point>() const
	{
		if (m_data_type != json_data_type::floating_point)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.floating;
	}

	////////////////////////////////////////////////////////////////
	template <> const typename json_data_type_to_type<json_data_type::string>::type& Json::get<json_data_type::string>() const
	{
		if (m_data_type != json_data_type::string)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.string;
	}

	////////////////////////////////////////////////////////////////
	template <> const typename json_data_type_to_type<json_data_type::array>::type& Json::get<json_data_type::array>() const
	{
		if (m_data_type != json_data_type::array)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.array;
	}

	////////////////////////////////////////////////////////////////
	template <> const typename json_data_type_to_type<json_data_type::object>::type& Json::get<json_data_type::object>() const
	{
		if (m_data_type != json_data_type::object)
			throw std::runtime_error("Json::get<...>() - wrong type");
		return m_value.object;
	}

	////////////////////////////////////////////////////////////////
	std::vector<Json>& Json::as_array()
	{
		if (this->data_type() == json_data_type::array)
			return this->get<json_data_type::array>();
		else
		{
			this->to_array();
			return this->get<json_data_type::array>();
		}
	}

	////////////////////////////////////////////////////////////////
	const std::vector<Json>& Json::as_array() const
	{
		if (this->data_type() == json_data_type::array)
			return this->get<json_data_type::array>();
		else
			throw std::runtime_error("Json::as_array() - wrong type");
	}

	////////////////////////////////////////////////////////////////
	std::map<std::string, Json>& Json::as_object()
	{
		if (this->data_type() == json_data_type::object)
			return this->get<json_data_type::object>();
		else
		{
			this->to_object();
			return this->get<json_data_type::object>();
		}
	}

	////////////////////////////////////////////////////////////////
	const std::map<std::string, Json>& Json::as_object() const
	{
		if (this->data_type() == json_data_type::object)
			return this->get<json_data_type::object>();
		else
			throw std::runtime_error("Json::as_object() - wrong type");
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator[](size_t index)
	{
		auto& arr = this->as_array();
		if (index >= arr.size())
			arr.resize(index + 1);
		return arr[index];
	}

	////////////////////////////////////////////////////////////////
	const Json& Json::operator[](size_t index) const
	{
		auto& arr = this->as_array();
		return arr[index];
	}

	////////////////////////////////////////////////////////////////
	Json& Json::at(size_t index)
	{
		return this->as_array().at(index);
	}

	////////////////////////////////////////////////////////////////
	const Json& Json::at(size_t index) const
	{
		return this->as_array().at(index);
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator[](const std::string& key)
	{
		return this->as_object()[key];
	}

	////////////////////////////////////////////////////////////////
	const Json& Json::operator[](const std::string& key) const
	{
		return this->as_object().at(key);
	}

	////////////////////////////////////////////////////////////////
	Json& Json::operator[](const char* key)
	{
		return this->as_object()[key];
	}

	////////////////////////////////////////////////////////////////
	const Json& Json::operator[](const char* key) const
	{
		return this->as_object().at(key);
	}

	////////////////////////////////////////////////////////////////
	Json& Json::at(const std::string& key)
	{
		return this->as_object().at(key);
	}

	////////////////////////////////////////////////////////////////
	const Json& Json::at(const std::string& key) const
	{
		return this->as_object().at(key);
	}

	////////////////////////////////////////////////////////////////
	Json& Json::at(const char* key)
	{
		return this->as_object().at(key);
	}

	////////////////////////////////////////////////////////////////
	const Json& Json::at(const char* key) const
	{
		return this->as_object().at(key);
	}

	////////////////////////////////////////////////////////////////
	bool Json::has_key(const std::string& key) const
	{
		return this->as_object().find(key) != this->as_object().end();
	}

	////////////////////////////////////////////////////////////////
	Json::operator bool() const
	{
		return this->data_type() != json_data_type::null;
	}

	////////////////////////////////////////////////////////////////
	Json::operator bool&()
	{
		return this->get<json_data_type::boolean>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator Int() const
	{
		return this->get<json_data_type::integer>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator Int&()
	{
		return this->get<json_data_type::integer>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator Float() const
	{
		return this->get<json_data_type::floating_point>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator Float&()
	{
		return this->get<json_data_type::floating_point>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator std::string() const
	{
		return this->get<json_data_type::string>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator const std::string&() const
	{
		return this->get<json_data_type::string>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator std::string&()
	{
		return this->get<json_data_type::string>();
	}

	////////////////////////////////////////////////////////////////
	Json::operator const char*() const
	{
		return this->get<json_data_type::string>().c_str();
	}

	////////////////////////////////////////////////////////////////
	Json Json::parse(const char* str)
	{
		return Json::parse(str, str + strlen(str));
	}

	namespace
	{
		const char* skip_whitespace(const char* p, const char* end)
		{
			while (p != end && isspace(*p))
				++p;
			return p;
		}

		const char* parse_json(const char* begin, const char* end, Json& obj);

		bool will_null_parse(const char* begin, const char* end)
		{
			return begin != end && begin[0] == 'n';
		}

		bool will_boolean_parse(const char* begin, const char* end)
		{
			return begin != end && (begin[0] == 't' || begin[0] == 'f');
		}

		bool will_number_parse(const char* begin, const char* end)
		{
			return begin != end && (
				begin[0] == '-' ||
				(begin[0] >= '0' && begin[0] <= '9') ||
				begin[0] == '.'
				);
		}

		bool will_string_parse(const char* begin, const char* end)
		{
			return begin != end && begin[0] == '"';
		}

		bool will_array_parse(const char* begin, const char* end)
		{
			return begin != end && begin[0] == '[';
		}

		bool will_object_parse(const char* begin, const char* end)
		{
			return begin != end && begin[0] == '{';
		}

		const char* parse_json_null(const char* begin, const char* end, Json& obj)
		{
			if (end - begin < 4)
				throw std::runtime_error("parse_json_null() - invalid json");
			if (strncmp(begin, "null", 4) != 0)
				throw std::runtime_error("parse_json_null() - invalid json");
			obj = nullptr;
			return begin + 4;
		}

		const char* parse_json_boolean(const char* begin, const char* end, Json& obj)
		{
			if (strncmp(begin, "true", 4) == 0)
			{
				obj = true;
				return begin + 4;
			}

			if (strncmp(begin, "false", 5) == 0)
			{
				obj = false;
				return begin + 5;
			}

			throw Json::parsing_error("parse_json_boolean() - invalid boolean");
		}

		const char* parse_json_number(const char* begin, const char* end, Json& obj)
		{
			const char* p = begin;
			bool is_float = false;
			while (p != end)
			{
				if (*p == '.' || *p == 'e' || *p == 'E')
					is_float = true;
				else if (!isdigit(*p))
					break;
				++p;
			}
			if (is_float)
			{
				obj = (Json::Float)atof(begin);
			}
			else
			{
				obj = (Json::Int)atoi(begin);
			}
			return p;
		}

		const char* parse_json_string(const char* begin, const char* end, Json& obj)
		{
			if (*begin != '\"')
				throw Json::parsing_error("parse_json_string() - invalid string");
			++begin;
			const char* p = begin;
			std::string str;
			while (p != end)
			{
				if (*p == '\"')
				{
					str.append(begin, p);
					obj = str;
					return p + 1;
				}
				else if (*p == '\\')
				{
					str.append(begin, p);
					++p;
					if (p == end)
						throw Json::parsing_error("parse_json_string() - invalid string");
					switch (*p)
					{
					case '\"':
						str.push_back('\"');
						break;
					case '\\':
						str.push_back('\\');
						break;
					case '/':
						str.push_back('/');
						break;
					case 'b':
						str.push_back('\b');
						break;
					case 'f':
						str.push_back('\f');
						break;
					case 'n':
						str.push_back('\n');
						break;
					case 'r':
						str.push_back('\r');
						break;
					case 't':
						str.push_back('\t');
						break;
					case 'u':
						throw Json::parsing_error("parse_json_string() - unicode not supported");
					default:
						throw Json::parsing_error("parse_json_string() - invalid string");
					}
					++p;
					begin = p;
				}
				else
				{
					++p;
				}
			}
			throw Json::parsing_error("parse_json_string() - invalid string");
		}

		const char* parse_json_array(const char* begin, const char* end, Json& obj)
		{
			if (*begin != '[')
				throw Json::parsing_error("parse_json_array() - invalid array");
			++begin;
			Json array = Json::make_array();
			while (begin != end)
			{
				begin = skip_whitespace(begin, end);
				if (begin == end)
					throw Json::parsing_error("parse_json_array() - invalid array");
				if (*begin == ']')
				{
					obj = array;
					return begin + 1;
				}
				Json value;
				begin = parse_json(begin, end, value);
				array.as_array().push_back(value);
				begin = skip_whitespace(begin, end);
				if (begin == end)
					throw Json::parsing_error("parse_json_array() - invalid array");
				if (*begin == ']')
				{
					obj = array;
					return begin + 1;
				}
				if (*begin != ',')
					throw Json::parsing_error("parse_json_array() - invalid array");
				++begin;
			}
			throw Json::parsing_error("parse_json_array() - invalid array");
		}

		const char* parse_json_object(const char* begin, const char* end, Json& obj)
		{
			if (*begin != '{')
				throw Json::parsing_error("parse_json_obj() - invalid object");
			++begin;
			Json object = Json::make_object();
			while (begin != end)
			{
				begin = skip_whitespace(begin, end);
				if (begin == end)
					throw Json::parsing_error("parse_json_obj() - invalid object");
				if (*begin == '}')
				{
					obj = object;
					return begin + 1;
				}
				Json key;
				begin = parse_json_string(begin, end, key);
				begin = skip_whitespace(begin, end);
				if (begin == end)
					throw Json::parsing_error("parse_json_obj() - invalid object");
				if (*begin != ':')
					throw Json::parsing_error("parse_json_obj() - invalid object");
				++begin;
				begin = skip_whitespace(begin, end);
				if (begin == end)
					throw Json::parsing_error("parse_json_obj() - invalid object");
				Json value;
				begin = parse_json(begin, end, value);
				object[key.get<json_data_type::string>()] = value;
				begin = skip_whitespace(begin, end);
				if (begin == end)
					throw Json::parsing_error("parse_json_obj() - invalid object");
				if (*begin == '}')
				{
					obj = object;
					return begin + 1;
				}
				if (*begin != ',')
					throw Json::parsing_error("parse_json_obj() - invalid object");
				++begin;
			}
			throw Json::parsing_error("parse_json_obj() - invalid object");
		}

		const char* parse_json(const char* begin, const char* end, Json& obj)
		{
			if (begin == end)
				// empty
				begin;

			begin = skip_whitespace(begin, end);

			if (begin == end)
				// empty
				begin;

			if (will_null_parse(begin, end))
				return parse_json_null(begin, end, obj);
			else if (will_boolean_parse(begin, end))
				return parse_json_boolean(begin, end, obj);
			else if (will_number_parse(begin, end))
				return parse_json_number(begin, end, obj);
			else if (will_string_parse(begin, end))
				return parse_json_string(begin, end, obj);
			else if (will_array_parse(begin, end))
				return parse_json_array(begin, end, obj);
			else if (will_object_parse(begin, end))
				return parse_json_object(begin, end, obj);
			else
				throw Json::parsing_error("parse_json() - invalid json");
		}
	}

	////////////////////////////////////////////////////////////////
	Json Json::parse(const char* begin, const char* end)
	{
		Json obj;
		auto last = parse_json(begin, end, obj);
		return obj;
	}

	////////////////////////////////////////////////////////////////
	Json Json::parse(const std::string& str)
	{
		return parse(str.c_str(), str.c_str() + str.size());
	}

	namespace
	{
		std::string dump_string(const std::string& str)
		{
			std::string result;
			result.reserve(str.size() + 2);
			result.push_back('"');
			for (auto c : str)
			{
				switch (c)
				{
				case '"':
					result.append("\\\"");
					break;
				case '\\':
					result.append("\\\\");
					break;
				case '\b':
					result.append("\\b");
					break;
				case '\f':
					result.append("\\f");
					break;
				case '\n':
					result.append("\\n");
					break;
				case '\r':
					result.append("\\r");
					break;
				case '\t':
					result.append("\\t");
					break;
				default:
					result.push_back(c);
					break;
				}
			}
			result.push_back('"');
			return result;
		}

		void dump_to_string(const Json& json, std::string& str)
		{
			if (json.is(json_data_type::null))
			{
				str += "null";
				return;
			}

			if (json.is(json_data_type::boolean))
			{
				str += json.get<json_data_type::boolean>() ? "true" : "false";
				return;
			}

			if (json.is(json_data_type::integer))
			{
				str += std::to_string(json.get<json_data_type::integer>());
				return;
			}

			if (json.is(json_data_type::floating_point))
			{
				str += std::to_string(json.get<json_data_type::floating_point>());
				return;
			}

			if (json.is(json_data_type::string))
			{
				str += dump_string(json.get<json_data_type::string>());
				return;
			}

			if (json.is(json_data_type::array))
			{
				str += '[';
				for (auto it = json.get<json_data_type::array>().begin(); it != json.get<json_data_type::array>().end(); ++it)
				{
					if (it != json.get<json_data_type::array>().begin())
						str += ',';
					dump_to_string(*it, str);
				}
				str += ']';
				return;
			}

			if (json.is(json_data_type::object))
			{
				str += '{';
				for (auto it = json.get<json_data_type::object>().begin(); it != json.get<json_data_type::object>().end(); ++it)
				{
					if (it != json.get<json_data_type::object>().begin())
						str += ',';
					str += dump_string(it->first);
					str += ":";
					dump_to_string(it->second, str);
				}
				str += '}';
				return;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	std::string Json::dump() const
	{
		std::string str;
		dump_to_string(*this, str);
		return str;
	}

#ifdef JSON_LITE_PRINTABLE
	////////////////////////////////////////////////////////////////
	size_t Json::printTo(Print& p) const
	{
		return p.print(this->dump().c_str());
	}
#endif

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t()
	{
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(null_value_t value)
	{
		null_value = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(bool value)
	{
		boolean = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(Int value)
	{
		integer = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(Float value)
	{
		floating = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(const char* value)
	{
		new (&string) std::string(value);
		string = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(const std::string& value)
	{
		new (&string) std::string(value);
		string = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(std::string&& value)
	{
		new (&string) std::string(std::move(value));
		string = std::move(value);
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(const std::vector<Json>& value)
	{
		new (&array) std::vector<Json>(value);
		array = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(std::vector<Json>&& value)
	{
		new (&array) std::vector<Json>(std::move(value));
		array = std::move(value);
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(const std::map<std::string, Json>& value)
	{
		new (&object) std::map<std::string, Json>(value);
		object = value;
	}

	////////////////////////////////////////////////////////////////
	Json::value_union_t::value_union_t(std::map<std::string, Json>&& value)
	{
		new (&object) std::map<std::string, Json>(std::move(value));
		object = std::move(value);
	}
}