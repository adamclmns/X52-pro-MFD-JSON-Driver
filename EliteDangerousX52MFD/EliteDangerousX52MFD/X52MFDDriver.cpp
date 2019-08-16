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

#include <filesystem>

#include <locale>
#include <codecvt>
#include <string>

#include <signal.h>



// DirectOutput function object
// Creation of this object automatically loads in DirectOutput but it still needs to be initialized
DirectOutputFn fn;

// Text filepath holders
TCHAR profileFilepath[260];
TCHAR defaultDirectory[260];
TCHAR journalFolderpath[260];
TCHAR currentJournal[260];

std::string inputFile;

// Instance checking
bool foundProfile = false;
bool closeOnWindowX = false;

// Internal Functions
void initDirectOutput();
void checkHR(HRESULT hr);

void registerFileChangeHandler(std::string filename, std::function<void()> handler);

void onFileChanged();



void cleanupAndClose();
BOOL controlHandler(DWORD fdwCtrlType);
int signalhandler(int, int);


int main(int argc, char** argv)
{
	// Setup control handling, if app is closed by other means. (Ctrl + C or hitting the 'X' button)
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)controlHandler, TRUE);
	_crt_signal_t err = signal(SIGTERM, (_crt_signal_t)signalhandler);
	if (err == SIG_ERR) {
		std::cout << "Error setting signal handler!" << std::endl;
	}
	err = signal(SIGINT, (_crt_signal_t)signalhandler);
	if (err == SIG_ERR) {
		std::cout << "Error setting signal handler!" << std::endl;
	}
	

	cxxopts::Options options("X52MDF-JSON-DRIVER", "Helps drive a X52 PRO MDF from a JSON file");
	options
		.allow_unrecognised_options()
		.positional_help("<input>")
		.show_positional_help();	

	options.add_options()
		("h,help", "Print help")
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


	initDirectOutput();

	inputFile = args["input"].as<std::vector<std::string>>()[0];
	std::cout << "Reading from JSON file = " << inputFile << std::endl;	
	
	onFileChanged(); // initial load
	registerFileChangeHandler(inputFile, onFileChanged);

	std::cout << "\nPress ENTER to close this application.";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return 0;
}


void onFileChanged()
{

	auto data = reader::ReadJSONFile(inputFile);

	fn.SetOrUpdateDisplayData(std::move(data));
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



void registerFileChangeHandler(std::string filename, std::function<void()> handler)
{

	DWORD dwWaitStatus;
	HANDLE dwChangeHandle;

	auto filepath = std::filesystem::path(filename);

	auto folderpath = filepath.remove_filename();
	LPCWSTR pFilename = folderpath.native().c_str();
	
	dwChangeHandle = FindFirstChangeNotificationW(pFilename, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);

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

	auto prevModifyTime = std::filesystem::file_time_type::min();

	while (true)
	{
		dwWaitStatus = WaitForSingleObject(dwChangeHandle, INFINITE);
		
		switch (dwWaitStatus)
		{
			/*
				FILE_NOTIFY_CHANGE_FILE_NAME
				Runs when a file is added to the directory. I'll be using this if the player exits to the main menu and restarts the game.
				A new journal should be created.
			*/
		case WAIT_OBJECT_0:
		{
			// Write to some file was detected - check if it was ours

			// Some events triggered when the file is flushed to empty.
			// reading empty files is bad for your health, so skip those events.
			std::ifstream file(filename);
			bool empty = file.peek() == std::ifstream::traits_type::eof();
			file.close();
			if (!empty) {
				auto modifytime = std::filesystem::last_write_time(filename);

				if (modifytime > prevModifyTime) {
					prevModifyTime = modifytime;
					handler(); // trigger the handler
				}
			}


			if (FindNextChangeNotification(dwChangeHandle) == FALSE)
			{
				printf("\nFindFirstChangeNotification failed.\n");
				ExitProcess(GetLastError());
			}
			break;
		}
			

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

int signalhandler(int signal, int subcode) {
	std::cout << "Caught signal" << signal << std::endl;
	cleanupAndClose();
	return 0;
}