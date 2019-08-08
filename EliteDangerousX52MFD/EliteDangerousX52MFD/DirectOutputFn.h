#pragma once
#include "DepInclude\DirectOutput.h"
#include "displaydata.h"
#include <algorithm>

#include <locale>
#include <codecvt>
#include <string>

class DirectOutputFn
{
	typedef std::vector<void*> DeviceList;
	DeviceList m_devices;
	HMODULE dll;
	HRESULT hr = S_OK;

	int currentPage = 0;
	std::vector<int> currentPageLine;

	
public:
	DirectOutputFn();
	~DirectOutputFn();

	void SetOrUpdateDisplayData(std::unique_ptr<mfdData> data);

	HRESULT Initialize(const wchar_t* wszPluginName);
	HRESULT Deinitialize();
	void RegisterDevice();
	bool GetDeviceType();
	HRESULT setDeviceProfile(std::string file);
	HRESULT setPage(int pageNumber, const DWORD flag);
	HRESULT setString(int pageNumber, int stringLineID, wchar_t* stringToOutput);
	HRESULT registerSoftBtnCallback();
	HRESULT registerPageCallback();
	HRESULT unRegisterSoftBtnCallback();
	HRESULT unRegisterPageCallback();
	int getCurrentPage();
	

private:

	std::unique_ptr<mfdData> displayData;

	static void __stdcall OnEnumerateDevice(void* hDevice, void* pCtxt);
	static void __stdcall OnDeviceChanged(void* hDevice, bool bAdded, void* pCtxt);
	static void __stdcall OnPageChanged(void* hDevice, DWORD dwPage, bool bSetActive, void* pCtxt);
	static void __stdcall OnSoftButtonChanged(void* hDevice, DWORD dwButtons, void* pCtxt);
	void updatePageLine(int page, int newLineNumber);
	void updatePageOnScroll(int oneUpZeroDown);

	void refresDisplay();
};