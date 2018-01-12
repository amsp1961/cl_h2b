//
// Created by Сергей on 01.07.2017.
//

#include <windows.h>
#include <string>
#include "COM_port.h"

using namespace std;

com_port::com_port(const std::string &name) {
    try {
        DCB COMMdcb;
        COMMTIMEOUTS cTo;

        // открыт файл COM
        if ((hComm = CreateFile(("\\\\.\\" + name).c_str(),   //указатель на файл
                                GENERIC_READ | GENERIC_WRITE, // доступ к чтению записи
                                0,
                /* exclusive access  */ //
                                NULL,
                /* no security attrs */ //
                                OPEN_EXISTING,          // как создать
                                0, // FILE_FLAG_OVERLAPPED,       // атрибуты файла
                                NULL // идентификатор файла,куда копируются свойства
        )) == INVALID_HANDLE_VALUE) //объект не создался
            //				  throw std::string("COM_port::CreateFile: ") + str_utf8(GetError()).utf8(); //выкидываем  свои
            //"исключения"
            throw std::string("COM_port::CreateFile: ") + GetError(); //выкидываем  свои "исключения"
        PurgeComm(hComm, PURGE_TXCLEAR | PURGE_RXCLEAR);
        Sleep(100);
        PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT);
        Sleep(100);

        // настройка параметров порта COM1
        if (!GetCommState(hComm, &COMMdcb)) //возвращается информация освязующем устройстве(струк-ра DCB)
            throw std::string("COM_port::GetCommState: ") + GetError(); //выкидываем  свои "исключения"

        //(струк-ра DCB)
        COMMdcb.BaudRate = CBR_9600; //текущая скорость
        COMMdcb.fBinary = TRUE;
        COMMdcb.fOutxCtsFlow = FALSE;
        COMMdcb.fDsrSensitivity = FALSE;
        COMMdcb.fOutxDsrFlow = FALSE;
        COMMdcb.fDtrControl = FALSE;
        COMMdcb.fRtsControl = RTS_CONTROL_DISABLE;
        COMMdcb.fOutX = FALSE;
        COMMdcb.fInX = FALSE;
        COMMdcb.fErrorChar = FALSE;
        COMMdcb.fNull = FALSE;
        COMMdcb.fAbortOnError = FALSE;
        COMMdcb.fParity = FALSE;           // разрешение проверки бита четности
        COMMdcb.Parity = NOPARITY;         // значение бита четности
        COMMdcb.ByteSize = 8;              // размер байта
        COMMdcb.StopBits = ONESTOPBIT;     // кол-во стоп-битов(конец посылки)
        if (!SetCommState(hComm, &COMMdcb)) // заполняется структура  DCB
            throw std::string("SetCommState: " + GetError()); // выкидываем  свои "исключения"

        if (!GetCommTimeouts(hComm, &cTo)) // возвращается информация time-out(струк-ра LPCOMMTIMEOUTS)
            throw std::string("GetCommTimeouts: " + GetError()); // выкидываем  свои "исключения"

        cTo.ReadIntervalTimeout = 0;
        cTo.ReadTotalTimeoutMultiplier = 2; // период для чтения результата вычислений с учетом кол-ва байт (~10мс)
        cTo.ReadTotalTimeoutConstant =
                100;                          // запас времени для чтения результата вычислений(см. ReadTotalTimeoutMultiplier)
        if (!SetCommTimeouts(hComm, &cTo)) //заполняется структура  LPCOMMTIMEOUTS
            throw std::string("SetCommTimeouts: " + GetError()); //выкидываем  свои "исключения"
    }catch (std::string e) {
        CloseHandle(hComm);
        throw e + ": " + name;
    }catch (...) {
        CloseHandle(hComm); //закрываем СОМ1
        throw std::string("Ошибка открытия COM порта: ") + name;
    }
}

std::string com_port::GetError(void) {
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  (LPTSTR) &lpMsgBuf,
                  0,
                  NULL);
    std::string ret = static_cast<TCHAR *>(lpMsgBuf);
    LocalFree(lpMsgBuf);

    return ret;
}

com_port::~com_port() {
    CloseHandle(hComm); //закрываем СОМ
}

void com_port::SetSleep(DWORD tt) {
    COMMTIMEOUTS cTo;

    GetCommTimeouts(hComm, &cTo); //возвращается информация time-out(струк-ра LPCOMMTIMEOUTS)
    cTo.ReadTotalTimeoutConstant = tt; // запас времени для чтения результата вычислений(см. ReadTotalTimeoutMultiplier)
    SetCommTimeouts(hComm, &cTo);      //заполняется структура  LPCOMMTIMEOUTS
}

void com_port::WriteFile(const unsigned char *src, const unsigned long ln) {
    DWORD wtn;
    if (::WriteFile(hComm, (LPCVOID) src, (DWORD) ln, &wtn, 0) == FALSE) {
        LPVOID lpMsgBuf;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &lpMsgBuf,
                      0,
                      NULL);

        // Display the string.
        MessageBox(NULL, (const TCHAR *) lpMsgBuf, "GetLastError", MB_OK | MB_ICONINFORMATION);

        // Free the buffer.
        LocalFree(lpMsgBuf);
    }
    if (wtn != ln)
        throw std::string("Ошибка в WriteFile");
}

void com_port::ReadFile(unsigned char *buf, unsigned long limit) {
    DWORD rdd;
    if (::ReadFile(hComm, buf, limit, &rdd, 0) == FALSE) {
        LPVOID lpMsgBuf;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      GetLastError(),
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &lpMsgBuf,
                      0,
                      NULL);

        // Display the string.
        MessageBox(NULL, (const TCHAR *) lpMsgBuf, "GetLastError", MB_OK | MB_ICONINFORMATION);

        // Free the buffer.
        LocalFree(lpMsgBuf);
    }
    if (rdd != limit)
        throw std::string("Ошибка в ReadFile");
}

unsigned int com_port::read(unsigned char *buf, unsigned int amnt) {
    DWORD rdd;
    if (!::ReadFile(hComm, buf, amnt, &rdd, NULL))
        throw std::string("Ошибка в ф-ии COM_port::read");
    return rdd;
}

//---------------------------------------------------------------------------
// определяем кол-во байтов в COM порту для чтения
//---------------------------------------------------------------------------
DWORD com_port::BytesInReadBuf(void) {
    DWORD rdd;
    COMSTAT lc;
    ::ClearCommError(hComm, &rdd, &lc);
    return (lc.cbInQue);
}

//---------------------------------------------------------------------------
// чистим COM порт
//---------------------------------------------------------------------------
void com_port::ClearRxBuf(void) {
    ::PurgeComm(hComm, PURGE_RXCLEAR);
}

void com_port::SetBoud(unsigned long boud) {
    DCB COMMdcb;
    if (!GetCommState(hComm, &COMMdcb)) //возвращается информация о связующем устройстве(струк-ра DCB)
        throw std::string("GetCommState: " + GetError()); //выкидываем  свои "исключения"

    COMMdcb.BaudRate = boud;                              //текущая скорость
    if (!SetCommState(hComm, &COMMdcb))                    //заполняется структура  DCB
        throw std::string("SetCommState: " + GetError()); //выкидываем  свои "исключения"
}

DCB com_port::GetDCB(void) {
    DCB dcb;
    GetCommState(hComm, &dcb);
    return dcb;
}
