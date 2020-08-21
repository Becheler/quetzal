#ifndef DVP_CSV_DATA_H
#define DVP_CSV_DATA_H

#include <vector>
#include <string>
#include <boost/optional.hpp>

namespace dvp
{

class csv_data;

class csv_record
{
friend class csv_data;
	csv_data const& owner;
	std::vector<std::string> fields_;
	

	void add_field(std::string const& field) {
		fields_.push_back(field);
	}
	
public:	
	csv_record(csv_data const& owner_) : owner(owner_) {}
	typedef std::vector<std::string>::const_iterator const_iterator;
	const_iterator begin() const { return fields_.begin(); }
	const_iterator end() const { return fields_.end(); }

	std::string const& operator[](size_t i) const { return fields_[i]; }
	size_t size() const { return fields_.size(); }

	boost::optional<std::string const&> operator[](std::string const& header) const;
};

class csv_data
{
friend class csv_data_handler;

	std::vector<csv_record> records_;
	std::vector<std::string> headers_;
	void new_record() { records_.emplace_back(*this); };
	void add_field(std::string const& field) { records_.back().add_field(field); };
public:
	typedef std::vector<csv_record>::const_iterator const_iterator;
	const_iterator begin() const { return records_.begin(); }
	const_iterator end() const { return records_.end(); }
	csv_record const& operator[](size_t i) { return records_[i]; }
	size_t size() const { return records_.size(); }	
	std::vector<std::string> const& headers() const { return headers_; }
};

boost::optional<std::string const&> csv_record::operator[](std::string const& header) const
{
	for(size_t i = 0; i < owner.headers().size(); ++i)
	{
		if(owner.headers()[i] == header)
			return fields_[i];
	}
	return boost::optional<std::string const&>();
}


class csv_data_handler {
	bool has_headers_;
	csv_data& data_;
	bool at_beginning_of_line;
	bool at_first_line;
public:
	bool field_handler(std::string const& s)
	{
		if(at_first_line && has_headers_)
		{
			data_.headers_.push_back(s);
		}
		else
		{
			if(at_first_line)
				data_.headers_.push_back(std::string("header") + std::to_string(data_.headers_.size()));
			if(at_beginning_of_line)
			{
				data_.new_record();
				at_beginning_of_line = false;
			}
			data_.add_field(s);
		}
		return true;
	}
	bool end_line_handler()
	{
		at_beginning_of_line = true;
		at_first_line = false;
		return true;
	}

	template<typename parser_>
	csv_data_handler(csv_data& data, bool has_header_line, parser_& parser) :
		data_(data),
		has_headers_(has_header_line),
		at_beginning_of_line(true),
		at_first_line(true)
	{
		parser.field_handler = [this](std::string const& s) { return this->field_handler(s); };
		parser.end_line_handler = [this]() { return this->end_line_handler(); };
	}
		
};
}

#endif
