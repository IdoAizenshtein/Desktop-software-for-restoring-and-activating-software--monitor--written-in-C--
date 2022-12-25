#ifndef LimitSingleInstance_H
#define LimitSingleInstance_H

#include <windows.h>
class CLimitSingleInstance
{
protected:
    DWORD  m_dwLastError;
    HANDLE m_hMutex;

public:
    CLimitSingleInstance(TCHAR *strMutexName)
    {
        m_hMutex = CreateMutex(NULL, FALSE, strMutexName); //do early
        m_dwLastError = GetLastError(); //save for use later...
    }

    ~CLimitSingleInstance()
    {
        if (m_hMutex)  //Do not forget to close handles.
        {
            CloseHandle(m_hMutex); //Do as late as possible.
            m_hMutex = NULL; //Good habit to be in.
        }
    }

    BOOL IsAnotherInstanceRunning()
    {
        return (ERROR_ALREADY_EXISTS == m_dwLastError);
    }
};
#endif