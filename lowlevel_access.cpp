#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{
    setlocale(LC_ALL, "Ru");
    auto filename = L"\\\\.\\C:";
    auto opendisk = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (opendisk == INVALID_HANDLE_VALUE)
    {
        cout << "Ошибка открытия диска" << endl;
        return 1;
    }
    BYTE* dataBuffer = new BYTE[512];
    DWORD readBytes = 512;
    DWORD readedBytes;
    auto readResult = ReadFile(opendisk, dataBuffer, readBytes, &readedBytes, NULL);
    if (!readResult || readedBytes != readBytes)
    {
        cout << "Ошибка чтения диска" << endl;
        return 2;
    }
    for (int i = 0; i < readedBytes; i++)
        cout << dataBuffer[i];
    cout << endl;
    delete[] dataBuffer;
    CloseHandle(opendisk);
    system("pause");
    return 0;
}