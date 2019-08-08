#include "stdafx.h"
#include "JSONReader.h"
namespace reader
{
	std::unique_ptr<mfdData> ReadJSONFile(std::string filename)
	{
		std::ifstream infile(filename);
		if (!infile.good())
		{
			std::cout << "Failed to open JSON file: " << filename << std::endl;
			ExitProcess(EXIT_FAILURE);
		}

		json json;
		infile >> json;

		auto data = std::make_unique<mfdData>();

		auto pages = json["pages"].get<std::vector<json::object_t>>();
		for (auto& page : pages)
		{
			mdfDataPage mdfPage;
			auto lines = page["lines"].get<std::vector<std::string>>();
			for (auto& line : lines)
			{
				mdfPage.lines.push_back(line);
			}

			data->pages.push_back(mdfPage);
		}

		return data;
	}

}
