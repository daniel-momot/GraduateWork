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

	/*const byte init[] = {
		0x00, 0x00,
		0x60,									// pusha
		0x8D, 0x74, 0x24, 0x24,					// lea    esi,[esp+0x24]
		0xAD,									// lodsd
		0x91,									// xchg   ecx,eax
		0xAD,									// lodsd
		0x95,									// xchg   ebp,eax
		0xAD,									// lodsd
		0x92,									// xchg   edx,eax
		0xAD,									// lodsd
		0x96,									// xchg   esi,eax
		0x60,									// pusha
		0x8D, 0x7C, 0x24, 0x10,					// lea    edi,[esp+0x10]
		0xA5,									// movsd
		0xA5,									// movsd
		0xA5,									// movsd
		0xA5									// movsd
	};

	const byte jmp1 = 0;
	const byte jmp2 = 0;
	const byte jmp3 = 0;

	const byte aes_l0[] = {
		0x31, 0xC0,								// xor    eax,eax
		0xE3, 0xFF,								// jecxz  3 <_main+0x3>
		0x89, 0xE7,								// mov    edi,esp
		0x89, 0xEE,								// mov    esi,ebp
		0x57,									// push   edi
		0xA5,									// movsd
		0xA5,									// movsd
		0xA5,									// movsd
		0xA5,									// movsd
		0xE8, 0xFC, 0xFF, 0xFF, 0xFF,			// call   e <_main+0xe>
		0x5F									// pop    edi
	};

	const byte aes_11[] = {
		0x8A, 0x1C, 0x07,						// mov    bl, [edi+eax]
		0x30, 0x1A,								// xor    [edx],bl
		0x42,									// inc    edx
		0x40,									// inc    eax
		0x3C, 0x10,								// cmp    al,0x10
		0xE0, 0xFF,								// loopne a <_main+0xa>
		0x91,									// xchg   ecx,eax
		0xB1, 0x10 								// mov    cl,0x10
	};

	const byte aes_12[] = {
		0x00,									// ???
		0x00,									// ???
		0x91,									// xchg   ecx,eax
		0xE9, 0x00, 0x00, 0x00, 0x00			// jmp    6 <_main+0x6>
	};

	const byte aes_13[] = {
		0x61,									// popa
		0x61,									// popa
		0xC3									// ret
	};

	const byte* srcs[] = { init, aes_l0, aes_11, aes_12, aes_13 };

	const size_t size_all = sizeof(init) + sizeof(aes_l0) + sizeof(aes_11) + sizeof(aes_12) + sizeof(aes_13);
	byte* func = (byte*) malloc(size_all);

	memcpy(func, init, sizeof(init));
	memcpy(func + sizeof(init), aes_l0, sizeof(aes_l0));
	memcpy(func + sizeof(init) + sizeof(aes_l0), aes_11, sizeof(aes_11));
	memcpy(func + sizeof(init) + sizeof(aes_l0) + sizeof(aes_11), aes_12, sizeof(aes_12));
	memcpy(func + sizeof(init) + sizeof(aes_l0) + sizeof(aes_11) + sizeof(aes_12), aes_13, sizeof(aes_13));

	return func;*/

	void* place = page;

	const byte init[] = {
		//0x60,									// pusha
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

	memcpy(place, init, sizeof(init));
	place = (byte*) place + sizeof(init);

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