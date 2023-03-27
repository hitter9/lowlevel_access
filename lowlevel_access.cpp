#include <iostream>
#include <Windows.h>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;

void ReadCluster(HANDLE disk, BYTE buffer[], int NumberCluster, int ClusterSize)
{
    int StartOffset = 3145728;
    LARGE_INTEGER Offset;
    Offset.QuadPart = (__int64)StartOffset + (__int64)NumberCluster * (__int64)ClusterSize;
    auto p = SetFilePointer(disk, Offset.LowPart, &Offset.HighPart, FILE_BEGIN);
    DWORD ReadByte;
    auto ReadResult = ReadFile(disk, buffer, ClusterSize, &ReadByte, NULL);
    if (!ReadResult || ClusterSize != ReadByte)
    {
        cout << "Ошибка чтения кластера" << endl;
        system("pause");
    }

}

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
    int FirstBitmapSec = pBootRecord->FirstBitmapSector;
    cout << "Первый сектор битовой карты: " << pBootRecord->FirstBitmapSector << endl;
    cout << "Количество кластеров: " << pBootRecord->NumberCluster << endl;
    cout << "Кластер, в котором начинается корневой каталог: " << pBootRecord->RootDirCluster << endl;
    cout << "Серийный номер: " << pBootRecord->SerialNumber << endl;
    int SecSize = pow(2, int(pBootRecord->SectorSize));
    cout << "Размер сектора: " << pow(2, int(pBootRecord->SectorSize)) << endl;
    int ClustMulti = pow(2, pBootRecord->ClusterMultiplier);
    cout << "Кластерный множитель: " << pow(2, pBootRecord->ClusterMultiplier) << endl;
    int ClusSize = pow(2, int(pBootRecord->SectorSize)) * pow(2, pBootRecord->ClusterMultiplier);
    cout << "Размер кластера: " << pow(2, int(pBootRecord->SectorSize)) * pow(2, pBootRecord->ClusterMultiplier) << endl;
    int StartOffset = 6144 * SecSize;
    BYTE* Buffer = new BYTE[ClusSize];
    cout << "Введите номер кластера, который хотите прочитать" << endl;
    int ClusterNumber;
    cin >> ClusterNumber;
    ReadCluster(opendisk, Buffer, ClusterNumber, ClusSize);
    for (int i = 0; i < ClusSize; i++)
    {
        cout << hex << setw(2) << setfill('0') << uppercase << int(Buffer[i]) << " ";
        if ((i + 1) % 16 == 0)
            cout << endl;
    }
    delete[] Buffer;
    delete[] dataBuffer;
    CloseHandle(opendisk);
    system("pause");
    return 0;
}