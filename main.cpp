// #include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
// #include <crtdbg.h>

using namespace std;

string construct_start_tag_name(const string& start_tag_name, const string& prefix = "<", const string& postfix = ">")
{
	const auto csstn{prefix + start_tag_name + postfix};

	// char buffer[1024];
	// sprintf_s(buffer, 1024, "%s%s%s", prefix.c_str(), start_tag_name.c_str(), postfix.c_str());
	// return string{ buffer };

	return csstn;
}

string construct_end_tag_name(const string& end_tag_name, const string& prefix = "</", const string& postfix = ">")
{
	const auto csetn{prefix + end_tag_name + postfix};

	// char buffer[1024];
	// sprintf_s(buffer, 1024, "%s%s%s", prefix.c_str(), end_tag_name.c_str(), postfix.c_str());
	// return string{ buffer };

	return csetn;
}

class hrml_attribute;

class hrml_tag
{
	string tag_name_;
	vector<hrml_attribute> tag_attributes_;
	vector<hrml_tag> children_tags_;

public:

	explicit hrml_tag(const string& tag_name, const vector<hrml_attribute>& tag_attributes = vector<hrml_attribute>{},
	                  const vector<hrml_tag>& children_tags = vector<hrml_tag>{})
		: tag_name_{tag_name}, tag_attributes_{tag_attributes}, children_tags_{children_tags}
	{
	}

	const string& get_tag_name() const noexcept
	{
		return tag_name_;
	}

	const vector<hrml_attribute>& get_tag_attributes() const noexcept
	{
		return tag_attributes_;
	}

	const vector<hrml_tag>& get_children_tags() const noexcept
	{
		return children_tags_;
	}

	void set_tag_name(const string& tag_name)
	{
		tag_name_ = tag_name;
	}

	void set_tag_attributes(const vector<hrml_attribute>& tag_attributes)
	{
		tag_attributes_ = tag_attributes;
	}

	void set_children_tags(const vector<hrml_tag>& children_tags)
	{
		children_tags_ = children_tags;
	}
};

class hrml_attribute
{
	string attribute_name_;
	string attribute_value_;

public:

	explicit hrml_attribute(string attribute_name, string attribute_value) :
		attribute_name_{move(attribute_name)}, attribute_value_{move(attribute_value)}
	{
	}

	const string& get_attribute_name() const
	{
		return attribute_name_;
	}

	const string& get_attribute_value() const
	{
		return attribute_value_;
	}

	void set_attribute_name(string attribute_name)
	{
		attribute_name_ = move(attribute_name);
	}

	void set_attribute_value(string attribute_value)
	{
		attribute_value_ = move(attribute_value);
	}
};

class hrml_tag_position
{
	string hrml_tag_name_;
	size_t start_tag_pos_;
	size_t end_tag_pos_;
	bool is_end_tag_;
	size_t nesting_level_{};
	vector<hrml_attribute> attributes_{};

public:

	hrml_tag_position(const string& hrml_tag_name, const size_t start_pos, const size_t end_pos = string::npos, const bool is_end_tag = false) :
		hrml_tag_name_{ hrml_tag_name }, start_tag_pos_{ start_pos }, end_tag_pos_{ end_pos }, is_end_tag_{ is_end_tag } { }
	

	// hrml_tag_position getters
	const string& get_hrml_tag_name() const noexcept { return hrml_tag_name_; }
	size_t get_hrml_start_tag_position() const noexcept { return start_tag_pos_; }
	size_t get_hrml_end_tag_position() const noexcept { return end_tag_pos_; }
	bool get_is_end_tag() const noexcept { return is_end_tag_; }
	size_t get_hrml_tag_nesting_level() const noexcept { return nesting_level_; }	
	const vector<hrml_attribute>& get_hrml_tag_attributes() const noexcept { return attributes_; }

	// hrml_tag_position setters
	void set_hrml_tag_name(const string& hrml_tag_name) { hrml_tag_name_ = hrml_tag_name; }
	void set_hrml_start_tag_position(const size_t start_pos) { start_tag_pos_ = start_pos; }
	void set_hrml_end_tag_position(const size_t end_pos) { end_tag_pos_ = end_pos; }
	void set_is_end_tag(const bool is_end_tag) { is_end_tag_ = is_end_tag; }
	void set_hrml_tag_nesting_level(const size_t nesting_level) { nesting_level_ = nesting_level; }
	void set_hrml_tag_attributes(vector<hrml_attribute> attributes) { attributes_ = move(attributes); }

};

string trim(const string& str)
{
	size_t begin{};
	auto end{str.size() - 1};

	if (0u == str.length()) return string{};

	for (; begin <= end; ++begin)
	{
		if (!isspace(str[begin])) break;
	}

	if (begin > end) return string{};

	for (; end > begin; --end)
	{
		if (!isspace(str[end])) break;
	}

	return str.substr(begin, end - begin + 1);
}

vector<string> split(const string& source, const char* needle, size_t const max_count = string::npos)
{
	vector<string> parts{};

	size_t prev = 0, current;

	string needle_st{needle};

	const auto source_len{source.length()};

	const auto needle_len{needle_st.size()};

	if ((0u == source_len) || (0u == needle_len) || (needle_len >= source_len)) return parts;

	size_t number_of_parts = 0;

	do
	{
		current = source.find(needle_st, prev);

		if (string::npos == current) break;

		number_of_parts++;

		if ((string::npos != max_count) && (parts.size() == max_count)) break;

		if ((current - prev) > 0) parts.emplace_back(source.substr(prev, current - prev));

		prev = current + needle_len;

		if (prev >= source_len) break;
	}
	while (string::npos != current);

	if (number_of_parts > 0 && prev < source_len)
	{
		if (string::npos == max_count) parts.emplace_back(source.substr(prev));

		else if ((string::npos != max_count) && (parts.size() < max_count)) parts.emplace_back(source.substr(prev));
	}

	return parts;
}

template <typename T>
bool has_key(const T& container, const typename T::key_type& key)
{
	return (container.find(key) != cend(container));
}

template <typename T>
bool has_key(T& container, const typename T::key_type& key)
{
	return (container.find(key) != end(container));
}

vector<hrml_tag> hrml_doc_tree{};

bool check_is_hrml_document_correctly_structured(const string&, unordered_map<string, size_t>&,
                                                 unordered_map<string, size_t>&);
bool parse_hrml_document_structure(const string&, vector<hrml_tag_position>&);

vector<hrml_attribute> parse_hrml_tag_attributes(string hrml_tag_data);

string process_query(const string&, vector<hrml_tag_position>&);

int main()
{
	int n{}, q{};
	
	// ifstream input(R"(G:\Programming\input02.txt)", ios_base::in);
	// if (!input) return 1;
	// input >> n;

	cin >> n;
	if (n < 1) return 2;

	// input >> q;
	// cin >> q;
	if (q < 1) return 3;

	string hrml_data_line{};
	string hrml_document{};

	// getline(input, hrml_data_line);
	getline(cin, hrml_data_line);

	for (auto i = 0; i < n; i++)
	{
		// getline(input, hrml_data_line);
		getline(cin, hrml_data_line);
		hrml_document += trim(hrml_data_line);
	}

	unordered_map<string, size_t> unique_start_tag_count{};
	unordered_map<string, size_t> unique_end_tag_count{};

	if (!check_is_hrml_document_correctly_structured(hrml_document, unique_start_tag_count, unique_end_tag_count))
	{
		cerr << "\nInput HRML document lines comprise an invalid HRML document structure!\n"
			<< "Please, make sure that the logical and semantical structure of your HRML is correct."
			<< endl;

		return -1;
	}

	vector<hrml_tag_position> hrml_tag_positions{};

	if (!parse_hrml_document_structure(hrml_document, hrml_tag_positions))
	{
		cerr << "\nFailed to parse logical structure of specified HRML document!\n";
		return 2;
	}
	string hrml_query_data_line{};
	vector<string> queries{};

	for (auto i = 0; i < q; i++)
	{
		// getline(input, hrml_query_data_line);
		getline(cin, hrml_query_data_line);
		queries.emplace_back(trim(hrml_query_data_line));
	}

	// input.close();

	for (const auto& query : queries)
	{
		cout << process_query(query, hrml_tag_positions) << '\n';
	}

	return 0;
}

bool check_is_hrml_document_correctly_structured(const string& hrml_document,
                                                 unordered_map<string, size_t>& unique_start_tag_count,
                                                 unordered_map<string, size_t>& unique_end_tag_count)
{
	auto end_tag_pos{hrml_document.length() - 1};

	auto const start_tag_count = [&]()
	{
		size_t first_start_tag_pos{};

		unsigned long number_of_start_tags{};

		while ((first_start_tag_pos = hrml_document.find('<', first_start_tag_pos)) != string::npos)
		{
			if (((first_start_tag_pos + 1) < hrml_document.length()) && (hrml_document[first_start_tag_pos + 1] != '/'))
			{
				number_of_start_tags++;

				// <a><b size = "20><c height = "24"></c></b></a>
				end_tag_pos = first_start_tag_pos + 1;

				const auto ecp1{ hrml_document.find(' ', end_tag_pos) };
				const auto ecp2{ hrml_document.find('>', end_tag_pos) };
				end_tag_pos = min(ecp1, ecp2);

				if (end_tag_pos != string::npos)				
				{
					auto start_tag_name{hrml_document.substr(first_start_tag_pos + 1, end_tag_pos - (first_start_tag_pos + 1))};

					if (unique_start_tag_count.find(start_tag_name) == end(unique_start_tag_count))					
						unique_start_tag_count.insert(make_pair(start_tag_name, 1));					
					else
						unique_start_tag_count[start_tag_name]++;
					
					if (hrml_document[end_tag_pos] != '>')
					{
						end_tag_pos = hrml_document.find('>', end_tag_pos + 1);
					}

				} else return 0UL;				

				first_start_tag_pos = end_tag_pos + 1;
			}
			else
			{
				first_start_tag_pos += 4;
			}
		}

		return number_of_start_tags;
	}();

	auto const end_tag_count = [&]()
	{
		auto last_end_tag_pos { hrml_document.length() - 1 };
		unsigned long number_of_end_tags{};

		while ((last_end_tag_pos = hrml_document.rfind("</", last_end_tag_pos)) != string::npos)
		{
			number_of_end_tags++;

			end_tag_pos = last_end_tag_pos + 2;

			if ((end_tag_pos = hrml_document.find('>', end_tag_pos)) != string::npos)
			{
				auto end_tag_name{hrml_document.substr(last_end_tag_pos + 2, end_tag_pos - (last_end_tag_pos + 2))};

				if (unique_end_tag_count.find(end_tag_name) == end(unique_end_tag_count))
					unique_end_tag_count.insert(make_pair(end_tag_name, 1));				
				else
					unique_end_tag_count[end_tag_name]++;
				
			} else return 0UL;			
			
			last_end_tag_pos -= 4;
		}

		return number_of_end_tags;

	}();

	if (start_tag_count != end_tag_count) return false;

	for (const auto& hrml_tag : unique_start_tag_count)
	{
		if (has_key(unique_end_tag_count, hrml_tag.first))
		{
			if (unique_start_tag_count[hrml_tag.first] == unique_end_tag_count[hrml_tag.first]) continue;

			return false;
		}

		return false;
	}

	return true;
}

bool parse_hrml_document_structure(const string& hrml_document, vector<hrml_tag_position>& hrml_document_tree)
{

	string start_tag_name{}, end_tag_name{};

	const size_t hrml_document_length { hrml_document.length() };

	size_t first_start_tag_pos{};

	while ((first_start_tag_pos < hrml_document_length) && (first_start_tag_pos = hrml_document.find('<', first_start_tag_pos)) != string::npos)
	{
		if (hrml_document[first_start_tag_pos + 1] != '/')
		{
			size_t end_tag_pos { first_start_tag_pos + 1 };

			const auto ecp1{ hrml_document.find(' ', end_tag_pos) };
			const auto ecp2{ hrml_document.find('>', end_tag_pos) };
			end_tag_pos = min(ecp1, ecp2);

			if (end_tag_pos != string::npos)
			{	
				const size_t start_tag_end = (hrml_document[end_tag_pos] != '>') ? hrml_document.find('>', end_tag_pos) : end_tag_pos;

				start_tag_name = hrml_document.substr(first_start_tag_pos + 1, end_tag_pos - (first_start_tag_pos + 1));

					hrml_document_tree.emplace_back(start_tag_name, first_start_tag_pos);

				if ((start_tag_end != string::npos) && (start_tag_end != end_tag_pos))
				{	
					const auto attribute_line{ hrml_document.substr(end_tag_pos, start_tag_end - end_tag_pos) };
					const auto attributes = parse_hrml_tag_attributes(move(attribute_line));
					hrml_document_tree.back().set_hrml_tag_attributes(move(attributes));
				}								

				first_start_tag_pos = (start_tag_end + 1);
			}
			else {
				return false;
			}
		}
		else
		{
			const size_t end_tag_end{ hrml_document.find('>', first_start_tag_pos + 2) };

			if (end_tag_end == string::npos) return false;

			end_tag_name = hrml_document.substr(first_start_tag_pos + 2, end_tag_end - (first_start_tag_pos + 2));

			hrml_document_tree.emplace_back(end_tag_name, first_start_tag_pos);
			
			hrml_document_tree.back().set_is_end_tag(true);

			first_start_tag_pos = end_tag_end + 1;
		}
	}
	
	size_t current_level{ 1u };
	hrml_document_tree.front().set_hrml_tag_nesting_level(current_level);

	for (size_t i{1u}; i < hrml_document_tree.size(); i++)
	{
		if (hrml_document_tree[i].get_is_end_tag())
		{
			hrml_document_tree[i].set_hrml_tag_nesting_level(current_level);
			current_level--;
			continue;
		} 
		
		current_level++;
		hrml_document_tree[i].set_hrml_tag_nesting_level(current_level);

	}

	stable_sort(begin(hrml_document_tree), end(hrml_document_tree), [](const hrml_tag_position& lhs, const hrml_tag_position& rhs)
	{
		return (lhs.get_hrml_tag_nesting_level() < rhs.get_hrml_tag_nesting_level());

	});

	return true;
}

vector<hrml_attribute> parse_hrml_tag_attributes(string hrml_tag_data)
{
	vector<hrml_attribute> hrml_tag_attributes{};

	auto attributes_line { move(hrml_tag_data) };

	auto start_pos = attributes_line.find_first_not_of(" \t\n\f\v\r<>");
	
	while (start_pos != string::npos)
	{
		auto end_pos = attributes_line.find_first_of(" \t=", start_pos + 1);

		if (end_pos == string::npos) return hrml_tag_attributes;

		const auto hrml_tag_attribute_name{ trim(attributes_line.substr(start_pos, end_pos - start_pos)) };		

		start_pos = attributes_line.find('"', end_pos + 1);

		if (start_pos == string::npos) return hrml_tag_attributes;
		
		start_pos++;

		end_pos = attributes_line.find('"', start_pos);

		if (end_pos == string::npos) return hrml_tag_attributes;

		const auto hrml_tag_attribute_value{ attributes_line.substr(start_pos, end_pos - start_pos) };
		
		hrml_tag_attributes.emplace_back(hrml_tag_attribute_name, hrml_tag_attribute_value);
		
		attributes_line.assign(attributes_line.substr(end_pos + 1)); // " "

		start_pos = attributes_line.find_first_not_of(" \t\n\f\v\r<>"); // -> string::npos

	}

	return hrml_tag_attributes;
}

string process_query(const string& query, vector<hrml_tag_position>& hrml_tags)
{
	if (query.find('.') == string::npos)
	{
		size_t start{0u}, last;

		if ((last = query.find('~', start)) != string::npos)
		{
			const auto tag_name { trim(query.substr(start, last - start)) };

			start = last + 1;

			const auto attribute_name { trim(query.substr(start)) };

			if (hrml_tags.empty())
			{
				return "Not Found!";
			}

			for (const auto& hrml_tag : hrml_tags)
			{			

				if ((hrml_tag.get_hrml_tag_name() == tag_name) && (hrml_tag.get_hrml_tag_nesting_level() == 1) && !hrml_tag.get_is_end_tag())
				{
					const auto hrml_tag_attributes = hrml_tag.get_hrml_tag_attributes();

					for (const auto& attribute : hrml_tag_attributes)
					{
						if (attribute_name == attribute.get_attribute_name()) return attribute.get_attribute_value();
					}
				}
			}
			
			return "Not Found!";

		}

		return "Not Found!";
	}

	size_t start{0u}, last, level{};

	while ((last = query.find('.', start)) != string::npos)
	{
		const auto tag_name{query.substr(start, last - start)};

		level++;

		bool is_parent_tag_found{};

		for (const auto& hrml_tag : hrml_tags)
		{

			if ((hrml_tag.get_hrml_tag_name() == tag_name) && (hrml_tag.get_hrml_tag_nesting_level() == level) && !hrml_tag.get_is_end_tag())
			{
				is_parent_tag_found = true;
				break;
			}
		}

		if (!is_parent_tag_found) return "Not Found!";

		start = last + 1;

	}

	if ((last = query.find('~', start)) != string::npos)
	{
		const auto tag_name { query.substr(start, last - start) };

		level++;

		auto child_tag_index{ string::npos };

		bool is_child_tag_found{};		

		for (size_t i{}; i < hrml_tags.size(); i++)
		{

			if ((hrml_tags[i].get_hrml_tag_name() == tag_name) && (hrml_tags[i].get_hrml_tag_nesting_level() == level) && !hrml_tags[i].get_is_end_tag())
			{
				child_tag_index = i;
				is_child_tag_found = true;
				break;
			}
		}

		if (!is_child_tag_found) return "Not Found!";

		start = last + 1;

		const auto attribute_name { trim(query.substr(start)) };

		const auto& child_tag_attributes = hrml_tags[child_tag_index].get_hrml_tag_attributes();

		for (const auto& ta : child_tag_attributes)
		{
			if (ta.get_attribute_name() == attribute_name)
			{
				return ta.get_attribute_value();
			}
		}

		return "Not Found!";
	}

	return "Not Found!";
}
