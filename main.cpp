//inlcudes
#include "mbed.h"
#include <math.h>

//constant definitions
#define pi 3.14159265358979323846
#define Fs 8000.0
#define N 205
#define fbin N/Fs

//debug tone values
int tone1 = 770;
int tone2 = 1209;


static int test = 2;


//function definitions
void samples();
float Goertzel(float Coeff);
void DisplayChar(char data);

//global variables
float data[205];
int flag = 0;

char KeypadIndexes[3][3] = {
	{'1', '2', '3'},
	{'4', '5', '6'}, 
	{'7', '8', '9'}
};

//pin assignments
AnalogIn analog_value(A0);
DigitalOut led(LED1);
DigitalIn Button(D12);
BusOut Display(D5, D6, D7, D8, D9, D10, D11);


//define ticker 
	Ticker sample;

//main code run first
int main()
{

	
	//local variables
	int f[8] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};
	float Coeff[8];
	int Max1 = 0;
	int Max2 = 0;		
	
	//calculate coefficents
	for (int i = 0; i< 8; i++){
		float k = round(f[i] * fbin);
		Coeff[i] = 2*cos((2*pi*k)/N);
		printf("Index: %d Coeff: %f\n\r",i, Coeff[i]);
	}
	
	
	
	//forever loop
	while(true)
	{
		printf("Waiting for button press\n\r");
		
		sample.attach(&samples, 1/Fs);
		
		while(Button != 0);		
		printf("button pressed\n\r");
		
		//if the data isnt ready, sleep
		if(flag)
		{
			
			//data is ready (205 samples collected) process data
			float mags[8];
			Max1 = 0;
			Max2 = 0;
			for(int i = 0; i<8; i++){					
				//calculate magnitudes
				mags[i] = Goertzel(Coeff[i]);
				printf("Mag %d: %f\n\r",i, mags[i]);		
				//find the 2 largest values
				if(mags[i] > Max1)
				{
					Max1 = i;
				} else if (mags[i] > Max2)
				{
					Max2 = i;
				}
			}
			DisplayChar(KeypadIndexes[Max1][Max2-4]);						
			//reset flag
			flag = 0;
		}
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
			//printf("Signal sample %d: %f\n\r", i, data[i]);
			//reset index
			index = 0;
		}
		//set ready flag
		flag = 1;
		sample.detach();
	}
}

float Goertzel(float Coeff)
{
	int targetfreq = 1000;
	
	int     k,i;
    float   floatnumSamples;
    float   omega,sine,cosine,coeff,q0,q1,q2,magnitude,real,imag;

    float   scalingFactor = N / 2.0;

    floatnumSamples = (float) N;
    k = (int) (0.5 + ((floatnumSamples * targetfreq) / Fs));
    omega = (2.0 * pi * k) / floatnumSamples;
    sine = sin(omega);
    cosine = cos(omega);
    coeff = 2.0 * cosine;
    q0=0;
    q1=0;
    q2=0;

    for(i=0; i<N; i++)
    {
        q0 = coeff * q1 - q2 + data[i];
        q2 = q1;
        q1 = q0;
    }

    // calculate the real and imaginary results
    // scaling appropriately
    real = (q1 - q2 * cosine) / scalingFactor;
    imag = (q2 * sine) / scalingFactor;

    magnitude = sqrtf(real*real + imag*imag);
    return magnitude;

	/*
	float q0=0;
	float q1=0;
	float q2=0;
	
	for(int i = 0; i<N; i++)
	{
		q2 = q1;
		q1 = q0;
		q0 = Coeff * (q1 - q2) + data[i];
	}
	float result = (q1*q2) + (q2*q2) - (q1*q2*Coeff);
	//printf("Coeffs: %f\n\r", result);
	return result;
	*/
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
		
		break;
		
	}
}

