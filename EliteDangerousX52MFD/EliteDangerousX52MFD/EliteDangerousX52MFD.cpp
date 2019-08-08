// EliteDangerousX52MFD.cpp : Entry point

/*
	EliteDangerousX52MFD v 1.1.2
	Special Thanks to:
		Frontier Developments for Elite Dangerous
		Saitek for the use and development of the SDK to run this project
		Niels Lohmann for work on JSON for Modern C++ (https://github.com/nlohmann/json)
*/

#include "stdafx.h"
#include "DepInclude/cxxopts.hpp"
#include <ShlObj.h>
#include "DirectOutputFn.h"
#include "JSONReader.h"


// DirectOutput function object
// Creation of this object automatically loads in DirectOutput but it still needs to be initialized
DirectOutputFn fn;

// Text filepath holders
TCHAR profileFilepath[260];
TCHAR defaultDirectory[260];
TCHAR journalFolderpath[260];
TCHAR currentJournal[260];

std::string inputFile;
std::string profileFile;

// Instance checking
bool foundProfile = false;
bool closeOnWindowX = false;

// Internal Functions
void initDirectOutput();
void checkHR(HRESULT hr);

void waitForJournalUpdate();
void cleanupAndClose();
BOOL controlHandler(DWORD fdwCtrlType);


int main(int argc, char** argv)
{
	// Setup control handling, if app is closed by other means. (Ctrl + C or hitting the 'X' button)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)controlHandler, TRUE);


	cxxopts::Options options("X52MDF-JSON-DRIVER", "Helps drive a X52 PRO MDF from a JSON file");
	options
		.allow_unrecognised_options()
		.positional_help("<input>")
		.show_positional_help();	

	options.add_options()
		("h,help", "Print help")
		("p,profile", "Profile file to load", cxxopts::value<std::string>(), "FILE")
		("input", "Input JSON file to monitor", cxxopts::value<std::vector<std::string>>(), "FILE")
		;

	options.parse_positional({"input"});

	auto args = options.parse(argc, argv);

	if (args.count("help"))
	{
		std::cout << options.help({ "", "Group" }) << std::endl;
		exit(0);
	}

	if (args.count("input") == 0)
	{
		std::cout << options.help({ "", "Group" }) << std::endl;
		exit(0);
	}

	if (args.count("profile"))
	{
		profileFile = args["profile"].as<std::string>();
	}

	initDirectOutput();

	inputFile = args["input"].as<std::vector<std::string>>().at(0);
	std::cout << "Reading from JSON file = " << inputFile << std::endl;	
	auto data = reader::ReadJSONFile(inputFile);

	fn.SetOrUpdateDisplayData(std::move(data));


	std::cout << "\nPress ENTER to close this application.";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return 0;
}


void initDirectOutput()
{
	// Initialize DirectOutput
	checkHR(fn.Initialize(L"EliteDangerousX52MFD"));

	// Registers and enumerates device
	fn.RegisterDevice();

	// Gets the enumerated device
	if (!fn.GetDeviceType())
	{
		std::cout << "\nPress ENTER to close this application.";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		ExitProcess(0);
	}

	//TODO(pbirk): Move profile setup here
	// Set the profile, if provided
	if (profileFile.length() > 0)
	{
		std::ifstream profile(profileFile);
		if (profile.good()) {
			checkHR(fn.setDeviceProfile(profileFile));
		}
		else
		{
			std::cout << "Couldn't link a profile from nonexisting file " << profileFile << std::endl;
		}
		
	}
	else
	{
		std::cout << "Couldn't link a profile since one was not provided!" << std::endl;
	}

	// Register right soft button clicks and scrolls
	checkHR(fn.registerSoftBtnCallback());

	// Register page change callback
	checkHR(fn.registerPageCallback());

}

/*
	PARAMETERS: HRESULT hr == some functions from DirectOutput return a HRESULT value, this just checks if it pass/fail and ouputs to the console
	RETURNS: none

	FUNCTION: Checks result of the function if it returns an HRESULT value

	Read more about HRESULT return types at
	Microsoft (MSDN) -> https://msdn.microsoft.com/en-us/library/windows/desktop/aa378137(v=vs.85).aspx
	Wikipedia -> https://en.wikipedia.org/wiki/HRESULT
*/
void checkHR(HRESULT hr)
{
	if (hr == S_OK)
	{
		std::cout << "DONE.\n";
	}
	else
	{
		std::cout << "FAILED/ hr = " << hr << std::endl;
	}
}

/*
	PARAMETERS: none
	RETURNS: none

	FUNCTION: This function will set a associated to when a file changes in the directory. This is useful so the program won't continually read the journal hogging resources. It will wait until there is a change in information.
*/
void waitForJournalUpdate()
{
	DWORD dwWaitStatus;
	HANDLE dwChangeHandle;

	// Watch for new Journal creations
	dwChangeHandle = FindFirstChangeNotification(journalFolderpath, FALSE, FILE_NOTIFY_CHANGE_FILE_NAME);
	if (dwChangeHandle == INVALID_HANDLE_VALUE)
	{
		printf("\nFindFirstChangeNotification failed.\n");
		FindCloseChangeNotification(dwChangeHandle);
		ExitProcess(GetLastError());
	}

	// Validity check, make sure the handle notification was set correctly
	if (dwChangeHandle == NULL)
	{
		printf("\nUnexpected NULL FindFirstChangeNotification\n");
		ExitProcess(GetLastError());
	}

	// Open up the journal in a stream for file size checking
	long prevPos;
	if (currentJournal[0] != _T('\0'))
	{
		std::ifstream journalFile(currentJournal, std::ios::binary | std::ios::ate);
		prevPos = journalFile.tellg();
		journalFile.close();
	}

	// Waiting and detecting changes
	bool changeDetected = false;
	while (!changeDetected)
	{
		dwWaitStatus = WaitForSingleObject(dwChangeHandle, 1000);
		switch (dwWaitStatus)
		{
		/*
			FILE_NOTIFY_CHANGE_FILE_NAME
			Runs when a file is added to the directory. I'll be using this if the player exits to the main menu and restarts the game.
			A new journal should be created.
		*/
		case WAIT_OBJECT_0:
			// Write to file was detected
			changeDetected = true;
			if (FindNextChangeNotification(dwChangeHandle) == FALSE)
			{
				printf("\nFindFirstChangeNotification failed.\n");
				ExitProcess(GetLastError());
			}
			break;

		/*
			After doing some research on the forum on the journal this is the method I have to use to determine if the file has been modified.
			I'll have to check to see if the file size has changed once every second and if so there has been an update.

			Apparently they are writing to the file by flushing the stream and I have no idea how to detect that.
			Previously I would use a wait object above but for file writes and it wasn't being detected unless I manually back out and enter the journal folder.
		*/
		case WAIT_TIMEOUT:
			if (currentJournal[0] != _T('\0'))
			{
				std::ifstream journalFile(currentJournal, std::ios::binary | std::ios::ate);
				long newPos = journalFile.tellg();
				journalFile.close();
				if (newPos > prevPos)
				{
					changeDetected = true;
				}
			}
			break;

		default:
			printf("\nUnhandled dwWaitStatus\n");
			ExitProcess(GetLastError());
			break;
		}
	}
	FindCloseChangeNotification(dwChangeHandle);
}

/*
	PARAMETERS: none
	RETURNS: none

	FUNCTION: Simplifies all cleanup into one function so the actions happen back to back allowing in easier exiting
*/
void cleanupAndClose() {
	//Unregister the callbacks
	checkHR(fn.unRegisterSoftBtnCallback());
	checkHR(fn.unRegisterPageCallback());

	// Deinitialize DirectOutput
	checkHR(fn.Deinitialize());
}

/*
	PARAMETERS: DWORD fdwCtrlType -> The type of action performed by the user on the console window or on system level
	RETURNS: bool condition -> Will alert the program if a certain close event has occured, so proper cleanup can be done

	FUNCTION: Case statements for determining the type of event invoked by the user on the console window
*/
BOOL controlHandler(DWORD fdwCtrlType) {
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		closeOnWindowX = true;
		cleanupAndClose();
		return(TRUE);

		// CTRL-CLOSE: confirm that the user wants to exit on 'X' button click on window. 
	case CTRL_CLOSE_EVENT:
		closeOnWindowX = true;
		cleanupAndClose();
		return(TRUE);

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		return FALSE;

	default:
		return FALSE;
	}
}