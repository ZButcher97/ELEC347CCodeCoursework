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
float Goertzel(float Coeff);

//global variables
float data[205];
int flag = 0;

//pin assignments
AnalogIn analog_value(A0);
DigitalOut led(LED1);

//main code run first
int main()
{
	//local variables
	int f[8] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};
	float Coeff[8];
	
	//calculate coefficents
	for (int i = 0; i< 8; i++){
		float k = round(f[i] * fbin);
		Coeff[i] = 2*cos((2*pi*k)/N);
		printf("Index: %d Coeff: %f\n\r",i, Coeff[i]);
	}
	
	//attach ticker function 
	Ticker sample;
	sample.attach(&samples, 1/Fs);
	
	//forever loop
	while(true)
	{
		//if the data isnt ready, sleep
		if(flag)
		{
			//data is ready (205 samples collected) process data
			float mags[8];
			for(int i = 0; i<8; i++){
				//calculate magnitudes
				mags[i] = Goertzel(Coeff[i]);
			}
			//reset flag
			flag = 0;
			
		}
		__WFI();
	}
}

//get samples
void samples()
{
	//temp data array
	static float tempdata[205];
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
	}
}

float Goertzel(float Coeff)
{
	for(int i = 0; i<N-1; i++)
	{
		
	}
	return 0.0;
}

