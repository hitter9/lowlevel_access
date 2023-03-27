#include <iostream>
#include <Windows.h>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;



int main()
{
    setlocale(LC_ALL, "Russian");
    auto filename = L"\\\\.\\D:";
    auto opendisk = CreateFileW(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (opendisk == INVALID_HANDLE_VALUE)
    {
        cout << "Ошибка открытия диска" << endl;
        return 1;
    }
    BYTE* dataBuffer = new BYTE[512];
    DWORD readBytes = 512;
    DWORD readedBytes;
    auto readResult = ReadFile(
        opendisk,
        dataBuffer,
        readBytes,
        &readedBytes,
        NULL
    );
    if (!readResult || readedBytes != readBytes)
    {
        cout << "Ошибка чтения диска" << endl;
        return 2;
    }
#pragma pack(push, 1)
    typedef struct
    {
        BYTE jump[3];
        BYTE name[8];
        BYTE reserved[61];
        UINT64 NumberSector;
        UINT32 FirstFATSector;
        UINT32 SectorsSizeFAT;
        UINT32 FirstBitmapSector;
        UINT32 NumberCluster;
        UINT32 RootDirCluster;
        UINT32 SerialNumber;
        BYTE reserved2[4];
        BYTE SectorSize;
        BYTE ClusterMultiplier;
    } BOOT_exFAT;
#pragma pack(pop)
    BOOT_exFAT* pBootRecord;
    pBootRecord = (BOOT_exFAT*)dataBuffer;
    cout << "Файловая система тома: " << pBootRecord->name << endl;
    cout << "Количество секторов: " << pBootRecord->NumberSector << endl;
    cout << "Сектор, с которого начинается FAT: " << pBootRecord->FirstFATSector << endl;
    cout << "Размер FAT в секторах: " << pBootRecord->SectorsSizeFAT << endl;
    cout << "Первый сектор битовой карты: " << pBootRecord->FirstBitmapSector << endl;
    cout << "Количество кластеров: " << pBootRecord->NumberCluster << endl;
    cout << "Кластер, в котором начинается корневой каталог: " << pBootRecord->RootDirCluster << endl;
    cout << "Серийный номер: " << pBootRecord->SerialNumber << endl;
    cout << "Размер сектора: " << pow(2, int(pBootRecord->SectorSize)) << endl;
    cout << "Кластерный множитель: " << pow(2, pBootRecord->ClusterMultiplier) << endl;
    cout << "Размер кластера: " << pow(2, int(pBootRecord->SectorSize)) * pow(2, pBootRecord->ClusterMultiplier) << endl;

    delete[] dataBuffer;
    CloseHandle(opendisk);
    system("pause");
    return 0;
}