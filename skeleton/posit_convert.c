#include <math.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
/* includes:
  uint32_t
  ui32_p32 
  uint_fast32_t 
  int_fast32_t 
  uint_fast16_t 
  int_fast8_t 
 */

typedef struct { uint32_t v; bool exact; } posit32_t;
union ui32_p32 { uint32_t ui; posit32_t p; };

void checkExtraP32TwoBits(double f32, double temp, bool * bitsNPlusOne, bool * bitsMore ){
	temp /= 2;
	if (temp<=f32){
		*bitsNPlusOne = 1;
		f32-=temp;
	}
	if (f32>0)
		*bitsMore = 1;
}

uint_fast32_t convertFractionP32(double f32, uint_fast16_t fracLength, bool * bitsNPlusOne, bool * bitsMore ){

	uint_fast32_t frac=0;

	if(f32==0) return 0;
	else if(f32==INFINITY) return 0x80000000;

	f32 -= 1; //remove hidden bit
	if (fracLength==0)
		checkExtraP32TwoBits(f32, 1.0, bitsNPlusOne, bitsMore);
	else{
		double temp = 1;
		while (true){
			temp /= 2;
			if (temp<=f32){
				f32-=temp;
				fracLength--;
				frac = (frac<<1) + 1; //shift in one
				if (f32==0){
					frac <<= (uint_fast16_t)fracLength;
					break;
				}

				if (fracLength == 0){
					checkExtraP32TwoBits(f32, temp, bitsNPlusOne, bitsMore);
					break;
				}
			}
			else{

				frac <<= 1; //shift in a zero
				fracLength--;
				if (fracLength == 0){
					checkExtraP32TwoBits(f32, temp, bitsNPlusOne, bitsMore);
					break;
				}
			}
		}
	}

	return frac;
}

posit32_t convertDoubleToP32(double f32){

	union ui32_p32 uZ;
	bool sign, regS;
	uint_fast32_t reg, frac=0;
	int_fast32_t exp=0;
	bool bitNPlusOne=0, bitsMore=0;

	(f32>=0) ? (sign=0) : (sign=1);

	if (f32 == 0 ){
		uZ.ui = 0;
		return uZ.p;
	}
	else if(f32 == INFINITY || f32 == -INFINITY || f32 == NAN){
		uZ.ui = 0x80000000;
		return uZ.p;
	}
	else if (f32 == 1) {
		uZ.ui = 0x40000000;
		return uZ.p;
	}
	else if (f32 == -1){
		uZ.ui = 0xC0000000;
		return uZ.p;
	}
	else if (f32 >= 1.329227995784916e+36){
		//maxpos
		uZ.ui = 0x7FFFFFFF;
		return uZ.p;
	}
	else if (f32 <= -1.329227995784916e+36){
		// -maxpos
		uZ.ui = 0x80000001;
		return uZ.p;
	}
	else if(f32 <= 7.52316384526264e-37 && !sign){
		//minpos
		uZ.ui = 0x1;
		return uZ.p;
	}
	else if(f32 >= -7.52316384526264e-37 && sign){
		//-minpos
		uZ.ui = 0xFFFFFFFF;
		return uZ.p;
	}
	else if (f32>1 || f32<-1){
		if (sign){
			//Make negative numbers positive for easier computation
			f32 = -f32;
		}

		regS = 1;
		reg = 1; //because k = m-1; so need to add back 1
		// minpos
		if (f32 <= 7.52316384526264e-37){
			uZ.ui = 1;
		}
		else{
			//regime
			while (f32>=16){
				f32 *=0.0625;  // f32/=16;
				reg++;
			}
			while (f32>=2){
				f32*=0.5;
				exp++;
			}

			int8_t fracLength = 28-reg;

			if (fracLength<0){
				//in both cases, reg=29 and 30, e is n+1 bit and frac are sticky bits
				if(reg==29){
					bitNPlusOne = exp&0x1;
					exp>>=1; //taken care of by the pack algo
				}
				else{//reg=30
					bitNPlusOne=exp>>1;
					bitsMore=exp&0x1;
					exp=0;
				}
				if (f32!=1){//because of hidden bit
					bitsMore =1;
					frac=0;
				}
			}
			else
				frac = convertFractionP32 (f32, fracLength, &bitNPlusOne, &bitsMore);


			if (reg>30 ){
				(regS) ? (uZ.ui= 0x7FFFFFFF): (uZ.ui=0x1);
			}
			//rounding off fraction bits
			else{

				uint_fast32_t regime = 1;
				if (regS) regime = ( (1<<reg)-1 ) <<1;
				if (reg<=28)  exp<<= (28-reg);
				uZ.ui = ((uint32_t) (regime) << (30-reg)) + ((uint32_t) exp ) + ((uint32_t)(frac));
				uZ.ui += (bitNPlusOne & (uZ.ui&1)) | ( bitNPlusOne & bitsMore);
			}
			if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;

		}
	}
	else if (f32 < 1 || f32 > -1 ){
		if (sign){
			//Make negative numbers positive for easier computation
			f32 = -f32;
		}
		regS = 0;
		reg = 0;

		//regime
		while (f32<1){
			f32 *= 16;
			reg++;
		}

		while (f32>=2){
			f32*=0.5;
			exp++;
		}


		//only possible combination for reg=15 to reach here is 7FFF (maxpos) and FFFF (-minpos)
		//but since it should be caught on top, so no need to handle
		int_fast8_t fracLength = 28-reg;
		if (fracLength<0){
			//in both cases, reg=29 and 30, e is n+1 bit and frac are sticky bits
			if(reg==29){
				bitNPlusOne = exp&0x1;
				exp>>=1; //taken care of by the pack algo
			}
			else{//reg=30
				bitNPlusOne=exp>>1;
				bitsMore=exp&0x1;
				exp=0;
			}
			if (f32!=1){//because of hidden bit
				bitsMore =1;
				frac=0;
			}
		}
		else
			frac = convertFractionP32 (f32, fracLength, &bitNPlusOne, &bitsMore);


		if (reg>30 ){
			(regS) ? (uZ.ui= 0x7FFFFFFF): (uZ.ui=0x1);
		}
		//rounding off fraction bits
		else{

			uint_fast32_t regime = 1;
			if (regS) regime = ( (1<<reg)-1 ) <<1;
			if (reg<=28)  exp<<= (28-reg);
			uZ.ui = ((uint32_t) (regime) << (30-reg)) + ((uint32_t) exp ) + ((uint32_t)(frac));
			uZ.ui += (bitNPlusOne & (uZ.ui&1)) | ( bitNPlusOne & bitsMore);
		}
		if (sign) uZ.ui = -uZ.ui & 0xFFFFFFFF;

	}
	else {
		//NaR - for NaN, INF and all other combinations
		uZ.ui = 0x80000000;
	}
	return uZ.p;
}

/*
int main() {
  float float_val = 1;
  posit32_t p = convertDoubleToP32((double)float_val);
  uint32_t bits = p.v;
  
  printf("\nposit representation of %f: ", float_val);
  for(int i = 31; i >= 0; i--) {
    printf("%d", !!(bits & (1 << i)));
  }
  printf("\n");
}
*/
