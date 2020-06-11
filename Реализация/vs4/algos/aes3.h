#define R(v,n)(((v)>>(n))|((v)<<(32-(n)))) // циклический сдвиг вправо на n
#define F(n)for(i=0;i<n;i++)
typedef unsigned char B;
typedef unsigned int W;
// Multiplication over GF(2**8)
W M(W x){
    W t=x&0x80808080;
    return((x^t)*2)^((t>>7)*27);
}
// SubByte
B S(B x){
    B i,y,c;
    if(x){
      for(c=i=0,y=1;--i;y=(!c&&y==x)?c=1:y,y^=M(y));
      x=y;F(4)x^=y=(y<<1)|(y>>7);
    }
    return x^99;
}
void E(B *s){
    W i,w,x[8],c=1,*k=(W*)&x[4];
    
    // copy plain text + master key to x
    F(8)x[i]=((W*)s)[i];

    for(;;){
      // AddRoundKey, 1st part of ExpandRoundKey
      w=k[3];F(4)w=(w&-256)|S(w),w=R(w,8),((W*)s)[i]=x[i]^k[i];

      // 2nd part of ExpandRoundKey
      w=R(w,8)^c;F(4)w=k[i]^=w;

      // if round 11, stop;
      if(c==108)break;
      // update round constant
      c=M(c);
      
      // SubBytes and ShiftRows
      F(16)((B*)x)[(i%4)+(((i/4)-(i%4))%4)*4]=S(s[i]);

      // if not round 11, MixColumns
      if(c!=108)
        F(4)w=x[i],x[i]=R(w,8)^R(w,16)^R(w,24)^M(R(w,8)^w);
    }
}


const void* get_func(void* page) {

	void* place = page;

	const byte init[] = {
		0x60,									// pusha
		//0x50, 0x53, 0x51, 0x52, 0x56, 0x57, 0x55, // push eax, ebx, ecx, edx, esi, edi, ebp // нов
		0x31, 0xC9,								// xor    ecx,ecx
		0xF7, 0xE1,								// mul    ecx
		0x40,									// inc    eax
		0xB1, 0x04, 							// mov    cl,0x4
		0x60,									// pusha
		0x8B, 0x74, 0x24, 0x44,					// mov    esi,[esp+0x44]
		0x89, 0xE7,								// mov    edi,esp
		0x60,									// pusha
		0x01, 0xC9,								// add    ecx,ecx
		0xF3, 0xA5,								// rep movsd
		0x61									// popa
	};

	const byte mult[] = {
		//0xE8, 0x10, 0x00, 0x00, 0x00,	// call   15 <_main+0x15>
		0x51,							// push   ecx
		0xB1, 0x04,						// mov    cl,0x4
		//0xB5, 0x00,						// mov ch, 0 // нов
		0x00, 0xC0,						// add    al,al
		0x73, 0x02,						// jae    $ + 4
		0x34, 0x1B,						// xor    al,0x1b
		0xC1, 0xC8, 0x08,				// ror    eax,0x8
		0xE2, 0xF5,						// loop   $ - 9
		0x59,							// pop    ecx
		0xC3,							// ret
		//0x5D							// pop    ebp
	};

	byte sub_byte1[] = {
		0x60,								// pusha
		0x84, 0xC0,							// test   al,al
		0x74, 0x34,							// je     sub_byte5
		0x92,								// xchg   edx,eax
		0xB1, 0xFF,							// mov    cl,0xff
	};

	const byte sub_byte2[] = {
		0xB0, 0x01,							// mov    al,0x1
		0x84, 0xE4,							// test   ah,ah
		0x75, 0x07,							// jne    sub_byte3
		0x38, 0xD0,							// cmp    al,dl
		0x0F, 0x94, 0xC4,					// sete   ah
		0x74, 0xF3,							// je     sub_byte2
	};








	byte sub_byte3[] = {
		0x88, 0xC6,							// mov    dh,al
		0xFF, 0xD5,							// call   ebp
		0x30, 0xF0,							// xor    al,dh
		0xE2, 0xED,							// loop   sub_byte2 ( + 2)
		0x88, 0xC2,							// mov    dl,al
		0xB1, 0x04,							// mov    cl,0x4
											// <--------------------|
		0xD0, 0xC2,							// rol    dl,1          |
		0x30, 0xD0,							// xor    al,dl         |
		0xE2, 0xFA,							// loop ----------------|
	};

	//byte i1 = (byte) (sizeof(sub_byte3) + sizeof(sub_byte2) + 3);
	//byte i2 = (byte) ( 0 - (int) sizeof(sub_byte2) - 6 );

	//sub_byte1[4] = i1;
	//sub_byte3[7] = i2;

	const byte sub_byte5[] = {
		0x34, 0x63,							// xor    al,0x63
		0x88, 0x44, 0x24, 0x1C,				// mov    [esp+0x1c],al
		0x61,								// popa
		0xC3								// ret
	};

	const byte b1[] = {
		0x60,								// pusha
		0x92,								// xchg   edx,eax
		0x87, 0xFE,							// xchg   esi,edi
		0x8B, 0x46, 0x1C,					// mov    eax, [esi+0x1c]
		0xC1, 0xC8, 0x08,					// ror    eax,0x8
	};

	const byte b2[] = {
											//   <---------------------|
		0x8B, 0x5E, 0x10,					// mov    ebx, [esi+0x10]  |
		0x31, 0x1E,							// xor    [esi],ebx        |
		0xA5,								// movsd                   |
		0xE8, 0xFC, 0xFF, 0xFF, 0xFF,		// call   ?                |
		0xC1, 0xC8, 0x08,					// ror    eax,0x8    
	};
		
	byte b3[] = {
		0xE2, 0xF0,							// loop   -----------------|
		0x31, 0xD0,							// xor    eax,edx
		0xB1, 0x04,							// mov    cl,0x4
											//   <---------------------|
		0x31, 0x06,							// xor    [esi],eax        |
		0xAD,								// lodsd                   |
		0xE2, 0xFB,							// loop   -----------------|
		0x61								// popa
	};

	b3[1] = (byte) (0 - sizeof(b2) - 2);

	//const byte fin[] = {
	//	0x5D, 0x5F, 0x5E, 0x5A, 0x59, 0x5B, 0x58 // pop ebp, edi, esi, edx, ecx, ebx, eax
	//};

	//memcpy(place, init, sizeof(init));
	//place = (byte*) place + sizeof(init);

	//memcpy(place, mult, sizeof(mult));
	//place = (byte*) place + sizeof(mult);

	//memcpy(place, fin, sizeof(fin));
	//place = (byte*) place + sizeof(fin);

	//memcpy(place, sub_byte1, sizeof(sub_byte1));
	//place = (byte*) place + sizeof(sub_byte1);

	//memcpy(place, sub_byte2, sizeof(sub_byte2));
	//place = (byte*) place + sizeof(sub_byte2);

	//memcpy(place, sub_byte3, sizeof(sub_byte3));
	//place = (byte*) place + sizeof(sub_byte3);

	//memcpy(place, sub_byte4, sizeof(sub_byte4));
	//place = (byte*) place + sizeof(sub_byte4);

	//memcpy(place, sub_byte5, sizeof(sub_byte5));
	//place = (byte*) place + sizeof(sub_byte5);

	memcpy(place, b1, sizeof(b1));
	place = (byte*) place + sizeof(b1);

	memcpy(place, b2, sizeof(b2));
	place = (byte*) place + sizeof(b2);

	memcpy(place, b3, sizeof(b3));
	place = (byte*) place + sizeof(b3);

	return page;
}

#include "windows.h"

void aes_asm (const byte plaintext[16], const byte key[16], byte cipher[16])  {
	typedef byte* (*cryptofunc)(const byte*,const byte*);

	void* page = VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	const cryptofunc func = (cryptofunc) get_func(page);

	cipher = func(plaintext, key);

	VirtualFree(page, 4096, MEM_RELEASE);
}