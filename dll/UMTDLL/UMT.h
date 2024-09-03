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


#define SUCCUSS	"SUCCESS"
#define FAILURE	"FAIL"


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

UMTDLL_DECLDIR HRESULT UMT_Initialize();

UMTDLL_DECLDIR HRESULT UMT_Deinitialize(void);

UMTDLL_DECLDIR HRESULT UMT_GetDeviceInfo(UINT32 devIdx, DEVICE_DATA_t* deviceInfo);

UMTDLL_DECLDIR HRESULT UMT_SetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum, UCHAR highLow);

UMTDLL_DECLDIR HRESULT UMT_GetGPIO(DEVICE_DATA_t* UMT_Handle, UCHAR pinNum);

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif