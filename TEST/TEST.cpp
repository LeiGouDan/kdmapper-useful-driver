#define UNICODE 1
#define _UNICODE 1

/* The code of interest is in the subroutine GetDriveGeometry. The
   code in main shows how to interpret the results of the call. */

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

typedef struct info_t {
    int pid = 0;
    DWORD_PTR address;
    void* value;
    SIZE_T size;
    void* data;
}info, * p_info;

#define wszDrive L"\\\\.\\u8CI"
#define ctl_write   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0366, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_read    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0367, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_base    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0368, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_clear	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0369, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define ctl_hello    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0367, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

HANDLE GetDrive(LPCWSTR wszPath)
{
    p_info out = { 0 };
    HANDLE hDevice = INVALID_HANDLE_VALUE;  
    BOOL bResult = FALSE;                
    DWORD junk = 0;                    

    hDevice = CreateFileW(wszPath,          
        GENERIC_READ | GENERIC_WRITE,               
        FILE_SHARE_READ | 
        FILE_SHARE_WRITE,
        NULL,             
        OPEN_EXISTING,    
        0,               
        NULL);          

    if (hDevice == INVALID_HANDLE_VALUE)   
    {
        return (FALSE);
    }

    bResult = DeviceIoControl(hDevice,                       // device to be queried
        ctl_hello, // operation to perform
        NULL, 0,                       // no input buffer
        out, sizeof(*out),            // output buffer
        &junk,                         // # bytes returned
        (LPOVERLAPPED)NULL);          // synchronous I/O

    //CloseHandle(hDevice);

    return (hDevice);
}

BOOL Handle(HANDLE hDevice, DWORD code,p_info out, p_info in)
{
    DWORD junk = 0;
    return DeviceIoControl(hDevice,
        code,
        in, sizeof(*in),
        out, sizeof(*out),
        &junk,
        (LPOVERLAPPED)NULL);
}

int main(int argc, wchar_t* argv[])
{
    p_info out = { 0 };
    p_info in = { 0 };
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    ULONGLONG DiskSize = 0;

    hDevice = GetDrive(wszDrive);

    if (hDevice != INVALID_HANDLE_VALUE)
    {
        in->pid = 2184;
        Handle(hDevice, ctl_base, out, in);
        wprintf(L"base address %p.\n", out->data);
    }
    else
    {
        wprintf(L"GetDrive failed. Error %ld.\n", GetLastError());
    }

    return ((int)hDevice);
}