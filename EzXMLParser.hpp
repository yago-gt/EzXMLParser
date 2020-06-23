#ifndef EZXMLPARSER
#define EZXMLPARSER

#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <regex>
#include <string_view>
#include <cstdint>

namespace ez
{

	typedef struct XMLTag
	{
		bool is_closed;
		std::string_view name;
		std::unordered_map<std::string_view, std::string_view> attrs;
		std::string_view text;
		XMLTag *parent = NULL;
		std::vector<XMLTag *> children;

		XMLTag(std::string_view &tag_s);
	} XMLTag;


	typedef struct XMLParser
	{
		std::string doc;
		std::vector<XMLTag> tags;
		std::unordered_map<std::string_view, std::vector<XMLTag *>> tags_by_name;
		std::unordered_map<std::string_view, std::unordered_map<std::string_view, std::vector<XMLTag *>>> tags_by_attr;

		XMLParser(const std::string &_doc);
	} XMLParser;

} // namespace ez

#endif