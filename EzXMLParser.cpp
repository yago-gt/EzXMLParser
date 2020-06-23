#include "EzXMLParser.hpp"
using namespace ez;

XMLTag::XMLTag(std::string_view &tag_s)
{
	tag_s = tag_s.substr(1, tag_s.size() - 2); // remove < and >
	std::vector<std::string_view> tokens;
	int token_start = 0;
	for (int i = 0; i < tag_s.size(); ++i)
	{
		if (isspace(tag_s[i]))
		{
			if (i != token_start)
			{
				tokens.emplace_back(&tag_s[token_start], i - token_start);
			}
			token_start = i + 1;
		}
		else
		{
			if (tag_s[i] == '\'' || tag_s[i] == '"')
			{
				auto close_quote_pos = tag_s.find_first_of(std::string_view(&tag_s[i], 1), i + 1);
				// skip if there is a escape character
				while (close_quote_pos != std::string_view::npos && tag_s[close_quote_pos - 1] == '\\')
				{
					close_quote_pos = tag_s.find_first_of(std::string_view(&tag_s[i], 1), close_quote_pos + 1);
				}
				if (close_quote_pos == std::string_view::npos)
				{
					tokens.emplace_back(&tag_s[token_start], i - token_start - 1);
					++i;
					token_start = i;
					i = tag_s.size() - 1;
					tokens.emplace_back(&tag_s[token_start], i - token_start - 1);
					token_start = i;
					break;
				}
				else
				{
					tokens.emplace_back(&tag_s[token_start], i - token_start - 1);
					++i;
					token_start = i;
					i = close_quote_pos + 1;
					tokens.emplace_back(&tag_s[token_start], i - token_start - 1);
					token_start = i + 1;
					if (i == tag_s.size() - 1)
						break;
				}
			}
		}
	}

	if (token_start < tag_s.size())
	{
		tokens.emplace_back(&tag_s[token_start], tag_s.size() - token_start);
	}

	// add name
	name = tokens[0];

	// add attributes
	for (int i = 1; i < tokens.size() - 1; i += 2)
	{
		attrs[tokens[i]] = tokens[i + 1];
	}
}


XMLParser::XMLParser(const std::string &_doc)
{
	doc = _doc;

	std::regex tag_regex("<[^!].*?>");
	std::sregex_iterator regex_it(doc.begin(), doc.end(), tag_regex);
	std::vector<std::string_view> tokens;

	for (regex_it; regex_it != std::sregex_iterator(); ++regex_it)
	{
		auto prefix = std::string_view(&*(regex_it->prefix().first), regex_it->prefix().length());
		auto infix = std::string_view(&*(doc.begin() + regex_it->position()), regex_it->length());

		if (!std::all_of(prefix.begin(), prefix.end(), isspace) && (prefix[0] != '<' && prefix[1] != '!'))
		{ // remove comments and just spaces
			tokens.push_back(prefix);
		}

		tokens.push_back(infix);
	}

	tags.reserve(tokens.size());
	std::vector<XMLTag *> tag_stack;
	for (auto &token : tokens)
	{
		if (token.size() >= 2)
		{
			// CLOSE TAG
			if (token[0] == '<' && token[1] == '/')
			{
				auto close_tag_name = token.substr(2, token.size() - 3); // remove </ and > from tag

				for (int i = tag_stack.size() - 1; i >= 0; --i)
				{

					// check if opening tag exist
					if (tag_stack[i]->name == close_tag_name)
					{
						tag_stack[i]->is_closed = true;

						// reparent and pop from stack in between
						while (tag_stack.size() - 1 > i)
						{
							tag_stack.back()->parent = tag_stack[i];
							tag_stack[i]->children.push_back(tag_stack.back());
							tag_stack.pop_back();
						}

						std::reverse(tag_stack[i]->children.begin(), tag_stack[i]->children.end());
						break;
					}
				}

				// NEW TAG
			}
			else if (token[0] == '<')
			{
				XMLTag *tag = &tags.emplace_back(token);
				tag_stack.push_back(tag);

				// map tags by name
				// if name already exist
				if (tags_by_name.count(tag->name))
				{
					tags_by_name[tag->name].push_back(tag);
				}
				else
				{
					std::vector<XMLTag *> tag_name_vec;
					tag_name_vec.push_back(tag);
					tags_by_name[tag->name] = tag_name_vec;
				}

				// map tags by attributes
				for (const auto &[attr, value] : tag->attrs)
				{
					if (tags_by_attr.count(attr))
					{
						if (tags_by_attr[attr].count(attr))
						{
							tags_by_attr[attr][value].push_back(tag);
						}
						else
						{
							std::vector<XMLTag *> tag_attr_vec;
							tag_attr_vec.push_back(tag);
							tags_by_attr[attr][value] = tag_attr_vec;
						}
					}
					else
					{
						std::unordered_map<std::string_view, std::vector<XMLTag *>> tag_attr_unmap;
						tags_by_attr[attr] = tag_attr_unmap;

						std::vector<XMLTag *> tag_attr_vec;
						tag_attr_vec.push_back(tag);
						tags_by_attr[attr][value] = tag_attr_vec;
					}
				}

				// NOT A TAG (text)
			}
			else
			{
				tags.back().text = token;
			}
		}
		else
		{
			// concat with the text
			if (tags.back().text.size() > 0)
			{
				tags.back().text = std::string_view(&tags.back().text[0], tags.back().text.size() + token.size());
			}
			else
			{
				tags.back().text = token;
			}
		}
	}
}
