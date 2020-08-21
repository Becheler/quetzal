#ifndef DVP_CSV_PARSER_H
#define DVP_CSV_PARSER_H

#include <functional>
#include "csv_common.h"

namespace dvp
{

template<typename csv_traits>
class csv_parser : protected csv_traits
{
private:
    int current_line_;
    int current_pos_;
    typename csv_traits::string_type last_value_;
    bool complete_;
	csv_error error_;
    enum class state : uint8_t
    {
        initial = 0,
        comment,
        record,
        quoted_record,
        quote_quoted_record,
        new_line,
        between_records
    };
    state state_;

	void call_field_handler();
	void call_end_line_handler();
	void call_comment_handler();
	void call_error_handler(csv_error err);
	void update_line_counter() {
		current_line_ += 1;
		current_pos_ = 1;
	}

public:
    std::function<bool(typename csv_traits::string_type const&)> comment_handler;
    std::function<bool(typename csv_traits::string_type const&)> field_handler;
    std::function<bool()> end_line_handler;
    std::function<bool()> end_file_handler;
    std::function<void(csv_error,int,int)> error_handler;

	static int state_matrix[7][5];

	template<typename... Args>
    csv_parser(Args... args);
    ~csv_parser()=default;
    csv_parser(csv_parser const&)=delete;
    csv_parser& operator=(csv_parser const&)=delete;

    /**
     * @brief consume consumes a new character, and acts accordingly
     */
    inline void consume(typename csv_traits::char_type c);

    /**
     * Inform the parser that data end has been reached.
     */
    void end_of_data();

    /**
     * @brief abort stops the parser (may be used if there is no need to continue because something is missing)
     */
    void abort();

    /**
     * @brief complete returns true if the parser has completed without errors
     * @return
     */
    bool complete() const;

    /**
     * @brief current_line returns the current line number
     * @return
     */
    int current_line() const;

    /**
     * @brief current_position returns the current position
     * @return
     */
    int current_position() const;

	/**
	 * @brief returns true if an error occurred
	 */
	bool error() const;

	csv_error last_error() const;

    /**
     * @brief set_separator sets the field separator used by the csv parser
     * @param separator character to use as separator
     */
    void set_separator(typename csv_traits::char_type separator);

    void reset() {
        complete_ = false;
		error_ = csv_error::no_error;
        state_ = state::initial;
        current_pos_ = 1;
        current_line_ = 1;
        csv_traits::truncate(last_value_);
    }

};

template<typename csv_traits>
template<typename... Args>
csv_parser<csv_traits>::csv_parser(Args... args) : csv_traits(args...),
    current_line_(1),
    current_pos_(1),
    complete_(false),
	error_(csv_error::no_error),
    state_(state::initial)
{
}

template<typename csv_traits>
void csv_parser<csv_traits>::call_field_handler()
{
	if(field_handler && error_ == csv_error::no_error)
	{
		if(!field_handler(last_value_))
		{
			call_error_handler(csv_error::error_user_aborted);
		}
	}
	csv_traits::truncate(last_value_);
}

template<typename csv_traits>
void csv_parser<csv_traits>::call_end_line_handler()
{
	if(end_line_handler)
	{
		end_line_handler();
	}
	update_line_counter();
}

template<typename csv_traits>
void csv_parser<csv_traits>::call_error_handler(csv_error err)
{
	error_ = err;
	if(error_handler)
		error_handler(error_, current_line_, current_pos_);
}

template<typename csv_traits>
void csv_parser<csv_traits>::call_comment_handler()
{
	if(comment_handler && error_ == csv_error::no_error)
	{
		if(!comment_handler(last_value_))
			call_error_handler(csv_error::error_user_aborted);
	}
	csv_traits::truncate(last_value_);
}

template<typename csv_traits>
void csv_parser<csv_traits>::consume(typename csv_traits::char_type c)
{
//    csv_token tok = csv_traits::token_type(c);
    current_pos_ += 1;
	switch(state_)
	{
		case state::initial:
		case state::new_line:
		{
			if(csv_traits::is_separator(c))
			{
				call_field_handler();
				state_ = state::between_records;
			}
			else if(csv_traits::is_double_quote(c))
				state_ = state::quoted_record;
			else if(csv_traits::is_start_comment(c))
				state_ = state::comment;
			else if(csv_traits::is_new_line(c))
			{
				call_end_line_handler();
				state_ = state::new_line;
			}
			else
			{
				csv_traits::append(last_value_, c);
				state_ = state::record;
			}
			break;
		}
		case state::record:
		{
			if(csv_traits::is_separator(c))
			{
				call_field_handler();
				state_ = state::between_records;
			}
			else if(csv_traits::is_new_line(c))
			{
				call_field_handler();
				call_end_line_handler();
				state_ = state::new_line;
			}
			else 
				csv_traits::append(last_value_, c);
			break;
		}
		case state::quote_quoted_record:
		{
			if(csv_traits::is_separator(c))
			{
				call_field_handler();
				state_ = state::between_records;
			}
			else if(csv_traits::is_double_quote(c))
			{
				csv_traits::append(last_value_, c);
				state_ = state::quoted_record;
			}
			else if(csv_traits::is_new_line(c))
			{
				call_field_handler();
				call_end_line_handler();
				state_ = state::new_line;
			}
			else
				call_error_handler(csv_error::malformed_quoted_string);
			break;
		}
		case state::between_records:
		{
			if(csv_traits::is_separator(c))
			{
				call_field_handler();
				state_ = state::between_records;
			}
			else if(csv_traits::is_double_quote(c))
				state_ = state::quoted_record;
			else if(csv_traits::is_new_line(c))
			{
				call_field_handler();
				call_end_line_handler();
				state_ = state::new_line;
			}
			else
			{
				csv_traits::append(last_value_, c);
				state_ = state::record;
			}
			break;
		}
		case state::comment:
		{
			if(csv_traits::is_new_line(c))
			{
				call_comment_handler();
				state_ = state::new_line;
				update_line_counter();
			}
			else
				csv_traits::append(last_value_, c);
			break;
		}
		case state::quoted_record:
		{
			if(csv_traits::is_double_quote(c))
				state_ = state::quote_quoted_record;
			else
			{
				if(csv_traits::is_new_line(c))
					update_line_counter();
				csv_traits::append(last_value_, c);
			}
			break;
		}
	}
}


template<typename csv_traits>
void csv_parser<csv_traits>::end_of_data()
{
    switch(state_)
    {
        case state::quote_quoted_record:
        case state::record:
        case state::between_records:
			call_field_handler();
			call_end_line_handler();
            break;
        case state::quoted_record:
			call_error_handler(csv_error::unterminated_quoted_string);
            break;
        case state::new_line:
            break;
        case state::comment:
			call_comment_handler();
            break;
        case state::initial:
			call_error_handler(csv_error::error_empty_file);
            break;
    }
	if(end_file_handler && error_ == csv_error::no_error)
	{
		if(!end_file_handler())
			error_ = csv_error::error_user_aborted;
	} 
}

template<typename csv_traits>
bool csv_parser<csv_traits>::complete() const
{
    return complete_;
}

template<typename csv_traits>
int csv_parser<csv_traits>::current_line() const
{
    return current_line_;
}

template<typename csv_traits>
int csv_parser<csv_traits>::current_position() const
{
    return current_pos_;
}

template<typename csv_traits>
bool csv_parser<csv_traits>::error() const
{
	return error_ != csv_error::no_error;
}

}
#endif // DVP_CSV_PARSER_H
