// JSONDataStructure.cpp
#include "stdafx.h"
#include "JSONDataStructure.h"

using namespace std;
using json = nlohmann::json;

typedef void(JSONDataStructure::*EventFunction)(json::object_t obj);
std::unordered_map<std::string, EventFunction> eventsMap;

/*
	PARAMETERS: std::string eventString -> determined event title from the journal
	RETURNS: none

	FUNCTION: Determines the correct function to execute based on the event string
*/
void JSONDataStructure::readStoreJSON(std::string eventString) 
{
	// String to JSON object and compare event
	json::object_t tempObj = json::parse(eventString.begin(), eventString.end());
	if (tempObj["event"].is_null() != true)
	{
		string event = tempObj["event"];
		auto it = eventsMap.find(event);
		if (it == eventsMap.end())
		{
			//cout << "Event: " << tempObj["event"] << " not in list.\n";
		}
		else
		{
			// If the event matches, call the associated function within the map
			(*this.*it->second)(tempObj);
		}
	}
}

/*
	PARAMETERS: none
	RETURNS: none

	FUNCTION: Adds entries to the unordered map on startup. By doing this, comparison of the string from the log, if true, will call the associated function and update the saved data.
*/
void JSONDataStructure::createMap()
{																						
	eventsMap.emplace("LoadGame", &JSONDataStructure::e_LoadGame);			
	eventsMap.emplace("Loadout", &JSONDataStructure::e_Loadout);			
	eventsMap.emplace("Cargo", &JSONDataStructure::e_Cargo);			
	eventsMap.emplace("Rank", &JSONDataStructure::e_Rank);								
	eventsMap.emplace("Docked", &JSONDataStructure::e_Docked);							
	eventsMap.emplace("DockingGranted", &JSONDataStructure::e_DockingGranted);			
	eventsMap.emplace("FSDJump", &JSONDataStructure::e_FSDJump);
	eventsMap.emplace("Liftoff", &JSONDataStructure::e_Liftoff);
	eventsMap.emplace("Location", &JSONDataStructure::e_Location);						
	eventsMap.emplace("SupercruiseEntry", &JSONDataStructure::e_SupercruiseEntry);		
	eventsMap.emplace("SupercruiseExit", &JSONDataStructure::e_SupercruiseExit);		
	eventsMap.emplace("Touchdown", &JSONDataStructure::e_Touchdown);					
	eventsMap.emplace("Undocked", &JSONDataStructure::e_Undocked);						
	eventsMap.emplace("Scan", &JSONDataStructure::e_Scan);								
	eventsMap.emplace("BuyExplorationData", &JSONDataStructure::e_BuyExplorationData);	
	eventsMap.emplace("SellExplorationData", &JSONDataStructure::e_SellExplorationData);
	eventsMap.emplace("BuyTradeData", &JSONDataStructure::e_BuyTradeData);				
	eventsMap.emplace("MarketBuy", &JSONDataStructure::e_MarketBuy);					
	eventsMap.emplace("MarketSell", &JSONDataStructure::e_MarketSell);					
	eventsMap.emplace("BuyAmmo", &JSONDataStructure::e_BuyAmmo);						
	eventsMap.emplace("BuyDrones", &JSONDataStructure::e_BuyDrones);					
	eventsMap.emplace("CommunityGoalReward", &JSONDataStructure::e_CommunityGoalReward);
	eventsMap.emplace("CrewHire", &JSONDataStructure::e_CrewHire);						
	eventsMap.emplace("FetchRemoteModule", &JSONDataStructure::e_FetchRemoteModule);	
	eventsMap.emplace("MissionCompleted", &JSONDataStructure::e_MissionCompleted);		
	eventsMap.emplace("ModuleBuy", &JSONDataStructure::e_ModuleBuy);				
	eventsMap.emplace("ModuleSell", &JSONDataStructure::e_ModuleSell);			
	eventsMap.emplace("ModuleSellRemote", &JSONDataStructure::e_ModuleSellRemote);		
	eventsMap.emplace("PayFines", &JSONDataStructure::e_PayFines);						
	eventsMap.emplace("PayLegacyFines", &JSONDataStructure::e_PayLegacyFines);			
	eventsMap.emplace("RedeemVoucher", &JSONDataStructure::e_RedeemVoucher);			
	eventsMap.emplace("RefuelAll", &JSONDataStructure::e_RefuelAll);				
	eventsMap.emplace("RefuelPartial", &JSONDataStructure::e_RefuelPartial);		
	eventsMap.emplace("Repair", &JSONDataStructure::e_Repair);			
	eventsMap.emplace("RepairAll", &JSONDataStructure::e_RepairAll);
	eventsMap.emplace("RestockVehicle", &JSONDataStructure::e_RestockVehicle);			
	eventsMap.emplace("SellDrones", &JSONDataStructure::e_SellDrones);					
	eventsMap.emplace("ShipyardBuy", &JSONDataStructure::e_ShipyardBuy);				
	eventsMap.emplace("ShipyardSell", &JSONDataStructure::e_ShipyardSell);			
	eventsMap.emplace("ShipyardTransfer", &JSONDataStructure::e_ShipyardTransfer);		
	eventsMap.emplace("ShipyardSwap", &JSONDataStructure::e_ShipyardSwap);				
	eventsMap.emplace("PowerplayFastTrack", &JSONDataStructure::e_PowerplayFastTrack);	
	eventsMap.emplace("PowerplaySalary", &JSONDataStructure::e_PowerplaySalary);	
	eventsMap.emplace("Continued", &JSONDataStructure::e_Continued);				
	eventsMap.emplace("DockFighter", &JSONDataStructure::e_DockFighter);				
	eventsMap.emplace("DockSRV", &JSONDataStructure::e_DockSRV);						
	eventsMap.emplace("LaunchFighter", &JSONDataStructure::e_LaunchFighter);			
	eventsMap.emplace("LaunchSRV", &JSONDataStructure::e_LaunchSRV);					
	eventsMap.emplace("Promotion", &JSONDataStructure::e_Promotion);					
	eventsMap.emplace("VehicleSwitch", &JSONDataStructure::e_VehicleSwitch);			
}

std::unique_ptr<JSONDataStructure::mdfPage> JSONDataStructure::getCmdrPage()
{

	auto page = make_unique<JSONDataStructure::mdfPage>();

	page->currentLine = cmdr.currentLine;
	page->lines.push_back(L"Greetings CMDR");
	page->lines.push_back(strToWStr(cmdr.name));
	page->lines.push_back(strToWStr(cmdr.ship));
	page->lines.push_back(strToWStr("CR: " + to_string(cmdr.creditBalance)));
	
	page->lines.push_back(strToWStr("COM: " + cmdr.combatRank));
	page->lines.push_back(strToWStr("TRA: " + cmdr.tradeRank));
	page->lines.push_back(strToWStr("EXP: " + cmdr.explorationRank));

	page->lines.push_back(strToWStr("EMP: " + cmdr.empire));
	page->lines.push_back(strToWStr("FED: " + cmdr.federation));
	page->lines.push_back(strToWStr("CQC: " + cmdr.cqc));

	return page;
}

std::unique_ptr<JSONDataStructure::mdfPage> JSONDataStructure::getLocationPage()
{

	auto page = make_unique<JSONDataStructure::mdfPage>();

	page->currentLine = loc.currentLine;
	for (auto const& line : loc.lines)
	{
		page->lines.push_back(line);
	}

	return page;
}

std::unique_ptr<JSONDataStructure::mdfPage> JSONDataStructure::getExplorationPage()
{

	auto page = make_unique<JSONDataStructure::mdfPage>();

	page->currentLine = expl.currentLine;
	for (auto const& line : expl.lines)
	{
		page->lines.push_back(line);
	}
	if (page->lines.size() == 0) {
		page->lines.push_back(L"No Exploration Data");
	}

	return page;
}

std::unique_ptr<JSONDataStructure::mdfPage> JSONDataStructure::getCargoPage()
{
	auto page = make_unique<JSONDataStructure::mdfPage>();
	page->currentLine = cargo.currentLine;

	page->lines.push_back(L"Cargo: " + to_wstring(cargo.currentCargo) + L"/" + to_wstring(cargo.capacity));

	for (auto const& item : cargo.store)
	{
		page->lines.push_back(strToWStr(item.first) + L": " + to_wstring(item.second));
	}

	return page;
}


/*
	PARAMETERS: string str -> string value
	RETURNS: wstring

	FUNCTION: Takes a string value(from the json file) and converts it to a wstring. Upon returning from this function the wstring value will be copied into the relevant page array. Usually as a wchar_t since the printing function to the controller requires a wchar_t type.
*/
std::wstring JSONDataStructure::strToWStr(std::string str)
{
	wstring wStr = wstring(str.begin(), str.end());
	return wStr;
}


std::string JSONDataStructure::formattedShipName(std::string ship)
{
	return ship; // already seems to be pretty formatted to me
}

/*
	Each function will take the full JSON object event associated to the correct event key value. Hopefully each of the associated key values are self explanatory and can be easily determined.
	For easy lookup, each event function is prefixed with 'e_' and the duplicated function name from the journal manual is listed after.

	Not all parts of the function I found relevant yet so some functions may be short and only some of the associated key values are used.

	Each function does key value checks to see if it even exists. Most of the time this might be redundant but I'd rather have value checking saftey instead of the program crashing. I haven't noticed a performance impact either.
	Once the key value is determined safe and exists, the value is taken from it and stored in the correct location.
*/

void JSONDataStructure::e_LoadGame(json::object_t obj)
{
	// Key value checking if it exists otherwise, will crash
	if (obj["Commander"].is_null() != true) {
		cmdr.name = obj["Commander"].get<std::string>();
	}
	else
	{
		cmdr.name = "No Name";
	}

	if (obj["Ship"].is_null() != true)
	{
		cmdr.ship = obj["Ship"].get<std::string>();
	}
	else
	{
		cmdr.name = "No Ship";
	}

	if (obj["Credits"].is_null() != true)
	{
		cmdr.creditBalance = (long long)obj["Credits"];
	}
}

void JSONDataStructure::e_Loadout(json::object_t obj)
{
	cargo.capacity = obj["CargoCapacity"];
}

void JSONDataStructure::e_Cargo(json::object_t obj)
{
	auto inventory = obj["Inventory"];
	if (inventory.is_null())
	{
		return;
	}

	

	for (auto& item : inventory)
	{
		std::string name;
		int amount = item["Count"];
		if(!item["Name_Localized"].is_null())
		{
			name = item["Name_Localized"].get<std::string>();
		}
		else
		{
			name = item["Name"].get<std::string>();
		}
		cargo.store.emplace(name, amount);
		cargo.currentCargo += amount;
	}
}

void JSONDataStructure::e_Rank(json::object_t obj)
{
	if (obj["Combat"].is_null() != true)
	{
		cmdr.combatRank = combatRank[obj["Combat"]];
	}
	else
	{
		cmdr.combatRank = "No Combat Rank";
	}

	if (obj["Trade"].is_null() != true)
	{
		cmdr.tradeRank = tradeRank[obj["Trade"]];
	}
	else
	{
		cmdr.tradeRank = "No Trade Rank";
	}

	if (obj["Explore"].is_null() != true)
	{
		cmdr.explorationRank = explorerRank[obj["Explore"]];
	}
	else
	{
		cmdr.explorationRank = "No Explore Rank";
	}

	if (obj["Empire"].is_null() != true)
	{
		cmdr.empire = empireRank[obj["Empire"]];
	}
	else
	{
		cmdr.empire = "No Empire Rank";
	}

	if (obj["Federation"].is_null() != true)
	{
		cmdr.federation = federationRank[obj["Federation"]];
	}
	else
	{
		cmdr.federation = "No Fed Rank";
	}

	if (obj["CQC"].is_null() != true)
	{
		cmdr.cqc = cqcRank[obj["CQC"]];
	}
	else
	{
		cmdr.cqc = "No CQC Rank";
	}

}

void JSONDataStructure::e_Docked(json::object_t obj)
{
	// Clear out the vector
	loc.lines.clear();

	// Reset currentLine
	loc.currentLine = 0;

	if (obj["StarSystem"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StarSystem"]).c_str());
	}

	if (obj["StationName"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StationName"]).c_str());
	}

	if (obj["StationType"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StationType"]).c_str());
	}

	loc.lines.push_back(strToWStr("Docked").c_str());

}

void JSONDataStructure::e_DockingGranted(json::object_t obj)
{
	// Store the starsystem as it will be needed after clearing
	wstring starsystem = loc.lines[0];

	// Clear vector and reset the current line
	loc.lines.clear();
	loc.currentLine = 0;

	// Push back the starsystem
	loc.lines.push_back(starsystem);

	// Get new info
	if (obj["StationName"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StationName"]).c_str());
	}

	if (obj["LandingPad"].is_null() != true)
	{
		loc.lines.push_back(strToWStr("LP -> " + to_string((int)obj["LandingPad"])).c_str());
	}
}

void JSONDataStructure::e_FSDJump(json::object_t obj)
{
	// Clear the vector and reset the currentline
	loc.lines.clear();
	loc.currentLine = 0;

	// Get new info
	if (obj["StarSystem"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StarSystem"]).c_str());
	}

	loc.lines.push_back(strToWStr("In Supercruise").c_str());

}

void JSONDataStructure::e_Liftoff(nlohmann::json::object_t obj)
{
	wstring starsystem = loc.lines[0];
	wstring body = loc.lines[1];

	loc.lines.clear();
	loc.currentLine = 0;

	loc.lines.push_back(starsystem);
	loc.lines.push_back(body);
}

void JSONDataStructure::e_Location(json::object_t obj)
{
	// Clear vector and reset the currentline
	loc.lines.clear();
	loc.currentLine = 0;

	// Get new info
	if (obj["StarSystem"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StarSystem"]).c_str());
	}

	// Determine docked status
	if (obj["Docked"].is_null() != true)
	{
		if (obj["Docked"])
		{
			// Get StationName and StationType since player is docked
			if (obj["StationName"].is_null() != true)
			{
				loc.lines.push_back(strToWStr(obj["StationName"]).c_str());
			}
			if (obj["StationType"].is_null() != true)
			{
				loc.lines.push_back(strToWStr(obj["StationType"]).c_str());
			}
			loc.lines.push_back(strToWStr("Docked").c_str());
		}
		else
		{
			// Not docked. Get Body and BodyType
			if (obj["Body"].is_null() != true)
			{
				loc.lines.push_back(strToWStr(obj["Body"]).c_str());
				std::string tempStr = obj["Body"];
				loc.lastKnownBody = tempStr;
			}
			if (obj["BodyType"].is_null() != true)
			{
				// BodyType can be null which could be a barycenter
				string bodyType = obj["BodyType"];
				if (bodyType.empty())
				{
					loc.lines.push_back(strToWStr("Barycenter").c_str());
				}
				else
				{
					loc.lines.push_back(strToWStr(bodyType).c_str());
				}
			}
		}
	}
}

void JSONDataStructure::e_SupercruiseEntry(json::object_t obj)
{
	// Clear vector and reset currentline
	loc.lines.clear();
	loc.currentLine = 0;

	// Get new data
	if (obj["StarSystem"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StarSystem"]).c_str());
		loc.lines.push_back(strToWStr("In Supercruise").c_str());
	}
}

void JSONDataStructure::e_SupercruiseExit(json::object_t obj)
{
	// Clear the vector and reset currentline
	loc.lines.clear();
	loc.currentLine = 0;

	// Get new data
	if (obj["StarSystem"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StarSystem"]).c_str());
	}

	if (obj["Body"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["Body"]).c_str());
		std::string tempStr = obj["Body"];
		loc.lastKnownBody = tempStr;
	}

	if (obj["BodyType"].is_null() != true)
	{
		// BodyType can be null which could be a barycenter. Might need testing if it is always a barycenter or it could just be space...
		string bodyType = obj["BodyType"];
		if (bodyType.empty())
		{
			loc.lines.push_back(strToWStr("Barycenter").c_str());
		}
		else
		{
			loc.lines.push_back(strToWStr(bodyType).c_str());
		}
	}
}

void JSONDataStructure::e_Touchdown(json::object_t obj)
{
	// Check to see if a starsystem has already been logged. (Vector should be populated) 
	// If there hasn't been a previously saved system, the player might have already started in SRV mode and the "Location" should be read next.
	// Otherwise, if the vector is not empty, get correct info.
	if (!loc.lines.empty())
	{
		// Get the starsystem
		wstring starsystem = loc.lines[0];

		// Clear vector and reset currentline
		loc.lines.clear();
		loc.currentLine = 0;

		// Write back starsystem and body
		loc.lines.push_back(starsystem);
		if (!loc.lastKnownBody.empty())
		{
			loc.lines.push_back(strToWStr(loc.lastKnownBody).c_str());
		}

		// Get new latitude and longitude data
		if (obj["Latitude"].is_null() != true)
		{
			stringstream ss;
			ss << fixed << setprecision(5) << (float)obj["Latitude"];
			if ((float)obj["Latitude"] < 0)
			{
				loc.lines.push_back(strToWStr("Lat:" + ss.str()).c_str());
			}
			else
			{
				loc.lines.push_back(strToWStr("Lat: " + ss.str()).c_str());
			}
		}

		if (obj["Longitude"].is_null() != true)
		{
			stringstream ss;
			ss << fixed << setprecision(5) << (float)obj["Longitude"];
			if ((float)obj["Longitude"] < 0)
			{
				loc.lines.push_back(strToWStr("Lon:" + ss.str()).c_str());
			}
			else
			{
				loc.lines.push_back(strToWStr("Lon: " + ss.str()).c_str());
			}
		}
	}
}

void JSONDataStructure::e_Undocked(json::object_t obj)
{
	// Get the starsystem
	wstring starsystem = loc.lines[0];

	// Clear the vector and reset currentline
	loc.lines.clear();
	loc.currentLine = 0;

	// Write back starsystem
	loc.lines.push_back(starsystem);

	// Get new data
	if (obj["StationName"].is_null() != true)
	{
		loc.lines.push_back(strToWStr(obj["StationName"]).c_str());
	}
}

void JSONDataStructure::e_Scan(json::object_t obj)
{
	// Clear the vector and reset the currentline
	expl.lines.clear();
	expl.currentLine = 0;

	// Get new data
	if (obj["BodyName"].is_null() != true)
	{
		expl.lines.push_back(strToWStr(obj["BodyName"]).c_str());
	}

	if (obj["StarType"].is_null() != true)
	{
		string temp0 = obj["StarType"];
		string temp1 = "Class: ";
		temp1 = temp1 + temp0;
		expl.lines.push_back(strToWStr(temp1).c_str());
	}

	if (obj["PlanetClass"].is_null() != true)
	{
		expl.lines.push_back(strToWStr(obj["PlanetClass"]).c_str());
	}

	if (obj["Landable"].is_null() != true)
	{
		bool isLandable = (bool)obj["Landable"];
		if (isLandable)
		{
			expl.lines.push_back(strToWStr("Landable").c_str());
		}
		else
		{
			expl.lines.push_back(strToWStr("Not Landable").c_str());
		}
	}

	if (obj["SurfaceGravity"].is_null() != true)
	{
		double gravity = (double)obj["SurfaceGravity"];
		gravity = gravity / 9.81;
		stringstream ss;
		ss << fixed << setprecision(2) << gravity;
		expl.lines.push_back(strToWStr(ss.str() + " G").c_str());
	}

	if (obj["Atmosphere"].is_null() != true)
	{
		string atmoStr = obj["Atmosphere"];
		if (!atmoStr.empty())
		{
			expl.lines.push_back(strToWStr(obj["Atmosphere"]).c_str());
		}
	}

	if (obj["TerraformState"].is_null() != true)
	{
		string terraStr = obj["TerraformState"];
		if (!terraStr.empty())
		{
			expl.lines.push_back(strToWStr(terraStr).c_str());
		}
	}

	if (obj["Volcanism"].is_null() != true)
	{
		string volStr = obj["TerraformState"];
		if (!volStr.empty())
		{
			expl.lines.push_back(strToWStr(obj["Volcanism"]).c_str());
		}
	}
}

void JSONDataStructure::e_BuyExplorationData(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_SellExplorationData(nlohmann::json::object_t obj)
{
	signed long int tempBalance = 0;
	signed long int tempBaseValue = 0;
	signed long int tempBonus = 0;
	if (obj["BaseValue"].is_null() != true)
	{
		tempBaseValue = obj["BaseValue"];
		cmdr.creditBalance += obj["BaseValue"];
	}

	if (obj["Bonus"].is_null() != true)
	{
		tempBonus = obj["Bonus"];
		cmdr.creditBalance += obj["Bonus"];
	}

	tempBalance = tempBaseValue + tempBonus;
	cmdr.creditBalance += tempBalance;
}

void JSONDataStructure::e_BuyTradeData(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_MarketBuy(nlohmann::json::object_t obj)
{
	if (obj["TotalCost"].is_null() != true)
	{
		signed long int tempBalance = obj["TotalCost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_MarketSell(nlohmann::json::object_t obj)
{
	if (obj["TotalSale"].is_null() != true)
	{
		signed long int tempBalance = obj["TotalSale"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_BuyAmmo(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_BuyDrones(nlohmann::json::object_t obj)
{
	if (obj["TotalCost"].is_null() != true)
	{
		signed long int tempBalance = obj["TotalCost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_CommunityGoalReward(nlohmann::json::object_t obj)
{
	if (obj["Reward"].is_null() != true)
	{
		signed long int tempBalance = obj["Reward"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_CrewHire(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_FetchRemoteModule(nlohmann::json::object_t obj)
{
	if (obj["TransferCost"].is_null() != true)
	{
		signed long int tempBalance = obj["TransferCost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_MissionCompleted(nlohmann::json::object_t obj)
{
	signed long int tempBalance = 0;
	signed long int tempReward = 0;
	signed long int tempDonation = 0;
	if (obj["Reward"].is_null() != true)
	{
		tempReward = obj["Reward"];
	}

	if (obj["Donation"].is_null() != true)
	{
		tempDonation = obj["Donation"];
		tempDonation = tempDonation * -1;
	}

	tempBalance = tempReward + tempDonation;
	if (tempBalance < 0)
	{
		cmdr.creditBalance -= abs(tempBalance);
	}
	else
	{
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_ModuleBuy(nlohmann::json::object_t obj)
{
	signed long int tempBuy = 0;
	signed long int tempSell = 0;
	signed long int tempBalance = 0;

	if (obj["BuyPrice"].is_null() != true)
	{
		tempBuy = obj["BuyPrice"];
		tempBuy = tempBuy * -1;
	}
	if (obj["SellPrice"].is_null() != true)
	{
		tempSell = obj["SellPrice"];
	}

	// If the balance is less than zero, player lost money on the module buy
	tempBalance = tempBuy + tempSell;
	if (tempBalance < 0)
	{
		cmdr.creditBalance -= abs(tempBalance);
	}
	else
	{
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_ModuleSell(nlohmann::json::object_t obj)
{
	if (obj["SellPrice"].is_null() != true)
	{
		signed long int tempBalance = obj["SellPrice"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_ModuleSellRemote(nlohmann::json::object_t obj)
{
	if (obj["SellPrice"].is_null() != true)
	{
		signed long int tempBalance = obj["SellPrice"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_PayFines(nlohmann::json::object_t obj)
{
	if (obj["Amount"].is_null() != true)
	{
		signed long int tempBalance = obj["Amount"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_PayLegacyFines(nlohmann::json::object_t obj)
{
	if (obj["Amount"].is_null() != true)
	{
		signed long int tempBalance = obj["Amount"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_RedeemVoucher(nlohmann::json::object_t obj)
{
	if (obj["Amount"].is_null() != true)
	{
		signed long int tempBalance = obj["Amount"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_RefuelAll(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_RefuelPartial(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_Repair(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_RepairAll(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_RestockVehicle(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_SellDrones(nlohmann::json::object_t obj)
{
	if (obj["TotalSale"].is_null() != true)
	{
		signed long int tempBalance = obj["TotalSale"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_ShipyardBuy(nlohmann::json::object_t obj)
{
	signed long int tempBalance = 0;
	signed long int tempBuy = 0;
	signed long int tempSell = 0;
	if (obj["ShipPrice"].is_null() != true)
	{
		tempBuy = obj["ShipPrice"];
		tempBuy = tempBuy * -1;
	}
	if (obj["SellPrice"].is_null() != true)
	{
		tempSell = obj["SellPrice"];
	}
	if (obj["StoreOldShip"].is_null() != true)
	{
		string temp = obj["StoreOldShip"];
		if (temp != "")
		{
			if (obj["ShipType"].is_null() != true)
			{
				string temp = obj["ShipType"];
				cmdr.ship = temp;
			}
		}
	}

	tempBalance = tempBuy + tempSell;
	if (tempBalance < 0)
	{
		cmdr.creditBalance -= abs(tempBalance);
	}
	else
	{
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_ShipyardSell(nlohmann::json::object_t obj)
{
	if (obj["ShipPrice"].is_null() != true)
	{
		signed long int tempBalance = obj["ShipPrice"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_ShipyardTransfer(nlohmann::json::object_t obj)
{
	if (obj["TransferPrice"].is_null() != true)
	{
		signed long int tempBalance = obj["TransferPrice"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_ShipyardSwap(json::object_t obj)
{
	// Changing the name of the currently selected ship
	if (obj["ShipType"].is_null() != true)
	{
		string temp = obj["ShipType"];
		cmdr.ship = temp;
	}
}

void JSONDataStructure::e_PowerplayFastTrack(nlohmann::json::object_t obj)
{
	if (obj["Cost"].is_null() != true)
	{
		signed long int tempBalance = obj["Cost"];
		cmdr.creditBalance -= tempBalance;
	}
}

void JSONDataStructure::e_PowerplaySalary(nlohmann::json::object_t obj)
{
	if (obj["Amount"].is_null() != true)
	{
		signed long int tempBalance = obj["Amount"];
		cmdr.creditBalance += tempBalance;
	}
}

void JSONDataStructure::e_Continued(json::object_t obj)
{
	// Notify new file needs to be read
	this->continueEvent = true;
}

void JSONDataStructure::e_DockFighter(json::object_t obj)
{
	// Restore mothership name if fighter was docked by player control
	if (cmdr.playerControlFighter)
	{
		cmdr.ship = cmdr.mothership;
		cmdr.playerControlFighter = false;
	}
}

void JSONDataStructure::e_DockSRV(json::object_t obj)
{
	// Restore mothership name
	cmdr.ship = cmdr.mothership;
}

void JSONDataStructure::e_LaunchFighter(json::object_t obj)
{
	// Determine if player controlled
	if (obj["PlayerControlled"].is_null() != true)
	{
		if ((bool)obj["PlayerControlled"])
		{
			// Save the current Mothership name
			cmdr.mothership = cmdr.ship;
			// Change ship name to Fighter
			cmdr.ship = "Fighter";
			cmdr.playerControlFighter = true;
		}
	}
}

void JSONDataStructure::e_LaunchSRV(json::object_t obj)
{
	// Save current mothership name
	cmdr.mothership = cmdr.ship;
	// Change ship name to SRV
	cmdr.ship = "SRV";
}

void JSONDataStructure::e_Promotion(json::object_t obj)
{
	string combat = "Combat: ";
	string trade = "Trade: ";
	string exploration = "Exploration: ";
	string cqc = "CQC: ";

	if (obj["Combat"].is_null() != true)
	{
		cmdr.combatRank = combatRank[obj["Combat"]];
	}

	if (obj["Trade"].is_null() != true)
	{
		cmdr.tradeRank = tradeRank[obj["Trade"]];
	}

	if (obj["Explore"].is_null() != true)
	{
		cmdr.explorationRank = explorerRank[obj["Explore"]];
	}

	if (obj["CQC"].is_null() != true)
	{
		cmdr.cqc = cqcRank[obj["CQC"]];
	}
}

void JSONDataStructure::e_VehicleSwitch(json::object_t obj)
{
	if (obj["VehicleSwitch"].is_null() != true)
	{
		// Determine switch
		string shipSwitch = obj["To"];
		if (shipSwitch == "Fighter")
		{
			// Save mothership name
			cmdr.mothership = cmdr.ship;
			// Ship name to fighter
			cmdr.ship = "Fighter";
		}
		else if (shipSwitch == "Mothership")
		{
			// Restore mothership name
			cmdr.ship = cmdr.mothership;
		}
	}
}