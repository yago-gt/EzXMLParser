#include "../EzXMLParser.hpp"
#include <string>
#include <fstream>
#include <streambuf>
using namespace std;

int main()
{
	ifstream file_stream("example_page.html");
	string input_doc;

	file_stream.seekg(0, ios::end);
	input_doc.reserve(file_stream.tellg());
	file_stream.seekg(0, ios::beg);

	input_doc.assign(istreambuf_iterator<char>((file_stream)), istreambuf_iterator<char>());

	ez::XMLParser xmlparser(input_doc);

	auto table = xmlparser.tags_by_attr["id"]["main-contact"][0];

	for (const auto &row : table->children)
	{
		for (const auto &item : row->children)
		{
			cout << item->text << "\t";
		}
		cout << "\n";
	}
}