#pragma once
#include "stdafx.h"


struct mdfDataPage
{
	std::vector<std::string> lines;
};

struct mfdData
{
	std::vector<mdfDataPage> pages;
};
