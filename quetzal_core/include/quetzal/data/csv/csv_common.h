#ifndef DVP_CSV_COMMON_H
#define DVP_CSV_COMMON_H

namespace dvp
{

enum class csv_token {
    other = 0,
    separator,
    new_line,
    double_quote,
    start_comment
};

enum class csv_error {
	no_error = 0,
	error_other,
	error_user_aborted,
	malformed_quoted_string,
	unterminated_quoted_string,
	error_empty_file
};

}
#endif // DVP_CSV_COMMON_H
