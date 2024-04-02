#include<stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <complex.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "cqt_block.h"



struct Array{
    struct Block array[WSIZE/4]; //4096 samples
} sample;

float amplitude[WSIZE] = {0.};
float temp_real[WSIZE] = {0.};
float temp_img[WSIZE] = {0.};


uint16_t array_get(void * ref ,uint32_t pos)
{
    struct Block * ptr = ((struct Block *)ref) + (pos/4);

    switch(pos%4)
    {
    case 0:
        return (ptr->block[0] >> 4);
    case 1:
        return ((ptr->block[0] << 8)  +    (ptr->block[1] >> 8)) & 4095;
    case 2:
        return ((ptr->block[1] << 4) + (ptr->block[2] >> 12)) & 4095;
    case 3:
        return (ptr->block[2] & 4095);
    }
		return 4;
}

//value should be lass than 4096 (12 bit)
void array_set(void * ref ,uint32_t pos, uint16_t val)
{
    struct Block * ptr = ((struct Block *)ref) + (pos/4);

    switch(pos%4)
    {
    case 0:
        ptr->block[0] = (ptr->block[0] & 15) + (val << 4);
    break;
    case 1:
        ptr->block[0] = (ptr->block[0] & (~15)) + (val >>8);  
        ptr->block[1] = (ptr->block[1] & (255)) + (val <<8);
    break;
    case 2:
        ptr->block[1] = (ptr->block[1] & (~255)) + (val >> 4);
        ptr->block[2] = (ptr->block[2] & (4095)) + (val << 12);
    break;
    case 3:
        ptr->block[2] = (ptr->block[2] & (~4095)) + val;
    break;
    }




}

//reverse binary number
uint16_t reverse(uint16_t input)
{
    uint16_t var = 0;
    for(uint16_t i = 0; i < log2(WSIZE); ++i)
    {
    var = ((var<<1) + ((input>>i)&1));
    }
    return var;
}


uint16_t max_level = 0, min_level = 4096;




void sample_set(uint16_t pos, uint16_t var)
{
pos &= (WSIZE-1);
//szukam minima i maksima sygnalu aby dobrac poziom zera
//chyba zbedne, ale najpierw sygnal cykliczny (okno)
//if(var < min_level) min_level = var; 
//if(var > max_level) max_level = var;
array_set(&sample,reverse(pos),var);
}

void FFT(float * result_real, float * result_img)
{

for(uint32_t L = 1; L < WSIZE; L<<=1 ) //krok pomiedzy blokami o innym range motylka
{
    
    for(uint32_t m = 0; m < L; m +=1) //krok w ramach motylka (0,1), (0,1,2,3) itd.
    {

        float w_r = cos((float) m * 2. * M_PI /(float) WSIZE) /2.;
        float w_i = sin((float) m * 2. * M_PI / (float) WSIZE) /2.;






        for(uint32_t pos = m; pos < WSIZE; pos+= (L<<1)) 
        {
            uint32_t pos2 = pos+L;

            //printf("(%d, %d) ", pos,pos2);


            float t_r = w_r * result_real[pos2] - w_i * result_img[pos2];
            float t_i = w_r * result_img[pos2] + w_i * result_real[pos2];

            float q_r = result_real[pos]/2.;
            float q_i = result_img[pos]/2.;

            //float t_r = w_r * array_get((void*) &result_real,pos2) - w_i * array_get((void*) &result_img,pos2);
            //float t_i = w_r * array_get((void*) &result_img,pos2) + w_i * array_get((void*) &result_real,pos2);
            
            //float q_r = array_get((void*) &result_real,pos)/2.;
            //float q_i = array_get((void*) &result_img,pos)/2.;

            result_real[pos2] = q_r - t_r;
            result_img[pos2] = q_i - t_i;
            result_real[pos] = q_r + t_r;
            result_img[pos] = q_i + t_i;


        }
    }
    //printf("\n");

}

}



float w(int n, int N)
{
    //return 25./46. + (21./46.)*cos(2.*M_PI*(n+N/2)/N);
	
	 return 0.5f * (1.f - cos(2.*M_PI*(n+N/2)/N));
}



float output[CQT_RESULT_SIZE] = {0.};

//const uint16_t len_arr[24] = {1970, 1564, 1241,985,782, 621,493,391,311,247,196,156, 124, 98, 78, 62, 49, 39, 31,25,20,16,13,10};

//https://doc.ml.tu-berlin.de/bbci/material/publications/Bla_constQ.pdf
//http://academics.wellesley.edu/Physics/brown/pubs/effalgV92P2698-P2701.pdf
float * CQT()
{
const int bins = 4;//3; // bins / octave (magic number) (basis)
const float Q =  5.285213f; //3.847322f;//1./(pow(2,1./bins)-1.); // (magic number) (basis)
const float fmin = 256.;
const float fmax = 8192.;


const int K = 20;//24;//bins * ceil(log2(fmax/fmin)); // bins * lg(fmax / fmin) dla pasma 20Hz do 20kHz(albo wiecej; fmax nie do konca znane) (magic number)
const float fs = 40000.; //sampling frequency; satisfies shannon-nyquist theorem



const int fftlen = WSIZE; //pow(2 , (int) ceil(log2(Q*fs/fmin) ));

//printf("bins=%d Q=%lf K=%d fftlen=%d\n",bins,Q,K,fftlen);

{
//float result_img[WSIZE] = {0}; //buffer
for(size_t i = 0; i < WSIZE; ++i) amplitude[reverse(i)] = (float) array_get(&sample,i);
	for(size_t i = 0; i < WSIZE; ++i) temp_img[i] = 0.;
FFT(amplitude,temp_img);;
for(size_t i = 0; i< WSIZE; ++i) amplitude[i] = sqrt(amplitude[i]*amplitude[i]+ temp_img[i]*temp_img[i]);//sqrt(pow(amplitude[i],2) +pow(result_img[i],2));
}


//FILE * foutput = fopen("result.txt","a");
//fprintf(foutput,"\n");
//for(size_t i = 0; i < WSIZE; ++i) fprintf(foutput,"%.2lf ",amplitude[i]);


//float output[K];

for(size_t k = 0; k < K; ++k)
{
output[k] = 0.;




//int len = Q*fs/(fmin * (float)(1<<k));
int len= /*len_arr[k];*/ (int)  (ceil( Q * fs / (fmin*  pow( 2,(float) k / (float)bins)  )));

for(size_t n = 0; n < (WSIZE/2 - len/2) ; ++n) temp_real[ reverse(n)] = temp_img[ reverse(n)] = 0.;
for(size_t n = (WSIZE/2 + len/2); n < WSIZE ; ++n) temp_real[ reverse(n)] = temp_img[ reverse(n)] =0.;

//float w_k_cq = pow(2, (float) k / (float)bins)* 2. * M_PI * fmin;
for(size_t n = 0; n < len; ++n)
{
float t_real = 4095. * w(n ,len) * cos( 2.*M_PI * ((float)n *fs/WSIZE - (fmin*  pow( 2,(float) k / (float)bins)  ))  /(float) len);
float t_img = 4095.* w(n ,len) * -sin( 2. * M_PI * ((float)n *fs/WSIZE - (fmin*  pow( 2,(float) k / (float)bins)  )) /(float) len); //nonexistent?
	
	
temp_real[  reverse( n + (WSIZE/2 - len/2))] = t_real;
temp_img[  reverse( n + (WSIZE/2 - len/2))] = t_img;

	//temp_real[ reverse( n + (WSIZE/2 - len/2))] = 2048;
	//temp_img[ reverse( n + (WSIZE/2 - len/2))] = 2048;

//temp_real[  reverse( n + (WSIZE/2 - len/2))] = 4095. * w(n ,len) * cos( 2.*M_PI * Q   *(float)n /(float) len);
//temp_img[  reverse( n + (WSIZE/2 - len/2))] = 4095.* w(n ,len) * -sin( 2. * M_PI * Q   *(float)n /(float) len); //nonexistent?
}

//fprintf(foutput,"\n");
//
//for(size_t i = 0; i < WSIZE; ++i) fprintf(foutput,"%.2lf ",temp_real[reverse(i)]);
//fprintf(foutput,"\n");

FFT(temp_real,temp_img);
for(size_t i = 1; i< WSIZE; ++i) temp_real[i] =sqrt(temp_real[i]*temp_real[i] + temp_img[i]*temp_img[i]); //sqrt(pow(temp_real[i],2) +pow(temp_img[i],2));
//for(size_t i = 0; i < WSIZE; ++i) temp_img[i] = 0.;


//for(size_t i = 0; i < WSIZE; ++i) fprintf(foutput,"%.2lf ",temp_real[i]);








for(size_t n = 1; n < WSIZE; ++n)
{
    output[k] += (temp_real[n] * amplitude[n]);
}


//printf("%lf : %d len %d\n", output[k]/(float)WSIZE, k, len);
output[k] /= len;
}




//fclose(foutput);
return output;
}



