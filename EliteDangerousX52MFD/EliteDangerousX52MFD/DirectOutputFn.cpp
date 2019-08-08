// DirectOutputFn.cpp : Contains all the functions used to communicate with the X52 Pro MFD
#include "stdafx.h"
#include "DirectOutputFn.h"


std::wstring strToWStr(std::string str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(str);
	return wide;
}


// Constructor -> Loads in DirectOutput.dll
DirectOutputFn::DirectOutputFn()
{
	hr = S_OK;
	currentPage = 0;

	std::cout << "Loading DirectOutput libaray... ";

	dll = LoadLibrary(TEXT("DepInclude/DirectOutput.dll"));
	if (NULL != dll)
	{
		std::cout << "DONE.\n";
	}
	else
	{
		std::cout << "FAILED.\n";
	}


}

// Deconstructor -> Frees DirectOutput.dll
DirectOutputFn::~DirectOutputFn()
{
	std::cout << "Freeing DirectOutput library... ";
	if (!FreeLibrary(dll))
	{
		std::cout << "FAILED.\n";
	}
	else
	{
		std::cout << "DONE.\n";
	}
}


// Public Functions


void DirectOutputFn::SetOrUpdateDisplayData(std::unique_ptr<mfdData> data)
{
	if (data->pages.size() == 0) {
		return;
	}
	displayData = std::move(data);
	if (currentPageLine.size() == 0) {
		// This is clearly the first time we initialize, so we need to set up the pages we want
		currentPage = 0;
		for(int i = 0; i < displayData->pages.size(); ++i)
		{ 
			setPage(i, i == 0 ? FLAG_SET_AS_ACTIVE : 0); // set first page active
			currentPageLine.push_back(0);
		}

	}

	for (int i = 0; i < currentPageLine.size(); ++i)
	{
		// re-clamp all pages to the max number of lines
		updatePageLine(i, currentPageLine[i]);
	}

	refresDisplay();
}

/*
	PARAMETERS: const wchar_t * wszPluginName = Name of this application
	RETURNS: HRESULT hr == status indicator to determine if passed

	FUNCTION: Initialized DirectOutput library
*/
HRESULT DirectOutputFn::Initialize(const wchar_t * wszPluginName)
{
	std::cout << "Initialzing DirectOutput library... " << std::endl;

	Pfn_DirectOutput_Initialize initDOFn = (Pfn_DirectOutput_Initialize)GetProcAddress(dll, "DirectOutput_Initialize");
	hr = initDOFn(wszPluginName);

	return hr;
}

/*
	PARAMETERS: none
	RETURNS: HRESULT hr == status indicator to determine if passed

	FUNCTION: Deinitializes the DirectOutput library
*/
HRESULT DirectOutputFn::Deinitialize()
{
	std::cout << "Deinitializing DirectOutput... " << std::endl;
	Pfn_DirectOutput_Deinitialize deInitfunc = (Pfn_DirectOutput_Deinitialize)GetProcAddress(dll, "DirectOutput_Deinitialize");
	hr = deInitfunc();

	return hr;
}

/*
	PARAMETERS: none
	RETURNS: none

	FUNCTION: Gets the currently selected device and adds it to the m_devices list
*/
void DirectOutputFn::RegisterDevice()
{
	std::cout << "Registering device callback... " << std::endl;
	Pfn_DirectOutput_RegisterDeviceCallback fnRegisterDeviceCallback = (Pfn_DirectOutput_RegisterDeviceCallback)GetProcAddress(dll, "DirectOutput_RegisterDeviceCallback");
	hr = fnRegisterDeviceCallback(OnDeviceChanged, this);
	if (hr == S_OK)
	{
		std::cout << "DONE.\n";
	}
	else
	{
		std::cout << "FAILED.\n";
	}

	std::cout << "Enumerating devices... " << std::endl;
	Pfn_DirectOutput_Enumerate fnEnumerate = (Pfn_DirectOutput_Enumerate)GetProcAddress(dll, "DirectOutput_Enumerate");
	hr = fnEnumerate(OnEnumerateDevice, this);
	if (FAILED(hr))
	{
		std::cout << "FAILED.\n";
	}
	else
	{
		std::cout << "DONE.\n";
	}
}

/*
	PARAMETERS: none
	RETURNS: bool == If the controller is not even connected, the vector will be 0.

	FUNCTION: Determines what device type is connected based on the m_devices array. Sort of hard coded to be the X52 Pro since that is all I am looking for
*/
bool DirectOutputFn::GetDeviceType()
{
	std::cout << "Getting device... " << std::endl;
	Pfn_DirectOutput_GetDeviceType fnGetDeviceType = (Pfn_DirectOutput_GetDeviceType)GetProcAddress(dll, "DirectOutput_GetDeviceType");
	GUID typeGUID = { 0 };
	if (m_devices.size() == 0)
	{
		std::cout << "The controller is not connected or is not a X52 Pro.\n";
		std::cout << "Please close the application and plug in the controller. Then restart the application.\n";
		return false;
	}
	for (DeviceList::iterator it = m_devices.begin(); it != m_devices.end(); it++)
	{
		hr = fnGetDeviceType(*it, &typeGUID);
		if (FAILED(hr))
		{
			std::cout << "FAILED.\n";
			return false;
		}

		if (typeGUID == DeviceType_X52Pro)
		{
			std::cout << "Got device X52Pro.\n";
			return true;
		}
	}
	return false;
}

/*
	PARAMETERS: wchar_t* filepath == full pathname to the desired profile
	RETURNS: HRESULT hr == status indicator to determine if passed

	FUNCTION: Sets the X52 Pro to the desired profile. This might be unneccessary as I will only be using the screen functions at the moment and not changing any keybindings or lights
*/
HRESULT DirectOutputFn::setDeviceProfile(std::string file)
{
	std::cout << "Setting the device profile... " << std::endl;
	Pfn_DirectOutput_SetProfile fnSetProfile = (Pfn_DirectOutput_SetProfile)GetProcAddress(dll, "DirectOutput_SetProfile");
	void * hdevice = m_devices[0];
	std::wstring filepath = strToWStr(file);
	size_t size = wcslen(filepath.c_str());
	hr = fnSetProfile(hdevice, size, filepath.c_str());
	return hr;
}

/*
	PARAMETERS: int pageNumber == page to add to display
			const DWORD flag == for setting the page active or not
	RETURNS: HRESULT hr == status indicator to determine if passed

	FUNCTION: The MFD works on a page system that can be scrolled through. Page 0 being the main page, page 1 being the next and so on.
			The flags are listed as:
				FLAG_SET_AS_ACTIVE == sets the page to be active
				0 == will not change the active page
*/

HRESULT DirectOutputFn::setPage(int pageNumber, const DWORD flag)
{
	std::cout << "Adding page... " << std::endl;
	Pfn_DirectOutput_AddPage fnSetPage = (Pfn_DirectOutput_AddPage)GetProcAddress(dll, "DirectOutput_AddPage");
	void * hdevice = m_devices[0];
	hr = fnSetPage(hdevice, pageNumber, flag);
	return hr;
}

/*
	PARAMETERS: int pageNumber == pageNumber to set the string on
			int stringLineID == line on the MFD to display the string
								0 -> line1
								1 -> line2
								2 -> line3
			wchar_t* stringToOutput == string to display on the MFD
	RETURNS: HRESULT hr == status indicator to determine if passed.

	FUNCTION: Sends a string to the MFD depending on the page and linenumber.
				** I can't seem to figure out how to add strings on non-active pages, so strings have to be set on the active page
*/
HRESULT DirectOutputFn::setString(int pageNumber, int stringLineID, wchar_t * stringToOutput)
{
	//cout << "Setting string... ";
	Pfn_DirectOutput_SetString fnSetString = (Pfn_DirectOutput_SetString)GetProcAddress(dll, "DirectOutput_SetString");
	void * hDevice = m_devices[0];
	size_t stringLength = wcslen(stringToOutput);
	hr = fnSetString(hDevice, pageNumber, stringLineID, stringLength, stringToOutput);
	return hr;
}

/*
	PARAMETERS: none
	RETURNS: HRESULT hr == status indicator to determine if passed.

	FUNCTION: Registers a handle so the device can let this program know the right scroll wheel moved up or down
*/
HRESULT DirectOutputFn::registerSoftBtnCallback()
{
	std::cout << "Registering soft button callback... " << std::endl;
	Pfn_DirectOutput_RegisterSoftButtonCallback fnRegSoftBtn = (Pfn_DirectOutput_RegisterSoftButtonCallback)GetProcAddress(dll, "DirectOutput_RegisterSoftButtonCallback");
	hr = fnRegSoftBtn(m_devices[0], OnSoftButtonChanged, this);
	return hr;
}

/*
	PARAMETERS: none
	RETURNS: HRESULT hr == status indicator to determine if passed.

	FUNCTION: Registers a handle so the device can let this program know when the left page wheel is used
*/
HRESULT DirectOutputFn::registerPageCallback()
{
	std::cout << "Registering page callback... " << std::endl;
	Pfn_DirectOutput_RegisterPageCallback fnRegPageCallback = (Pfn_DirectOutput_RegisterPageCallback)GetProcAddress(dll, "DirectOutput_RegisterPageCallback");
	hr = fnRegPageCallback(m_devices[0], OnPageChanged, this);
	return hr;
}

/*
	PARAMETERS: none
	RETURNS: HRESULT hr == status indicator to determine if passed.

	FUNCTION: Unregisters the right scroll wheel handle. Cleanup function.
*/
HRESULT DirectOutputFn::unRegisterSoftBtnCallback()
{
	std::cout << "Unregistering soft button callback... " << std::endl;
	Pfn_DirectOutput_RegisterSoftButtonCallback fnRegSoftBtn = (Pfn_DirectOutput_RegisterSoftButtonCallback)GetProcAddress(dll, "DirectOutput_RegisterSoftButtonCallback");
	hr = fnRegSoftBtn(m_devices[0], NULL, NULL);
	return hr;
}

/*
	PARAMETERS: none
	RETURNS: HRESULT hr == status indicator to determine if passed.

	FUNCTION: Unregisters the right scroll wheel handle. Cleanup function.
*/
HRESULT DirectOutputFn::unRegisterPageCallback()
{
	std::cout << "Unregistering page callback... " << std::endl;
	Pfn_DirectOutput_RegisterPageCallback fnRegPageCallback = (Pfn_DirectOutput_RegisterPageCallback)GetProcAddress(dll, "DirectOutput_RegisterPageCallback");
	hr = fnRegPageCallback(m_devices[0], NULL, NULL);
	return hr;
}

/*
	PARAMETERS: none
	RETURNS: currentPage == the currently selected active page on the MFD. See setString() as to why this is neccessary

	FUNCTION: Returns the currently selected page on the MFD back to the main function.
*/
int DirectOutputFn::getCurrentPage()
{
	return currentPage;
}


// Private Functions
void __stdcall DirectOutputFn::OnEnumerateDevice(void * hDevice, void * pCtxt)
{
	DirectOutputFn* pThis = (DirectOutputFn*)pCtxt;
	pThis->m_devices.push_back(hDevice);
}

void __stdcall DirectOutputFn::OnDeviceChanged(void * hDevice, bool bAdded, void * pCtxt)
{
	DirectOutputFn* pThis = (DirectOutputFn*)pCtxt;
	if (bAdded)
	{
		// device has been added, add to list of devices
		{
			TCHAR tsz[1024];
			_sntprintf_s(tsz, sizeof(tsz) / sizeof(tsz[0]), sizeof(tsz) / sizeof(tsz[0]), _T("DeviceAdded(%p)\n"), hDevice);
			OutputDebugString(tsz);
		}
		pThis->m_devices.push_back(hDevice);
	}
	else
	{
		// device has been removed, remove from list of devices
		{
			TCHAR tsz[1024];
			_sntprintf_s(tsz, sizeof(tsz) / sizeof(tsz[0]), sizeof(tsz) / sizeof(tsz[0]), _T("DeviceRemoved(%p)\n"), hDevice);
			OutputDebugString(tsz);
		}
		for (DeviceList::iterator it = pThis->m_devices.begin(); it != pThis->m_devices.end(); ++it)
		{
			if (*it == hDevice)
			{
				pThis->m_devices.erase(it);
				break;
			}
		}
	}
}

void __stdcall DirectOutputFn::OnPageChanged(void * hDevice, DWORD dwPage, bool bSetActive, void * pCtxt)
{
	DirectOutputFn* pThis = (DirectOutputFn*)pCtxt;
	pThis->currentPage = dwPage;
	pThis->refresDisplay();
}

void __stdcall DirectOutputFn::OnSoftButtonChanged(void * hDevice, DWORD dwButtons, void * pCtxt)
{
	DirectOutputFn* pThis = (DirectOutputFn*)pCtxt;
	if (dwButtons & SoftButton_Up)
	{
		pThis->updatePageOnScroll(-1);
	}
	else if (dwButtons & SoftButton_Down)
	{
		pThis->updatePageOnScroll(1);
	}
}

void DirectOutputFn::updatePageLine(int page, int newLineNumber)
{
	auto pageData = displayData->pages[page];
	int currentLine = currentPageLine[page];

	// clamp current line within the available lines
	currentPageLine[page] = std::min(std::max(newLineNumber, 0), (int)pageData.lines.size() - 1);
}

/*
	PARAMETERS: int direction -> int value to determine if the user wants to scroll down or up. Value of 1 will scroll down, value of -1 will scroll up.
	RETURNS: none

	FUNCTION: Updates the current page based on the scroll function of the right wheel. 
*/
void DirectOutputFn::updatePageOnScroll(int direction)
{
	int currentLine = currentPageLine[currentPage];

	// clamp current line within the available lines
	updatePageLine(currentPage, currentLine + direction);

	refresDisplay();
}




// Refresh the MFD display
void DirectOutputFn::refresDisplay() 
{
	int line = currentPageLine[currentPage];

	auto page = displayData->pages[currentPage];

	// Set the lines we are looking for
	for (int i = 0; i < 3 && i < page.lines.size() - line; i++)
	{
		int idx = line + i;
		std::wstring line = strToWStr(page.lines[idx]);
		wchar_t* wc = const_cast<wchar_t*>(line.c_str());
		setString(currentPage, i, wc);
	}

	// fill any remaining lines with blanks
	for (int i = page.lines.size() - line; i < 3; i++) {
		setString(currentPage, i, L"");
	}

}