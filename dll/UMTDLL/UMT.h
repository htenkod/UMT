#pragma once




#if defined UMTDLL_EXPORTS
#define UMTDLL_DECLDIR  __declspec(dllexport)
#else
#define UMTDLL_DECLDIR __declspec(dllimport)
#endif

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C"
{
#endif

typedef struct _DEVICE_DATA {

	BOOL                    HandlesOpen;
	WINUSB_INTERFACE_HANDLE WinusbHandle;
	HANDLE                  DeviceHandle;
	TCHAR                   DevicePath[MAX_PATH];
	UCHAR					SerialNumber[64];
	UCHAR                   BulkOutPipe;
	UCHAR                   BulkInPipe;	
}DEVICE_DATA_t, *PDEVICE_DATA_t;

#define UMT_SITES_MAX	10


#define SUCCESS	"SUCCESS"
#define FAILURE	"FAIL"
#define DONE "DONE!"
#define RESET ""

typedef enum {
	UMT_CMD_GPIO,
	UMT_CMD_UART,
	UMT_CMD_ADC,
	UMT_CMD_I2C,
	UMT_CMD_SPI,
}UMT_CMD_CODE_t;

typedef enum {
	UMT_CMD_OUT,
	UMT_CMD_IN,
}UMT_CMD_DIR_t;


typedef struct
{
	UMT_CMD_CODE_t cmdCode;
	UMT_CMD_DIR_t  cmdDir;
	ULONG32		   frameLen; // length of frame body data in bytes
	CHAR		   frameData[512];
}UMT_FRAME_t;

#define UMT_FRAME_HDR_LEN	(sizeof(UMT_CMD_CODE_t) + sizeof(UMT_CMD_DIR_t) + sizeof(ULONG32))

UMTDLL_DECLDIR HRESULT __stdcall UMT_Initialize();

UMTDLL_DECLDIR HRESULT __stdcall UMT_Deinitialize(void);

UMTDLL_DECLDIR HRESULT __stdcall UMT_GetDeviceInfo(UINT32 devIdx, DEVICE_DATA_t* deviceInfo);

UMTDLL_DECLDIR HRESULT __stdcall UMT_SetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum, UCHAR highLow);

UMTDLL_DECLDIR HRESULT __stdcall UMT_GetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum);

UMTDLL_DECLDIR HRESULT __stdcall UMT_UART_Up(DEVICE_DATA_t* UMT_Handle, UCHAR txPin, UCHAR rxPin, UINT32 baud, UINT32 rdBuffSz, UINT32 wdBuffSz);

UMTDLL_DECLDIR HRESULT __stdcall UMT_UART_Read(DEVICE_DATA_t* UMT_Handle, UINT32 idx, CHAR* rdBuff, UINT32 hex, UINT16 numOfbytes);

UMTDLL_DECLDIR HRESULT __stdcall UMT_UART_Write(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UINT32 hex, UCHAR* wrBuff);

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_Up(DEVICE_DATA_t* UMT_Handle,UCHAR tckPin, UCHAR tmsPin,UCHAR tdoPin, UCHAR tdiPin, UCHAR pgcPin,UCHAR pgdPin,UCHAR mclrPin);

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_DevId(DEVICE_DATA_t* UMT_Handle, UINT32 idx, CHAR* devID);

UMTDLL_DECLDIR HRESULT __stdcall UMT_Tap_Flash(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UINT32 addr, UINT32 fwoffset, UINT32 memoffset, UCHAR* testfirmware);

UMTDLL_DECLDIR HRESULT __stdcall UMT_ADC_Up(DEVICE_DATA_t* UMT_Handle, UCHAR adcPin);

UMTDLL_DECLDIR HRESULT __stdcall UMT_ADC_Get(DEVICE_DATA_t* UMT_Handle, UCHAR txPin, UINT16* adcCount);

UMTDLL_DECLDIR HRESULT __stdcall UMT_TMOD_Write(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR* addr, UCHAR* wrBuff);

UMTDLL_DECLDIR HRESULT __stdcall UMT_TMOD_Read(DEVICE_DATA_t* UMT_Handle, UINT32 idx, UCHAR* addr, CHAR* rdBuff, UINT32 hex, UINT16 numOfbytes);

UMTDLL_DECLDIR HRESULT __stdcall UMT_Reset(DEVICE_DATA_t* UMT_Handle);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif