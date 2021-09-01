// funGost.cpp: определяет точку входа для консольного приложения.
// ГОСТ 28147-89

#include <iostream>
using namespace std;

#define BYTE unsigned char
#define UINT32 unsigned int

bool EncryptData(BYTE * Buf, int BufSt);
bool DecryptData(BYTE * Buf, int BufSt);
void Step_0_7(BYTE * Buf, int BufSt);
void Step_7_0(BYTE * Buf, int BufSt);
void Step(BYTE * Buf, int BufSt, int K);
void Exch(BYTE * Buf, int BufSt);


const BYTE Sb[128] =   //таблица замен - просто константные значения
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
        0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
        0x00, 0x08, 0x01, 0x09, 0x02, 0x0A, 0x03, 0x0B,
        0x04, 0x0C, 0x05, 0x0D, 0x06, 0x0E, 0x07, 0x0F,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
        0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09, 0x08,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F,
        0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E,
        0x06, 0x0A, 0x00, 0x04, 0x0B, 0x01, 0x05, 0x09,
        0x0C, 0x03, 0x07, 0x0E, 0x02, 0x08, 0x0D, 0x0F,
        0x00, 0x01, 0x09, 0x02, 0x08, 0x03, 0x07, 0x04,
        0x06, 0x05, 0x0A, 0x0F, 0x0B, 0x0E, 0x0C, 0x0D

};

const BYTE Key[32] = //ключ - 256 бит
{
        0xA6 , 0xB8 , 0xC9 , 0xD1 ,
        0x6F , 0x94 , 0x47 , 0x25 ,
        0xB8 , 0x41 , 0xC9 , 0xD1 ,
        0x53 , 0x17 , 0x94 , 0xAE ,
        0x12 , 0x34 , 0x05 , 0x67 ,
        0x89 , 0xDA , 0xFC , 0x6E ,
        0x32 , 0x18 , 0x85 , 0x16 ,
        0x73 , 0x33 , 0x76 , 0xB9
};





int main()
{
    BYTE lol[] = "12345678!";
    cout << "Data:" << lol << endl;
    EncryptData(lol, 8); // кодирую первые 8 байт ! оставляю
    cout << "EncryptData:" << lol << endl;
    DecryptData(lol, 8);
    cout << "DecryptData:" << lol << endl;

    return 0;
}

bool EncryptData(BYTE * Buf, int BufSt)
{
    for (int i = 0; i<BufSt; i += 8)
    {
        Step_0_7(Buf, i);
        Step_0_7(Buf, i);
        Step_0_7(Buf, i);
        Step_7_0(Buf, i);
        Exch(Buf, i);
    }
    return true;
}

bool DecryptData(BYTE * Buf, int BufSt)
{
    for (int i = 0; i<BufSt; i += 8)
    {
        Step_0_7(Buf, i);
        Step_7_0(Buf, i);
        Step_7_0(Buf, i);
        Step_7_0(Buf, i);
        Exch(Buf, i);
    }
    return true;
}

void Step_0_7(BYTE * Buf, int BufSt)
{
    for (int K = 3; K<32; K += 4) Step(Buf, BufSt, K);
}
void Step_7_0(BYTE * Buf, int BufSt)
{
    for (int K = 31; K>0; K -= 4) Step(Buf, BufSt, K);
}
void Step(BYTE * Buf, int BufSt, int R0)
{

    union
    {
        UINT32 BufW;
        BYTE Buf_W[4];
    };

    BufW = (Buf[4 + BufSt] << 24 | Buf[5 + BufSt] << 16 | Buf[6 + BufSt] << 8 | Buf[7 + BufSt]) +
           (Key[R0 - 3] << 24 | Key[R0 - 2] << 16 | Key[R0 - 1] << 8 | Key[R0]);


    for (R0 = 0; R0<4; R0++)
    {
        BYTE a = Sb[(((Buf_W[R0] >> 4) & 0xf) + R0 * 0x20 + 0x10) & 0xff];
        Buf_W[R0] = ((a >> 4) & 0xf) | ((a << 4) & 0xf0) | (Sb[((Buf_W[R0] & 0xf) + R0 * 0x20) & 0xff]);
    }


    BufW = (BufW << 11) | (BufW >> 32 - 11);

    Buf_W[3] ^= Buf[0 + BufSt];
    Buf_W[2] ^= Buf[1 + BufSt];
    Buf_W[1] ^= Buf[2 + BufSt];
    Buf_W[0] ^= Buf[3 + BufSt];

    Buf[0 + BufSt] = Buf[4 + BufSt];
    Buf[1 + BufSt] = Buf[5 + BufSt];
    Buf[2 + BufSt] = Buf[6 + BufSt];
    Buf[3 + BufSt] = Buf[7 + BufSt];

    Buf[4 + BufSt] = Buf_W[3];
    Buf[5 + BufSt] = Buf_W[2];
    Buf[6 + BufSt] = Buf_W[1];
    Buf[7 + BufSt] = Buf_W[0];

}
void Exch(BYTE * Buf, int BufSt)
{
    for (int i = 0; i<4; i++)
    {
        BYTE t = Buf[i + 4 + BufSt];
        Buf[i + 4 + BufSt] = Buf[i + BufSt];
        Buf[i + BufSt] = t;
    }
}

