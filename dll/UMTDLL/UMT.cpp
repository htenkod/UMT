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

void convertHexToReadable(const CHAR hexData[], size_t hexDataLength, CHAR* output, size_t outputSize) {
    size_t outputIndex = 0;

    for (size_t i = 0; i < hexDataLength; ++i) {
        // Convert each byte to its two-digit hex value
        unsigned char value = static_cast<unsigned char>(hexData[i]);
        int written = sprintf_s(output + outputIndex, outputSize - outputIndex, "%02X", value);
        if (written < 0) {
            std::cerr << "Error writing to output buffer" << std::endl;
            return;
        }
        outputIndex += written;
    }

    // Ensure the output string is null-terminated, if space allows.
    if (outputIndex < outputSize)
        output[outputIndex] = '\0';
    else if (outputSize > 0)
        output[outputSize - 1] = '\0';
}


void hexToDecimal(const UCHAR* input, UCHAR* output, size_t size) {
    // Convert UCHAR* to std::string
    std::string hexStr(reinterpret_cast<const char*>(input));

    // Convert hex string to decimal integer
    unsigned long long decimalValue = std::stoull(hexStr, nullptr, 16);

    // Convert decimal integer to decimal string (store in UCHAR*)
    snprintf(reinterpret_cast<char*>(output), size, "%llu", decimalValue);
}

void decimalToHex(const CHAR* decimalStr, CHAR* hexStr, size_t size) {
    // Convert decimal string to integer
    unsigned long long decimalValue = std::strtoull(decimalStr, nullptr, 10);

    // Convert integer to hex string and store in hexStr
    snprintf(hexStr, size, "%llX", decimalValue);
}

bool searchInRawBuffer(const char* buffer, size_t bufferSize, const char* target, size_t* index) {
    size_t targetLen = strlen(target);

    // Search manually in the full buffer
    for (size_t i = 0; i <= bufferSize - targetLen; i++) {
        if (memcmp(buffer + i, target, targetLen) == 0) {
            *index = i;
            return true; // Found the target string
        }
    }
    *index = bufferSize;
    return false; // Not found
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


BOOL ReadFromBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG cbSize, CHAR *szBuffer, ULONG* pcbRead)
{
    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    BOOL bResult = TRUE;
    
    ULONG cbRead = 0;
    ULONG respLen = 0;
    ULONG exprespLen = 0;
    size_t flagindex = 0;

    memset(szBuffer, 0, cbSize);

    do{
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bResult = WinUsb_ReadPipe(hDeviceHandle, *pID, ((UCHAR*)szBuffer + cbRead), cbSize, &cbRead, 0);

        if (!bResult || cbRead == 0) break;

        respLen += cbRead;

        if (respLen >= cbSize) break;
    } while (searchInRawBuffer(szBuffer, cbSize, "DONE!", &flagindex) == FALSE);

      // Null-terminate safely
    if (respLen < cbSize) {
        szBuffer[respLen] = '\0';
    }
    else {
        szBuffer[cbSize - 1] = '\0';  // Prevent out-of-bounds write
    }

    // Locate "<" in buffer
    char* found = strstr(szBuffer, "<");
    if (found != nullptr) {
        size_t newLen = respLen - (found - szBuffer); //strlen(found) + 1;
        memmove(szBuffer, found, newLen);
        szBuffer[newLen - 1] = '\0';  // Ensure null termination
    }
    else {
        return -1;
    }

    // Null-terminate after "DONE!>"
    char* newlinepos = &szBuffer[flagindex] + strlen("DONE!>");
    if (*newlinepos != '\0') {
        *newlinepos = '\0';  // Correctly null-terminate
    }

    *pcbRead = respLen;

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
        return -1;
    }

    return S_OK;
    
}


static HRESULT UMT_CheckStatus(CHAR *response, size_t respsize)
{
    size_t index = 0;

    if (searchInRawBuffer(response, respsize, "DONE!", &index) == TRUE)
    {
        return S_OK;
    }
    else 
    {
        return -1;
    }

}


UMTDLL_DECLDIR HRESULT __stdcall UMT_SetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum, UCHAR highLow)
{
    const char* cmdFmt = "pinset %d %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];    


    sprintf_s(localBuf, cmdFmt, pinNum, highLow);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }    

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_GetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum)
{
    const char* cmdFmt = "pinget %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];


    sprintf_s(localBuf, cmdFmt, pinNum);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

//wrBuffSz and rdBuffSz can be set to 0 to use default values
UMTDLL_DECLDIR HRESULT __stdcall __stdcall UMT_UART_Up(DEVICE_DATA_t* UMT_Handle, UCHAR txPin, UCHAR rxPin, UINT32 baud, UINT32 rdBuffSz, UINT32 wdBuffSz)
{
    const char* cmdFmt = "uartup %d %d %d %d %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    // configure min buff size
    if (wdBuffSz < 512)
        wdBuffSz = 512;

    // configure min buff size
    if (rdBuffSz < 512)
        rdBuffSz = 512;

    sprintf_s(localBuf, 512, cmdFmt, txPin, rxPin, baud, rdBuffSz, wdBuffSz);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {
        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;
    }

    if (S_OK == UMT_CheckStatus(localBuf, sizeof(localBuf))){

        const char* targetstr = ">";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        char* positionPtr2 = strstr(localBuf, "\r\n<");

        if (positionPtr2 != nullptr) {
            *positionPtr2 = '\0';
        }
        else {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        return std::stoi(localBuf + offset);
        
    }
    else {
        return UMT_CheckStatus(localBuf, sizeof(localBuf));
    }
    
}

UMTDLL_DECLDIR HRESULT __stdcall UMT_UART_Read(DEVICE_DATA_t* UMT_Handle, UINT32 idx, CHAR* rdBuff, UINT32 hex, UINT16 numOfbytes)
{
    const char* cmdFmt = "uartrd %d %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];
    CHAR tmpBuf[539];

    if (numOfbytes > 512) {
        numOfbytes = 512;
    }

    sprintf_s(localBuf, 512, cmdFmt, idx, numOfbytes);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;
    }

    if (S_OK == UMT_CheckStatus(localBuf, sizeof(localBuf))) {

        size_t positionDataLen = 0;

        searchInRawBuffer(localBuf, sizeof(localBuf), "VAL:", &positionDataLen);

        const char* numberstr = localBuf + positionDataLen + strlen("VAL:");

        const char* targetstr = ">";

        char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        size_t DataLen = static_cast<size_t>(atoi(numberstr));

        *positionPtr = 0;
        const char *offset = positionPtr + strlen(targetstr);

        size_t  positionPtr2 = 0;
        if (searchInRawBuffer(localBuf, sizeof(localBuf), "\r\n<", &positionPtr2) == TRUE) {
            localBuf[positionPtr2] = '\0';
        }
        else {
            return -1;
        }

        if(DataLen){
            if (hex == 1) {
                convertHexToReadable(offset, DataLen, tmpBuf, sizeof(tmpBuf));

                strcpy_s(rdBuff, strlen(tmpBuf) + 1, tmpBuf);
            }
            else {
                strcpy_s(rdBuff, strlen(offset) + 1, offset);
            }
        }
        return S_OK;
    }
    
    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_UART_Write(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UINT32 hex, UCHAR* wrBuff)
{
    const char* cmdFmt = "uartwr %d %d %s\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, idx, hex,  wrBuff);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    ///* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_I2C_Up(DEVICE_DATA_t* UMT_Handle, UCHAR sclPin, UCHAR sdaPin, UINT32 speed) 
{
    const char* cmdFmt = "i2cup %d %d %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, sclPin, sdaPin, speed);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    if (S_OK == UMT_CheckStatus(localBuf, sizeof(localBuf))) {

        const char* targetstr = ">";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        char* positionPtr2 = strstr(localBuf, "\r\n<");

        if (positionPtr2 != nullptr) {
            *positionPtr2 = '\0';
        }
        else {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        return std::stoi(localBuf + offset);

    }
    else {
        return UMT_CheckStatus(localBuf, sizeof(localBuf));
    }

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_I2C_Read(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR addr, CHAR* rdBuff, UINT16 numOfbytes)
{
    const char* cmdFmt = "i2crd %d %d %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];
    CHAR tmpBuf[256];

    if (numOfbytes > 128) {
        numOfbytes = 128;
    }

    sprintf_s(localBuf, 512, cmdFmt, idx, addr, numOfbytes);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;
    }

    if (S_OK == UMT_CheckStatus(localBuf, sizeof(localBuf))) {

        size_t positionDataLen = 0;

        searchInRawBuffer(localBuf, sizeof(localBuf), "VAL:", &positionDataLen);

        const char* numberstr = localBuf + positionDataLen + strlen("VAL:");

        const char* targetstr = ">";

        char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        size_t DataLen = static_cast<size_t>(atoi(numberstr));

        *positionPtr = 0;
        const char* offset = positionPtr + strlen(targetstr);

        size_t  positionPtr2 = 0;
        if (searchInRawBuffer(localBuf, sizeof(localBuf), "\r\n<", &positionPtr2) == TRUE) {
            localBuf[positionPtr2] = '\0';
        }
        else {
            return -1;
        }

        if (DataLen) {            
             memcpy_s(rdBuff, numOfbytes, offset, DataLen);
            
        }
        return S_OK;
    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_I2C_Write(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR addr, UINT16 numOfbytes, UCHAR* wrBuff)
{
    const char* cmdFmt = "i2cwr %d %d %d %s\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, idx, addr, numOfbytes, wrBuff);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    ///* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_I2C_Down(DEVICE_DATA_t* UMT_Handle, UCHAR adcPin)
{
    const char* cmdFmt = "i2cdown %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, adcPin);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    ///* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}


UMTDLL_DECLDIR HRESULT __stdcall UMT_ADC_Up(DEVICE_DATA_t* UMT_Handle, UCHAR adcPin)
{
    const char* cmdFmt = "adcup %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, cmdFmt, adcPin);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_Up(DEVICE_DATA_t* UMT_Handle, UCHAR tckPin, UCHAR tmsPin, UCHAR tdoPin, UCHAR tdiPin, UCHAR pgcPin, UCHAR pgdPin, UCHAR mclrPin)
{
    const char* cmdFmt = "tapup %d %d %d %d %d %d %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, tckPin, tmsPin, tdoPin, tdiPin, pgcPin, pgdPin, mclrPin);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    if (S_OK == UMT_CheckStatus(localBuf, sizeof(localBuf))) {

        const char* targetstr = ">";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        char* positionPtr2 = strstr(localBuf, "\r\n<");

        if (positionPtr2 != nullptr) {
            *positionPtr2 = '\0';
        }
        else {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        return std::stoi(localBuf + offset);

    }
    else {
        return UMT_CheckStatus(localBuf, sizeof(localBuf));
    }

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_DevId(DEVICE_DATA_t* UMT_Handle, UINT32 idx, CHAR* devID)
{
    const char* cmdFmt = "tapdevid %d\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, idx);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    if (S_OK == UMT_CheckStatus(localBuf, sizeof(localBuf))) {

        const char* targetstr = ">";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        char* positionPtr2 = strstr(localBuf, "\r\n<");

        if (positionPtr2 != nullptr) {
            *positionPtr2 = '\0';
        }
        else {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        strcpy_s(devID, strlen(localBuf), (localBuf + offset));

        return S_OK;

    }
    else {
        return UMT_CheckStatus(localBuf, sizeof(localBuf));
    }

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_Flash(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UINT32 addr,UINT32 fwoffset, UINT32 memoffset, UCHAR* testfirmware)
{
    const char* cmdFmt = "tapflash %d %d %d %d %s\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt, idx, addr, fwoffset, memoffset,testfirmware);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));


}


UMTDLL_DECLDIR HRESULT __stdcall UMT_ADC_Get(DEVICE_DATA_t* UMT_Handle, UCHAR txPin, UINT16 *adcCount)
{
    const char* cmdFmt = "adcget %d\r\n";
    #define ADC_STRING "ADC Count ="

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, cmdFmt, txPin);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

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
        return UMT_CheckStatus(localBuf, sizeof(localBuf));
    }    

    return UMT_CheckStatus(localBuf, sizeof(localBuf));
}

UMTDLL_DECLDIR HRESULT __stdcall UMT_TMOD_Write(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR* addr, UCHAR* wrBuff)
{
    const char* cmdFmt = "taptmodwr %d %s %s\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];
    UCHAR addrHex[20] = {};
    UCHAR wrBuffHex[20] = {};

    hexToDecimal(addr, addrHex, sizeof(addrHex));
    hexToDecimal(wrBuff, wrBuffHex, sizeof(wrBuffHex));

    sprintf_s(localBuf, 512, cmdFmt, idx, addrHex, wrBuffHex);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    ///* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;

    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_TMOD_Read(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR* addr, CHAR* rdBuff, UINT16 numOfbytes)
{
    const char* cmdFmt = "taptmodrd %d %s\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];
    //CHAR tmpBuf[256];
    UCHAR addrHex[20];

    if (numOfbytes > 128) {
        numOfbytes = 128;
    }

    hexToDecimal(addr, addrHex, sizeof(addrHex));

    sprintf_s(localBuf, 512, cmdFmt, idx, addrHex);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        if (!ReadFromBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkInPipe, sizeof(localBuf), localBuf, &cbRead))
            return -1;
    }

    if (S_OK == UMT_CheckStatus(localBuf, sizeof(localBuf))) {

        const char* targetstr = ">";

        const char* positionPtr = strstr(localBuf, targetstr);

        if (positionPtr == nullptr) {
            return -1;
        }

        size_t  positionPtr2 = 0;

        if (searchInRawBuffer(localBuf, sizeof(localBuf), "\r\n<", &positionPtr2) == TRUE) {
            localBuf[positionPtr2] = '\0';
        }
        else {
            return -1;
        }

        size_t offset = (positionPtr - localBuf) + strlen(targetstr);

        const CHAR* inputstart = localBuf + offset;

        strcpy_s(rdBuff, strlen(inputstart) + 1, inputstart);

        return S_OK;
    }

    return UMT_CheckStatus(localBuf, sizeof(localBuf));

}

UMTDLL_DECLDIR HRESULT __stdcall UMT_Reset(DEVICE_DATA_t* UMT_Handle)
{
    const char* cmdFmt = "reset\r\n";

    ULONG cbSent = 0;
    ULONG cbRead = 0;
    CHAR localBuf[512];

    sprintf_s(localBuf, 512, cmdFmt);

    if (!WriteToBulkEndpoint(UMT_Handle->WinusbHandle, &UMT_Handle->BulkOutPipe, (PUCHAR)&localBuf, (ULONG)strlen(localBuf), &cbSent))
        return -1;

    /* Sent complete command bytes */
    if (cbSent == (ULONG)strlen(localBuf))
    {

        return S_OK;

    }
    else {
        return -1;
    }


}