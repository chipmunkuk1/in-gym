#ifndef SN76496_H
#define SN76496_H

#define MAX_76496 4

struct SN76496interface
{
    int num;    /* total number of 76496 in the machine */
    int baseclock;
    int volume[MAX_76496];
};

void SN76496_0_w(int offset,int data);
void SN76496_1_w(int offset,int data);
void SN76496_2_w(int offset,int data);
void SN76496_3_w(int offset,int data);

void SN76496Write(int chip,int data);
void SN76496Update_8(int chip,void *buffer,int length);
void SN76496Update_16(int chip,void *buffer,int length);
void SN76496_set_clock(int chip,int clock);
//static void SN76496_set_volume(int chip,int volume,int gain);
int SN76496_init(int chip,int clock,int sample_rate,int sample_bits);
int SN76496_sh_start();

#endif
