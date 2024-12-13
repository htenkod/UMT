#include "pch.h"




DEVICE_DATA_t UMT_DeviceData[UMT_SITES_MAX];

HMODULE hwinusb = NULL;

HANDLE devHandle = NULL;

DEFINE_GUID(UMT_CLASS_GUID,
    0x13EB360B, 0xBC1E, 0x46CB, 0xAC, 0x8B, 0xEF, 0x3D, 0xA4, 0x7B, 0x40, 0x62);



static HRESULT RetrieveDevicePath(
    _Out_bytecap_(BufLen) LPTSTR DevicePath,
    _In_                  ULONG  BufLen,
    _In_                  ULONG  devIdx,
    _Out_opt_             PBOOL  FailureDeviceNotFound
)
{
    BOOL                             bResult = FALSE;
    HDEVINFO                         deviceInfo;
    SP_DEVICE_INTERFACE_DATA         interfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = NULL;
    ULONG                            length;
    ULONG                            requiredLength = 0;
    HRESULT                          hr;

    if (NULL != FailureDeviceNotFound) {

        *FailureDeviceNotFound = FALSE;
    }

    //
    // Enumerate all devices exposing the interface
    //
    deviceInfo = SetupDiGetClassDevs(&UMT_CLASS_GUID,
        NULL,
        NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (deviceInfo == INVALID_HANDLE_VALUE) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    //
    // Get the first interface (index 0) in the result set
    //
    bResult = SetupDiEnumDeviceInterfaces(deviceInfo,
        NULL,
        &UMT_CLASS_GUID,
        devIdx,
        &interfaceData);

    if (FALSE == bResult) {

        //
        // We would see this error if no devices were found
        //
        if (ERROR_NO_MORE_ITEMS == GetLastError() &&
            NULL != FailureDeviceNotFound) {

            *FailureDeviceNotFound = TRUE;
        }

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Get the size of the path string
    // We expect to get a failure with insufficient buffer
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
        &interfaceData,
        NULL,
        0,
        &requiredLength,
        NULL);

    if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {

        hr = HRESULT_FROM_WIN32(GetLastError());
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Allocate temporary space for SetupDi structure
    //
    detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
        LocalAlloc(LMEM_FIXED, requiredLength);

    if (NULL == detailData)
    {
        hr = E_OUTOFMEMORY;
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    length = requiredLength;

    //
    // Get the interface's path string
    //
    bResult = SetupDiGetDeviceInterfaceDetail(deviceInfo,
        &interfaceData,
        detailData,
        length,
        &requiredLength,
        NULL);

    if (FALSE == bResult)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        LocalFree(detailData);
        SetupDiDestroyDeviceInfoList(deviceInfo);
        return hr;
    }

    //
    // Give path to the caller. SetupDiGetDeviceInterfaceDetail ensured
    // DevicePath is NULL-terminated.
    //
    hr = StringCbCopy(DevicePath,
        BufLen,
        detailData->DevicePath);

    LocalFree(detailData);
    SetupDiDestroyDeviceInfoList(deviceInfo);

    return hr;
}



BOOL ReadDeviceSerialNumber(_Inout_ PDEVICE_DATA_t DeviceData)
{
    if (DeviceData->WinusbHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    ULONG sz = 0;

    BOOL bResult = TRUE;

    ULONG cbRead = 0;
    
    bResult = WinUsb_GetDescriptor(DeviceData->WinusbHandle, 0x03, 3, 0x0409, DeviceData->SerialNumber, 64, &cbRead);

    //sz = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)&DeviceData->SerialNumber[2], (cbRead - 2) / 2, NULL, 0, NULL, NULL);

    //sz = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)&szBuffer[2], cbRead, DeviceData->SerialNumber, sz, NULL, NULL);

    if (!bResult)
    {
        return bResult;

    }

    return bResult;
}


BOOL
ReadFromBulkEndpoint(
    WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG cbSize, CHAR *szBuffer)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;
    const char* targetstr = " *** ";
    
    ULONG cbRead = 0;
    bResult = WinUsb_ReadPipe(hDeviceHandle, *pID, (UCHAR *)szBuffer, cbSize, &cbRead, 0);

    szBuffer[cbRead - 1] = '\0';

    if (strstr(szBuffer, targetstr) != 0) {
        strcpy_s(szBuffer, strlen(szBuffer), strstr(szBuffer, targetstr));
    }
    else {
        return -1;
    }

    char* newlinepos = strchr(szBuffer, '\r');

    if (newlinepos != nullptr) {
        *newlinepos = '\0';
    }
    else {
        return -1;
    }


    return bResult;
}


BOOL WriteToBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, PUCHAR data, ULONG sz, ULONG* pcbWritten)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE || !pID || !pcbWritten)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;
    
    ULONG cbSent = 0;

    bResult = WinUsb_WritePipe(hDeviceHandle, *pID, (PUCHAR)data, sz, &cbSent, 0);

    if (!bResult)
    {
        return bResult;
    }

    *pcbWritten = cbSent;

    return bResult;
}

BOOL DummyReadBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID) {

    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    CHAR dummyBuf[512];
    BOOL bResult = TRUE;

    ULONG cbRead = 0;
    bResult = WinUsb_ReadPipe(hDeviceHandle, *pID, (UCHAR*)dummyBuf, sizeof(dummyBuf), &cbRead, 0);

    return bResult;

}

static HRESULT
GetBulkPipes(
    _Inout_ PDEVICE_DATA_t DeviceData
)
{
    BOOL result;
    USB_INTERFACE_DESCRIPTOR usbInterface;
    WINUSB_PIPE_INFORMATION_EX pipe;
    HRESULT hr = S_OK;
    UCHAR i;

    result = WinUsb_QueryInterfaceSettings(DeviceData->WinusbHandle,
        0,
        &usbInterface);

    if (result == FALSE)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        //printf(_T("WinUsb_QueryInterfaceSettings failed to get USB interface.\n"));
        CloseHandle(DeviceData->DeviceHandle);
        return hr;
    }

    for (i = 0; i < usbInterface.bNumEndpoints; i++)
    {
        result = WinUsb_QueryPipeEx(
            DeviceData->WinusbHandle,
            0,
            (UCHAR)i,
            &pipe);

        if (result == FALSE)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            //printf(_T("WinUsb_QueryPipeEx failed to get USB pipe.\n"));
            CloseHandle(DeviceData->DeviceHandle);
            return hr;
        }

        if (pipe.PipeType == UsbdPipeTypeBulk)
        {
            if (USB_ENDPOINT_DIRECTION_IN(pipe.PipeId))
            {
                DeviceData->BulkInPipe = pipe.PipeId;
            }
            if (USB_ENDPOINT_DIRECTION_OUT(pipe.PipeId))
            {
                DeviceData->BulkOutPipe = pipe.PipeId;
            }

        }
    }

    return hr;
}

/**/
/* Load libusb dll
/**/

static int load_libusb(void)
{
	int retVal = -1;
	LPCWSTR libName = TEXT("hwinusb.dll");

	if ((hwinusb = LoadLibrary(libName)) != NULL) {

		retVal = 0;
	}


	return retVal;
}

static int nmi_free_bus(void)
{
	int retVal;

	if (hwinusb != NULL) {
		if (FreeLibrary(hwinusb) == TRUE) {
			hwinusb = NULL;
			retVal = 0;
		}
		else {
			retVal = -1;
		}
	}
	else {
		retVal = -2;
	}

	return retVal;
}

/**/


UMTDLL_DECLDIR HRESULT __stdcall UMT_Initialize()
{
	/*Load the LIBUSB DLL */

	/*int LIBUSB_CALL libusb_init(libusb_context **ctx);*/    

    HRESULT hr = S_OK;
    BOOL    bResult;
    ULONG   idx = 0;

    BOOL FailureDeviceNotFound = FALSE;


    for (idx = 0; idx < UMT_SITES_MAX; idx++)
    {
        UMT_DeviceData[idx].HandlesOpen = FALSE;

        hr = RetrieveDevicePath(UMT_DeviceData[idx].DevicePath,
            sizeof(UMT_DeviceData[idx].DevicePath),
            idx,
            &FailureDeviceNotFound);

        if (FAILED(hr)) {
            // break the loop if no more devices found
            if (FailureDeviceNotFound)
                break;         
            return hr;
        }
        

        UMT_DeviceData[idx].DeviceHandle = CreateFile(UMT_DeviceData[idx].DevicePath,
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            NULL);

        if (INVALID_HANDLE_VALUE == UMT_DeviceData[idx].DeviceHandle) {

            hr = HRESULT_FROM_WIN32(GetLastError());
            return hr;
        }
        

        bResult = WinUsb_Initialize(UMT_DeviceData[idx].DeviceHandle,
            &UMT_DeviceData[idx].WinusbHandle);

        if (FALSE == bResult) {

            hr = HRESULT_FROM_WIN32(GetLastError());
            CloseHandle(UMT_DeviceData[idx].DeviceHandle);
            return hr;
        }

        

        hr = GetBulkPipes(&UMT_DeviceData[idx]);

        if (FAILED(hr)) {

            return hr;
        }

        if (ReadDeviceSerialNumber(&UMT_DeviceData[idx]) != TRUE)
            return -1;

        UMT_DeviceData[idx].HandlesOpen = TRUE;

       
    }

    return idx;

}

/*
Routine description :

Perform required cleanup when the device is no longer needed.

If OpenDevice failed, do nothing.

Arguments :

    DeviceData - Struct filled in by OpenDevice

    Return value :

None

*/
UMTDLL_DECLDIR HRESULT __stdcall UMT_Deinitialize(void)
{
    ULONG idx = 0;
 
    for (idx = 0; idx < UMT_SITES_MAX; idx++)
    {
        if (FALSE == UMT_DeviceData[idx].HandlesOpen) {

            //
            // Called on an uninitialized DeviceData
            //
            break;
        }

        WinUsb_Free(UMT_DeviceData[idx].WinusbHandle);
        CloseHandle(UMT_DeviceData[idx].DeviceHandle);
        UMT_DeviceData[idx].HandlesOpen = FALSE;

    }

    return S_OK;    
}


UMTDLL_DECLDIR HRESULT __stdcall UMT_GetDeviceInfo(UINT32 devIdx, DEVICE_DATA_t *deviceInfo)
{
    if (UMT_DeviceData[devIdx].HandlesOpen)
    {
        memcpy_s(deviceInfo, sizeof(DEVICE_DATA_t), &UMT_DeviceData[devIdx], sizeof(DEVICE_DATA_t));
    }
    else
    {
        return S_FALSE;
    }

    return S_OK;
    
}


static HRESULT UMT_CheckStatus(CHAR *response)
{
    if (strstr(response, SUCCUSS))
    {
        return S_OK;
    }
    else if (strstr(response, FAILURE))
    {
        return -1;
    }

    return -2;
}


UMTDLL_DECLDIR HRESULT __stdcall UMT_SetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum, UCHAR highLow)
{
    const char* cmdFmt = "pinset %d %d\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];    


    sprintf_s(localBuf, cmdFmt, pinNum, highLow);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }    

    return UMT_CheckStatus(localBuf);

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_GetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum)
{
    const char* cmdFmt = "pinget %d\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];


    sprintf_s(localBuf, cmdFmt, pinNum);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    return UMT_CheckStatus(localBuf);

}

//wrBuffSz and rdBuffSz can be set to 0 to use default values
UMTDLL_DECLDIR HRESULT __stdcall __stdcall UMT_UART_Up(DEVICE_DATA_t* UMT_Handle, UCHAR txPin, UCHAR rxPin, UINT32 baud, UINT32 rdBuffSz, UINT32 wdBuffSz)
{
    const char* cmdFmt = "uartup %d %d %d %d %d\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    // configure min buff size
    if (wdBuffSz < 512)
        wdBuffSz = 512;

    // configure min buff size
    if (rdBuffSz < 512)
        rdBuffSz = 512;

    sprintf_s(localBuf, 512, cmdFmt, txPin, rxPin, baud, rdBuffSz, wdBuffSz);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    if (S_OK == UMT_CheckStatus(localBuf)){

        const char* targetstr = "S *** ";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        return std::stoi(localBuf + offset);
        
    }
    else {
        return -1;
    }
    
}

UMTDLL_DECLDIR HRESULT __stdcall UMT_UART_Read(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UINT16 numOfbytes, CHAR* rdBuff)
{
    const char* cmdFmt = "uartrd %d %d\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, idx, numOfbytes);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    memset(localBuf, 0, sizeof(localBuf));

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;
    }

    sprintf_s(rdBuff, 512, "%s", localBuf);


    return UMT_CheckStatus(localBuf);

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_UART_Write(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR* wrBuff)
{
    const char* cmdFmt = "uartwr %s\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, wrBuff);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    return UMT_CheckStatus(localBuf);

}



UMTDLL_DECLDIR HRESULT __stdcall UMT_ADC_Up(DEVICE_DATA_t* UMT_Handle, UCHAR adcPin)
{
    const char* cmdFmt = "adcup %d\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, cmdFmt, adcPin);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    return UMT_CheckStatus(localBuf);

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_Up(DEVICE_DATA_t* UMT_Handle, UCHAR tckPin, UCHAR tmsPin, UCHAR tdoPin, UCHAR tdiPin, UCHAR pgcPin, UCHAR pgdPin, UCHAR mclrPin)
{
    const char* cmdFmt = "tapup %d %d %d %d %d %d %d\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, tckPin, tmsPin, tdoPin, tdiPin, pgcPin, pgdPin, mclrPin);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    if (S_OK == UMT_CheckStatus(localBuf)) {

        const char* targetstr = "S *** ";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        return std::stoi(localBuf + offset);

    }
    else {
        return -1;
    }

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_DevId(DEVICE_DATA_t* UMT_Handle, UINT32 idx, CHAR* devID)
{
    const char* cmdFmt = "tapdevid %d\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, idx);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    if (S_OK == UMT_CheckStatus(localBuf)) {

        const char* targetstr = "S *** ";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        strcpy_s(devID, strlen(localBuf), (localBuf + offset));

        return 0;

    }
    else {
        return -1;
    }

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_Flash(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR addr,UINT32 fwoffset, UINT32 memoffset, UCHAR* testfirmware)
{
    const char* cmdFmt = "tapflash %d %d %d %d %s\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, idx, addr, fwoffset, memoffset,testfirmware);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    if (S_OK == UMT_CheckStatus(localBuf)) {

        return 0;
    }
    else {
        return -1;
    }


}


UMTDLL_DECLDIR HRESULT __stdcall UMT_ADC_Get(DEVICE_DATA_t* UMT_Handle, UCHAR txPin, UINT16 *adcCount)
{
    const char* cmdFmt = "adcget %d\r\n";
    #define ADC_STRING "ADC Count ="

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, cmdFmt, txPin);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    CHAR* adcCntPtr = strstr(localBuf, ADC_STRING);
    if (adcCntPtr != NULL)
    {
        UINT32 adcVal = 0;
        adcCntPtr += sizeof(ADC_STRING);
        while (*adcCntPtr != '\r')
        {
            adcVal *= 10;
            adcVal += *adcCntPtr - '0';
            adcCntPtr++;
        }
        *adcCount = adcVal;
    }
    else
    {
        return S_FALSE;
    }    

    return UMT_CheckStatus(localBuf);
}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Reset(DEVICE_DATA_t* UMT_Handle) {

    const char* cmdFmt = "reset\r\n";

    ULONG cbSent = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return S_FALSE;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!DummyReadBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe))
            return S_FALSE;

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf))
            return S_FALSE;

    }

    return UMT_CheckStatus(localBuf);

}