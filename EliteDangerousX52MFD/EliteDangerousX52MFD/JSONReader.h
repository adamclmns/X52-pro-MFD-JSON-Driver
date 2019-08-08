#pragma once
#include "stdafx.h"
#include "DepInclude\json.hpp"
#include "displaydata.h"

namespace reader
{
	using json = nlohmann::json;

	/*
		Read the provided JSON file into an mfdData structure
	*/
	std::unique_ptr<mfdData> ReadJSONFile(std::string filename);

}
