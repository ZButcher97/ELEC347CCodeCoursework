//inlcudes
#include "mbed.h"
#include <math.h>

//constant definitions
#define pi 3.14159265358979323846
#define Fs 8000.0
#define N 205
#define fbin N/Fs

//function definitions
void samples();
void DisplayChar(char data);
float goetzel(float Coeff);


//pin assignments
AnalogIn analog_value(A3);
DigitalOut led(LED1);

BusOut Display(D5, D6, D7, D8, D9, D10, D11);

//define ticker 
Ticker sample;
	
//global variables
float data[N];
int flag = 0;
char KeypadIndexes[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'}, 
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

//debug tone values
int tone1 = 770;
int tone2 = 1209;
static int test = 2;
DigitalIn Button(D12);


//main code
int main()
{
	//local variables
	int f[8] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};
	float Coeff[8];
	int MaxVal1 = 0;
	int MaxIndex1 = 0;
	int MaxVal2 = 0;
	int MaxIndex2 = 0;
	
	//calculate coefficents - These never change once calculated
	for (int i = 0; i< 8; i++){
		float k = round(f[i] * fbin);
		Coeff[i] = 2*cos((2*pi*k)/N);
		printf("Index: %d Coeff: %f\n\r",i, Coeff[i]);
	}
	
	//start sampling
	sample.attach(&samples, 1/Fs);
	
	//forever loop
	while(true)
	{
		//if the data isnt ready, sleep
		if(flag)
		{
			//data is ready (205 samples collected) process data
			float mags[8];
			float* test;
			MaxVal1 = 0;
			MaxVal2 = 0;
			for(int i = 0; i<8; i++){					
				//calculate magnitudes
				mags[i] =  goetzel(Coeff[i]);
				printf("mag %d: %f\n\r",i ,mags[i] );
			}
			for(int i = 0; i < 4; i++)
			{
				if(mags[i] > MaxVal1)
				{
					MaxVal1 = mags[i];
					MaxIndex1 = i;
				}
			}
			for(int i = 4; i < 8; i++)
			{
				if(mags[i] > MaxVal2)
				{
					MaxVal2 = mags[i];
					MaxIndex2 = i;
				}
			}
			printf("Max1: %d Max2: %d\n\r", MaxIndex1, MaxIndex2);
			//display the relevent character on the 7-segment display and over serial 
			printf("Row = %d  Col= %d\n\r", MaxIndex1, MaxIndex2-4);
			DisplayChar(KeypadIndexes[MaxIndex1][MaxIndex2-4]);	
			printf("Key Pressed: %c\n\r", KeypadIndexes[MaxIndex1][MaxIndex2-4]) ;
			//reset flag
			flag = 0;
			sample.attach(&samples, 1/Fs);
		}
		//sleep until the next sample
		__WFI();
	}
}

//get samples
void samples()
{
	//temp data array
	static double tempdata[205];
	
	//array index
	static int index = 0;
	
	if(index < N)
	{
		//if not all data collected, read data and store
		tempdata[index] = analog_value.read();		

		index++;
	} 
	else 
	{
		//when all data is sampled, move to global aray and set ready flag
		for(int i = 0; i<=205;i++)
		{
			data[i] = tempdata[i];
			//reset index
			index = 0;
		}
		//set ready flag
		flag = 1;
		sample.detach();
	}
}

float goetzel(float Coeff)
{
	double magnitude_re;
	double magnitude_im;
	
  double Q0_re;
  double Q0_im;
  double Q1_re;
  double Q1_im;
  double Q2_re;
  double Q2_im;
	
	double result;
  double Q1_re_tmp;
	
  Q0_re = 0.0;
  Q0_im = 0.0;
  Q1_re = 0.0;
  Q1_im = 0.0;
  Q2_re = 0.0;
  Q2_im = 0.0;
	
  for (int i = 0; i < N-1; i++) {
    Q2_re = Q1_re;
    Q2_im = Q1_im;
    Q1_re = Q0_re;
    Q1_im = Q0_im;
    Q0_re = (Coeff * Q0_re - Q2_re) + data[i];
    Q0_im = Coeff * Q0_im - Q2_im;
  }

  Q1_re_tmp = Q1_re * Q2_re - Q1_im * Q2_im;
  Q0_re = Q1_re * Q2_im + Q1_im * Q2_re;
  Q0_im = Q2_re * Q2_im;
	
  magnitude_re = (Q1_re_tmp + (Q2_re * Q2_re - Q2_im * Q2_im)) - Q1_re_tmp * Coeff;
  magnitude_im = (Q0_re + (Q0_im + Q0_im)) - Q0_re * Coeff;
	result = sqrtf((magnitude_im*magnitude_im) + (magnitude_re*magnitude_re));
	
  return result;
}

void DisplayChar(char data)
{
	int ToDisplay = 0b0000000;
	/*
	f = Pin D5
	g = Pin D6
	b = Pin D7
	a = Pin D8
	e = Pin D9
	d = Pin D10
	c = Pin D11
	*/
	switch(data){ 
		case '1':	  //cdeabgf
			Display = 0b0111011;
		break;
		
		case '2':   //cdeabgf
			Display = 0b1000001;
		break;
		
		case '3':   //cdeabgf
			Display = 0b0010001;
		break;
		
		case '4':   //cdeabgf
			Display = 0b0111000;
		break;
		
		case '5':   //cdeabgf
			Display = 0b0010100;
		break;
		
		case '6':   //cdeabgf
			Display = 0b0000100;
		break;
		
		case '7':   //cdeabgf
			Display = 0b0110011;
		break;
		
		case '8':   //cdeabgf
			Display = 0b0000000;
		break;
		
		case '9':   //cdeabgf
			Display = 0b0010000;
		break;
		
		default:
			Display = 0b1111111;
		break;
		
	}
}


