//
// Created by Сергей on 01.07.2017.
//

#ifndef CLIONMYSTEND_COM_PORT_H
#define CLIONMYSTEND_COM_PORT_H

class com_port
{
private:
    HANDLE hComm;

public:
    explicit com_port(const std::string&);
    std::string GetError();
    HANDLE _GetHandle()
    {
        return hComm;
    }
    void SetSleep(DWORD); //время задержки
    void WriteFile(const unsigned char* src, const unsigned long ln);
    void ReadFile(unsigned char* buf, unsigned long limit);
    unsigned int read(unsigned char* buf, unsigned int amnt);
    DWORD BytesInReadBuf();
    void ClearRxBuf();
    void SetBoud(unsigned long boud);
    DCB GetDCB();

    virtual ~com_port();
};

#endif // CLIONMYSTEND_COM_PORT_H
