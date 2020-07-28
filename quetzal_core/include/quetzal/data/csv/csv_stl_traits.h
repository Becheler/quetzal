#ifndef DVP_CSV_TRAITS_H
#define DVP_CSV_TRAITS_H

#include "csv_common.h"

namespace dvp
{

template<typename string_type_, typename char_type_ = typename string_type_::value_type>
struct csv_traits_ {
	typedef string_type_ string_type;
	typedef char_type_ char_type;
    static void append(string_type& ref, char_type val) { ref.push_back(val); }
    static void truncate(string_type& ref) { ref.clear();}
	static bool is_separator(char_type c) { return c == char_type(',');}
	static bool is_new_line(char_type c) { return c == char_type('\n');}
	static bool is_double_quote(char_type c) { return c == char_type('"'); }
	static bool is_start_comment(char_type c) { return c == char_type('#'); }
};

typedef csv_traits_<std::string> csv_stl_traits;

struct csv_custom_separator_traits {
	char separator;
	csv_custom_separator_traits()=delete;
	csv_custom_separator_traits(char sep) : separator(sep) {}
	typedef std::string string_type;
	typedef char char_type;
	static void append(string_type& ref, char_type val) { ref.push_back(val); }
	static void truncate(string_type& ref) { ref.clear(); }
	csv_token token_type(char_type c)
	{
		if(c == separator)
			return csv_token::separator;
		else if(c == '\n')
			return csv_token::new_line;
		else if(c == '"')
			return csv_token::double_quote;
		else if(c == '#')
			return csv_token::start_comment;
		else
			return csv_token::other;
	}
};

}
#endif // DVP_CSV_TRAITS_H
