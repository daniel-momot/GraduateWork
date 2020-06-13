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

	const byte tmp[] = {

		0x8B, 0x04, 0x24


	};


	const byte code[] = {
		0x60,									// pusha
		0x31, 0xC9,								// xor    ecx,ecx
		0xF7, 0xE1,								// mul    ecx
		0x40,									// inc    eax
		0xB1, 0x04,								// mov    cl,0x4
		0x60,									// pusha
		0x8B, 0x74, 0x24, 0x44,					// mov    esi,[esp+0x44]
		0x89, 0xE7,								// mov    edi,esp
		0x60,									// pusha
		0x01, 0xC9,								// add    ecx,ecx
		0xF3, 0xA5,								// rep movs
		0x61,									// popa

												// AddRoundKey, AddRoundConstant, ExpandRoundKey

		0x60,									// pusha  (xxxx)
		0x92,									// xchg   edx,eax
		0x87, 0xFE,								// xchg   esi,edi
		0x8B, 0x46, 0x1C,						// mov    eax,[esi+0x1c]
		0xC1, 0xC8, 0x08,						// ror    eax,0x8
		0x8B, 0x5E, 0x10,						// mov    ebx,[esi+0x10]  (|)
		0x31, 0x1E,								// xor    [esi],ebx
		0xA5,									// movs   

		0xE8, 0x00, 0x00, 0x00, 0x00,			// call   0
		0x5D,									// pop    ebp
		0x83, 0xC5, 0x06,						// add    ebp,0x6
		0xEB, 0x7D,								// jmp    (subb)

		0xC1, 0xC8, 0x08,						// ror    eax,0x8
		0xE2, 0xF0,								// loop   (|)
		0x31, 0xD0,								// xor    eax,edx
		0xB1, 0x04,								// mov    cl,0x4
		0x31, 0x06,								// xor    [esi],eax (||)
		0xAD,									// lods   
		0xE2, 0xFB,								// loop   (||)
		0x61,									// popa
		0x3C, 0x6C,								// cmp    al,0x6c
		0x0F, 0x85, 0x03, 0x00, 0x00, 0x00,		// jne    (x)
		0x61,									// popa
		0x61,									// popa
		0xC3,									// ret

		0xE8, 0x00, 0x00, 0x00, 0x00,			// call   0
		0x5D,									// pop    ebp
		0x83, 0xC5, 0x09,						// add    ebp,0x9
		0xE9, 0xC3, 0x00, 0x00, 0x00,			// jmp    (mult)

												// ShiftRows and SubBytes

		0x60,									// pusha
		0xB1, 0x10,								// mov    cl,0x10
		0xAC,									// lods   (xx)

		0xE8, 0x00, 0x00, 0x00, 0x00,			// call   0
		0x5D,									// pop    ebp
		0x83, 0xC5, 0x06,						// add    ebp,0x6
		0xEB, 0x46,								// jmp    (subb)

		0x52,									// push   edx
		0x89, 0xD3,								// mov    ebx,edx
		0x83, 0xE3, 0x03,						// and    ebx,0x3
		0xC1, 0xEA, 0x02,						// shr    edx,0x2
		0x29, 0xDA,								// sub    edx,ebx
		0x83, 0xE2, 0x03,						// and    edx,0x3
		0x8D, 0x1C, 0x93,						// lea    ebx,[ebx+edx*4]
		0x88, 0x04, 0x1F,						// mov    [edi+ebx*1],al
		0x5A,									// pop    edx
		0x42,									// inc    edx
		0xE2, 0xE2,								// loop   (xx)
		0x61,									// popa
		0x3C, 0x6C,								// cmp    al,0x6c
		0x74, 0xA5,								// je     (xxxx)

												// MixColumns

		0x60,									// pusha
		0x8B, 0x07,								// mov    eax,[edi]   (yy)
		0x89, 0xC3,								// mov    ebx,eax
		0xC1, 0xC8, 0x08,						// ror    eax,0x8
		0x89, 0xC2,								// mov    edx,eax
		0x31, 0xD8,								// xor    eax,ebx

		0xE8, 0x00, 0x00, 0x00, 0x00,			// call   0
		0x5D,									// pop    ebp
		0x83, 0xC5, 0x06,						// add    ebp,0x6
		0xEB, 0x4A,								// jmp    (mult)

		0x31, 0xD0,								// xor    eax,edx
		0xC1, 0xCB, 0x10,						// ror    ebx,0x10
		0x31, 0xD8,								// xor    eax,ebx
		0xC1, 0xCB, 0x08,						// ror    ebx,0x8
		0x31, 0xD8,								// xor    eax,ebx
		0xAB,									// stos   
		0xE2, 0xE4,								// loop   (yy)
		0x61,									// popa
		0xEB, 0xDC,								// jmp    (xxxx)

												// SubByte (subb)

		0x60,									// pusha
		0x84, 0xC0,								// test   al,al
		0x74, 0x22,								// je     (kkk)
		0x92,									// xchg   edx,eax
		0xB1, 0xFF,								// mov    cl,0xff
		0xB0, 0x01,								// mov    al,0x1   (zz)
		0x84, 0xE4,								// test   ah,ah    (zzz)
		0x75, 0x07,								// jne    (kkkk)
		0x38, 0xD0,								// cmp    al,dl
		0x0F, 0x94, 0xC4,						// sete   ah
		0x74, 0xF3,								// je     (zz)
		0x88, 0xC6,								// mov    dh,al  (kkkk)

		0xE8, 0x00, 0x00, 0x00, 0x00,			// call   0
		0x5D,									// pop    ebp
		0x83, 0xC5, 0x06,						// add    ebp,0x6
		0xEB, 0x00,								// jmp    ?

		0x30, 0xF0,								// xor    al,dh
		0xE2, 0xED,								// loop   (zzz)
		0x88, 0xC2,								// mov    dl,al
		0xB1, 0x04,								// mov    cl,0x4
		0xD0, 0xC2,								// rol    dl,1   (zzzz)
		0x30, 0xD0,								// xor    al,dl
		0xE2, 0xFA,								// loop   (zzzz)
		0x34, 0x63,								// xor    al,0x63  (kkk)
		0x88, 0x44, 0x24, 0x1C,					// mov    [esp+0x1c],al
		0x61,									// popa
		0xC3,									// ret

												// Multiplication over GF(2**8) (mult)

		0x51,									// push   ecx
		0xB1, 0x04,								// mov    cl,0x4
		0x00, 0xC0,								// add    al,al (*)
		0x73, 0x02,								// jae    (**)
		0x34, 0x1B,								// xor    al,0x1b 
		0xC1, 0xC8, 0x08,						// ror    eax,0x8 (**)
		0xE2, 0xF5,								// loop   (*)

		0xB9, 0x02, 0x00, 0x00, 0x00,			// mov    ecx,0x2
		0x01, 0x4C, 0x24, 0x04,					// add    [esp+0x4],ecx
		0x59,									// pop    ecx
		0xC3,									// ret
	};

	memcpy(page, tmp, sizeof(tmp));
	place = (byte*) place + sizeof(tmp);




	return page;
}

