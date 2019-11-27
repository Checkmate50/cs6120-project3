#include "posit.h"
#include <stdlib.h> /* for lldiv_t */

posit32_t softposit_addMagsP32( uint_fast32_t uiA, uint_fast32_t uiB ) {
	uint_fast16_t regA, regB;
	uint_fast64_t frac64A=0, frac64B=0;
	uint_fast32_t fracA=0, regime, tmp;
	bool sign, regSA, regSB, rcarry=0, bitNPlusOne=0, bitsMore=0;
	int_fast8_t kA=0;
	int_fast32_t expA;
	int_fast16_t shiftRight;
	union ui32_p32 uZ;

	sign = signP32UI( uiA );
	if (sign){
		uiA = -uiA & 0xFFFFFFFF;
		uiB = -uiB & 0xFFFFFFFF;
	}

	if ((int_fast32_t)uiA < (int_fast32_t)uiB){
		uiA ^= uiB;
		uiB ^= uiA;
		uiA ^= uiB;
	}
	regSA = signregP32UI( uiA );
    regSB = signregP32UI( uiB );

    tmp = (uiA<<2)&0xFFFFFFFF;
	if (regSA){
		while (tmp>>31){
			kA++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
	}
	else{
		kA=-1;
		while (!(tmp>>31)){
			kA--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;
	}

	expA = tmp>>29; //to get 2 bits
	frac64A = ((0x40000000ULL | tmp<<1) & 0x7FFFFFFFULL) <<32;
	shiftRight = kA;

	tmp = (uiB<<2) & 0xFFFFFFFF;
	if (regSB){
		while (tmp>>31){
			shiftRight--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
	}
	else{
		shiftRight++;
		while (!(tmp>>31)){
			shiftRight++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;
	}
	frac64B = ((0x40000000ULL | tmp<<1) & 0x7FFFFFFFULL) <<32;
	//This is 4kZ + expZ; (where kZ=kA-kB and expZ=expA-expB)
	shiftRight = (shiftRight<<2) + expA - (tmp>>29);

	//Manage CLANG (LLVM) compiler when shifting right more than number of bits
	(shiftRight>63) ? (frac64B=0): (frac64B >>= shiftRight); //frac64B >>= shiftRight

	frac64A += frac64B;

	rcarry = 0x8000000000000000 & frac64A; //first left bit
	if (rcarry){
		expA++;
		if (expA>3){
			kA ++;
			expA&=0x3;
		}
		frac64A>>=1;
	}
	if(kA<0){
		regA = -kA;
		regSA = 0;
		regime = 0x40000000>>regA;
	}
	else{
		regA = kA+1;
		regSA=1;
		regime = 0x7FFFFFFF - (0x7FFFFFFF>>regA);
	}

	if(regA>30){
		//max or min pos. exp and frac does not matter.
		(regSA) ? (uZ.ui= 0x7FFFFFFF): (uZ.ui=0x1);
	}
	else{
		//remove hidden bits
		frac64A = (frac64A & 0x3FFFFFFFFFFFFFFF) >>(regA + 2) ; // 2 bits exp

		fracA = frac64A>>32;

		if (regA<=28){
			bitNPlusOne |= (0x80000000 & frac64A) ;
			expA <<= (28-regA);
		}
		else {
			if (regA==30){
				bitNPlusOne = expA&0x2;
				bitsMore = (expA&0x1);
				expA = 0;
			}
			else if (regA==29){
				bitNPlusOne = expA&0x1;
				expA>>=1;
			}
			if (fracA>0){
				fracA=0;
				bitsMore =1;
			}
		}

		uZ.ui = packToP32UI(regime, expA, fracA);
		//n+1 frac bit is 1. Need to check if another bit is 1 too if not round to even
		if (bitNPlusOne){
			(0x7FFFFFFF & frac64A) ? (bitsMore=1) : (bitsMore=0);
			uZ.ui += (uZ.ui&1) | bitsMore;
		}
	}
	if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
	return uZ.p;
}


posit32_t softposit_subMagsP32( uint_fast32_t uiA, uint_fast32_t uiB ) {

	uint_fast16_t regA, regB;
	uint_fast64_t frac64A=0, frac64B=0;
	uint_fast32_t fracA=0, regime, tmp;
	bool sign, regSA, regSB, ecarry=0, bitNPlusOne=0, bitsMore=0;
	int_fast8_t kA=0;
	int_fast32_t expA=0;
	int_fast16_t shiftRight;
	union ui32_p32 uZ;

	sign = signP32UI( uiA );
	if (sign)
		uiA = -uiA & 0xFFFFFFFF;
	else
		uiB = -uiB & 0xFFFFFFFF;

	if (uiA==uiB){ //essential, if not need special handling
		uZ.ui = 0;
		return uZ.p;
	}
	if ((int_fast32_t)uiA < (int_fast32_t)uiB){
		uiA ^= uiB;
		uiB ^= uiA;
		uiA ^= uiB;
		(sign) ? (sign = 0 ) : (sign=1); //A becomes B
	}
	regSA = signregP32UI( uiA );
	regSB = signregP32UI( uiB );

	tmp = (uiA<<2)&0xFFFFFFFF;
	if (regSA){
		while (tmp>>31){
			kA++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
	}
	else{
		kA=-1;
		while (!(tmp>>31)){
			kA--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;
	}

	expA = tmp>>29; //to get 2 bits
	frac64A = ((0x40000000ULL | tmp<<1) & 0x7FFFFFFFULL) <<32;
	shiftRight = kA;


	tmp = (uiB<<2) & 0xFFFFFFFF;
	if (regSB){
		while (tmp>>31){
			shiftRight--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}

	}
	else{
		shiftRight++;
		while (!(tmp>>31)){
			shiftRight++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;

	}
	frac64B = ((0x40000000ULL | tmp<<1) & 0x7FFFFFFFULL) <<32;

	//This is 4kZ + expZ; (where kZ=kA-kB and expZ=expA-expB)
	shiftRight = (shiftRight<<2) + expA - (tmp>>29);
	if (shiftRight>63){
		uZ.ui = uiA;
		if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
		return uZ.p;
	}
	else
		(frac64B >>= shiftRight);

	frac64A -= frac64B;

	while((frac64A>>59)==0){
		kA--;
		frac64A<<=4;
	}
	ecarry = (0x4000000000000000 & frac64A);//(0x4000000000000000 & frac64A)>>62;
	while (!ecarry){
		if (expA==0){
			kA--;
			expA=3;
		}
		else
			expA--;
		frac64A<<=1;
		ecarry = (0x4000000000000000 & frac64A);
	}

	if(kA<0){
		regA = -kA;
		regSA = 0;
		regime = 0x40000000>>regA;
	}
	else{
		regA = kA+1;
		regSA=1;
		regime = 0x7FFFFFFF - (0x7FFFFFFF>>regA);
	}
	if(regA>30){
		//max or min pos. exp and frac does not matter.
		(regSA) ? (uZ.ui= 0x7FFFFFFF): (uZ.ui=0x1);
	}
	else{
		//remove hidden bits
		frac64A = (frac64A & 0x3FFFFFFFFFFFFFFF) >>(regA + 2) ; // 2 bits exp

		fracA = frac64A>>32;

		if (regA<=28){
			bitNPlusOne |= (0x80000000 & frac64A) ;
			expA <<= (28-regA);
		}
		else {
			if (regA==30){
				bitNPlusOne = expA&0x2;
				bitsMore = (expA&0x1);
				expA = 0;
			}
			else if (regA==29){
				bitNPlusOne = expA&0x1;
				expA>>=1;
			}
			if (fracA>0){
				fracA=0;
				bitsMore =1;
			}

		}

		uZ.ui = packToP32UI(regime, expA, fracA);
		//n+1 frac bit is 1. Need to check if another bit is 1 too if not round to even
		if (bitNPlusOne){
			(0x7FFFFFFF & frac64A) ? (bitsMore=1) : (bitsMore=0);
			uZ.ui += (uZ.ui&1) | bitsMore;
		}
	}
	if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;
	return uZ.p;
}


posit32_t p32_add( posit32_t a, posit32_t b ){
    union ui32_p32 uA, uB, uZ;
    uint_fast32_t uiA, uiB;

    uA.p = a;
	uiA = uA.ui;
	uB.p = b;
	uiB = uB.ui;

#ifdef SOFTPOSIT_EXACT
		uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#endif

    //Zero or infinity
	if (uiA==0 || uiB==0){ // Not required but put here for speed
#ifdef SOFTPOSIT_EXACT
		uZ.ui.v = uiA | uiB;
		uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#else
		uZ.ui = uiA | uiB;
#endif
		return uZ.p;
	}
	else if ( uiA==0x80000000 || uiB==0x80000000 ){
		//printf("in infinity\n");
#ifdef SOFTPOSIT_EXACT
		uZ.ui.v = 0x80000000;
		uZ.ui.exact = 0;
#else
		uZ.ui = 0x80000000;
#endif
		return uZ.p;
	}

	//different signs
	if ((uiA^uiB)>>31)
		return softposit_subMagsP32(uiA, uiB);
	else
		return softposit_addMagsP32(uiA, uiB);

}

posit32_t p32_sub( posit32_t a, posit32_t b ) {


	union ui32_p32 uA, uB, uZ;
	uint_fast32_t uiA, uiB;

	uA.p = a;
	uiA = uA.ui;
	uB.p = b;
	uiB = uB.ui;

#ifdef SOFTPOSIT_EXACT
		uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#endif

	//infinity
	if ( uiA==0x80000000 || uiB==0x80000000 ){
#ifdef SOFTPOSIT_EXACT
		uZ.ui.v = 0x80000000;
		uZ.ui.exact = 0;
#else
		uZ.ui = 0x80000000;
#endif
		return uZ.p;
	}
	//Zero
	else if ( uiA==0 || uiB==0 ){
#ifdef SOFTPOSIT_EXACT
		uZ.ui.v = (uiA | -uiB);
		uZ.ui.exact = 0;
#else
		uZ.ui = (uiA | -uiB);
#endif
		return uZ.p;
	}

	//different signs
	if ((uiA^uiB)>>31)
			return softposit_addMagsP32(uiA, (-uiB & 0xFFFFFFFF));
	else
		return softposit_subMagsP32(uiA, (-uiB & 0xFFFFFFFF));



}

posit32_t p32_mul( posit32_t pA, posit32_t pB ){


	union ui32_p32 uA, uB, uZ;
	uint_fast32_t uiA, uiB;
	uint_fast32_t regA, fracA, regime, tmp;
	bool signA, signB, signZ, regSA, regSB, bitNPlusOne=0, bitsMore=0, rcarry;
	int_fast32_t expA;
	int_fast8_t kA=0;
	uint_fast64_t frac64Z;

	uA.p = pA;
	uiA = uA.ui;
	uB.p = pB;
	uiB = uB.ui;

#ifdef SOFTPOSIT_EXACT
		uZ.ui.exact = (uiA.ui.exact & uiB.ui.exact);
#endif
	//NaR or Zero
	if ( uiA==0x80000000 || uiB==0x80000000 ){

#ifdef SOFTPOSIT_EXACT
		uZ.ui.v = 0x80000000;
		uZ.ui.exact = 0;
#else
		uZ.ui = 0x80000000;
#endif
		return uZ.p;
	}
	else if (uiA==0 || uiB==0){
#ifdef SOFTPOSIT_EXACT

		uZ.ui.v = 0;
		if ( (uiA==0 && uiA.ui.exact) || (uiB==0 && uiB.ui.exact) )
			uZ.ui.exact = 1;
		else
			uZ.ui.exact = 0;
#else
		uZ.ui = 0;
#endif
		return uZ.p;
	}

	signA = signP32UI( uiA );
	signB = signP32UI( uiB );
	signZ = signA ^ signB;

	if(signA) uiA = (-uiA & 0xFFFFFFFF);
	if(signB) uiB = (-uiB & 0xFFFFFFFF);

	regSA = signregP32UI(uiA);
	regSB = signregP32UI(uiB);


	tmp = (uiA<<2)&0xFFFFFFFF;
	if (regSA){

		while (tmp>>31){

			kA++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
	}
	else{
		kA=-1;
		while (!(tmp>>31)){
			kA--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;
	}
	expA = tmp>>29; //to get 2 bits
	fracA = ((tmp<<1) | 0x40000000) & 0x7FFFFFFF;

	tmp = (uiB<<2)&0xFFFFFFFF;
	if (regSB){
		while (tmp>>31){
			kA++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
	}
	else{
		kA--;
		while (!(tmp>>31)){
			kA--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;
	}
	expA += tmp>>29;
	frac64Z = (uint_fast64_t) fracA * (((tmp<<1) | 0x40000000) & 0x7FFFFFFF);

	if (expA>3){
		kA++;
		expA&=0x3; // -=4
	}

	rcarry = frac64Z>>61;//3rd bit of frac64Z
	if (rcarry){
		expA++;
		if (expA>3){
			kA ++;
			expA&=0x3;
		}
		frac64Z>>=1;
	}

	if(kA<0){
		regA = -kA;
		regSA = 0;
		regime = 0x40000000>>regA;
	}
	else{
		regA = kA+1;
		regSA=1;
		regime = 0x7FFFFFFF - (0x7FFFFFFF>>regA);
	}



	if(regA>30){
		//max or min pos. exp and frac does not matter.
		(regSA) ? (uZ.ui= 0x7FFFFFFF): (uZ.ui=0x1);
	}
	else{
		//remove carry and rcarry bits and shift to correct position (2 bits exp, so + 1 than 16 bits)
		frac64Z = (frac64Z&0xFFFFFFFFFFFFFFF) >> regA;
		fracA = (uint_fast32_t) (frac64Z>>32);

		if (regA<=28){
			bitNPlusOne |= (0x80000000 & frac64Z);
			expA<<= (28-regA);
		}
		else {
			if (regA==30){
				bitNPlusOne = expA&0x2;
				bitsMore = (expA&0x1);
				expA = 0;
			}
			else if (regA==29){
				bitNPlusOne = expA&0x1;
				expA>>=1; //taken care of by the pack algo
			}
			if (fracA>0){
				fracA=0;
				bitsMore =1;
			}

		}
		//sign is always zero
		uZ.ui = packToP32UI(regime, expA, fracA);
		//n+1 frac bit is 1. Need to check if another bit is 1 too if not round to even
		if (bitNPlusOne){
			(0x7FFFFFFF & frac64Z) ? (bitsMore=1) : (bitsMore=0);
			uZ.ui += (uZ.ui&1) | bitsMore;
		}
	}

	if (signZ) uZ.ui = -uZ.ui & 0xFFFFFFFF;
	return uZ.p;

}

posit32_t p32_div( posit32_t pA, posit32_t pB )
{
    union ui32_p32 uA, uB, uZ;
    uint_fast32_t uiA, uiB, fracA, fracB, regA, regime, regB, tmp;
    bool signA, signB, signZ, regSA, regSB, bitNPlusOne=0, bitsMore=0, rcarry;
	int_fast8_t kA=0;
	int_fast32_t expA;
	uint_fast64_t frac64A, frac64Z, rem;
	lldiv_t divresult;

	uA.p = pA;
	uiA = uA.ui;
	uB.p = pB;
	uiB = uB.ui;

	//Zero or infinity
	if ( uiA==0x80000000 || uiB==0x80000000 || uiB==0){
#ifdef SOFTPOSIT_EXACT
		uZ.ui.v = 0x80000000;
		uZ.ui.exact = 0;
#else
		uZ.ui = 0x80000000;
#endif
		return uZ.p;
	}
	else if (uiA==0){
#ifdef SOFTPOSIT_EXACT

		uZ.ui.v = 0;
		if ( (uiA==0 && uiA.ui.exact) || (uiB==0 && uiB.ui.exact) )
			uZ.ui.exact = 1;
		else
			uZ.ui.exact = 0;
#else
		uZ.ui = 0;
#endif
		return uZ.p;
	}

	signA = signP32UI( uiA );
	signB = signP32UI( uiB );
	signZ = signA ^ signB;
	if(signA) uiA = (-uiA & 0xFFFFFFFF);
	if(signB) uiB = (-uiB & 0xFFFFFFFF);
	regSA = signregP32UI(uiA);
	regSB = signregP32UI(uiB);

	tmp = (uiA<<2)&0xFFFFFFFF;
	if (regSA){
		while (tmp>>31){
			kA++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
	}
	else{
		kA=-1;
		while (!(tmp>>31)){
			kA--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;
	}
	expA = tmp>>29; //to get 2 bits
	fracA = ((tmp<<1) | 0x40000000) & 0x7FFFFFFF;
	frac64A = (uint64_t) fracA << 30;

	tmp = (uiB<<2)&0xFFFFFFFF;
	if (regSB){
		while (tmp>>31){
			kA--;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
	}
	else{
		kA++;
		while (!(tmp>>31)){
			kA++;
			tmp= (tmp<<1) & 0xFFFFFFFF;
		}
		tmp&=0x7FFFFFFF;
	}
	expA -= tmp>>29;
	fracB = ((tmp<<1) | 0x40000000) & 0x7FFFFFFF;

	divresult = lldiv (frac64A,(uint_fast64_t)fracB);
	frac64Z = divresult.quot;
	rem = divresult.rem;

	if (expA<0){
		expA+=4;
		kA--;
	}
	if (frac64Z!=0){
		rcarry = frac64Z >> 30; // this is the hidden bit (14th bit) , extreme right bit is bit 0
		if (!rcarry){
			if (expA==0){
				kA--;
				expA=3;
			}
			else
				expA--;
			frac64Z<<=1;
		}
	}

	if(kA<0){
		regA = -kA;
		regSA = 0;
		regime = 0x40000000>>regA;
	}
	else{
		regA = kA+1;
		regSA=1;
		regime = 0x7FFFFFFF - (0x7FFFFFFF>>regA);
	}
	if(regA>30){
		//max or min pos. exp and frac does not matter.
		(regSA) ? (uZ.ui= 0x7FFFFFFF): (uZ.ui=0x1);
	}
	else{
		//remove carry and rcarry bits and shift to correct position
		frac64Z &= 0x3FFFFFFF;

		fracA = (uint_fast32_t)frac64Z >> (regA+2);

		if (regA<=28){
			bitNPlusOne = (frac64Z >> (regA +1)) & 0x1;
			expA<<= (28-regA);
			if (bitNPlusOne) ( ((1<<(regA+1))-1) & frac64Z ) ? (bitsMore=1) : (bitsMore=0);
		}
		else {
			if (regA==30){
				bitNPlusOne = expA&0x2;
				bitsMore = (expA&0x1);
				expA = 0;
			}
			else if (regA==29){
				bitNPlusOne = expA&0x1;
				expA>>=1; //taken care of by the pack algo
			}
			if (frac64Z>0){
				fracA=0;
				bitsMore =1;
			}

		}
		if (rem) bitsMore =1;

		uZ.ui = packToP32UI(regime, expA, fracA);
		if (bitNPlusOne) uZ.ui += (uZ.ui&1) | bitsMore;
	}

	if (signZ) uZ.ui = -uZ.ui & 0xFFFFFFFF;
	return uZ.p;
}

