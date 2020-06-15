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

