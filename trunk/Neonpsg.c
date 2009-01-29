
//////////////////////////////////////////////////////////////////////////////////////////
//
// in_GYM   (WinAMP Plugin of Win32 Port of DOS GYM Player!!)
//
// v0.1.6.4    -    Tweaked MJazz some more, sounds better I think...
// 27-Aug-2004
//
// v0.1.6.3    -    Tweaked MJazz to actually do somthing... doh! :)
// 26-Aug-2004
//
// v0.1.6.2    -    Woot, MJazz support has been added (I hope it's right, if it's not, please
// 26-Aug-2004      let me know! =)
//
// v0.1.6.1    -    Changed gymx title option slightly, when checked, will show the gymx title
// 16-Aug-2004      instead of the filename, when un-checked, will show the filename instead.
//                  if the gymx header is not present, it will just show the filename.
//
// v0.1.6.0    -    Added option to turn off gymx titles
// 14-Aug-2004
//
// v0.1.5.9    -    Added option to disable VGM support so that you can, if you want, have vgm
// 27-Dec-2003      files handled by another plugin...  This setting requires a restart
//                  of winamp when changed to become active...
//
// v0.1.5.8    -    Added some more settings to play with, you can now change the replay rate
// 26-Dec-2003      to slow down or speed up the tunes, 50/60/70Hz or 0.83/1.00/1.17x.
//
// v0.1.5.7    -    Added SMS FM playback! Woohar! (Took blimmin ages, cos MAMES YM2413 code 
// 02-Oct-2003      is b0rked! I had to take code from another vgm player to get it to work!)
//                  Also, all sample rates are now working again, for GYM and VGM musics.
//                  
// v0.1.5.6    -    Added preliminary VGM decoding.  Plays most SMS/Megadrive/Game Gear 
//                  songs ok.  You need to un-compress your .vgz files into .vgm files 
//                  for this plugin to play them...  GZip support will eventually be added
//                  when I get my head around the gz code.
//                  Renember : set the plugin for 44100hz sample rate, or the vgm files
//                  will playback totally wrong!
//
// v0.1.5.5    -    Added settings dialog so the sample rate can be changed. Change is 
//                  made when the next song starts, or if current song is stopped and 
//                  restarted.
//
// v0.1.5.4    -    Changed the default rendering rate to 96000, should make for some higher
//                  quality output.
//
// v0.1.5.3    -    Fixed seeking, added code to pull the title of the GYM file (if it has a
//                  GYMX header that is) To do: Add code so that GYMX headers can be
//                  viewed / edited / stored. (uh.... eep!  total C newbie here! :)
//                  Apart from the missing gymx editing, and compression, this plugin is
//                  as complete as it's going to get me thinks! ;)
//
// v0.1.5.2    -    Implemented vBlank counting thanks to a prod by Mangas 2000.  Now the 
//                  position bar, and song length actually reflect the length of the song
//                  and not the length of the file!  Bad news is that seeking is now disabled
//                  this will be fixed soon, but i wanted to get this out first ;)
//
// v0.1.5.1    -    Fixed a silly bug, (with version 0.1.5.0, bring up the playlist and add a few
//                  gym files, see what happens?, now load a few more, eventually, WinAMP will
//                  crash.... ;)  - ooops!
// 
// v0.1.5.0    -    Added in PCM decoding, and GYMX header support (does nothing yet...)
// 
// v0.1.0.2    -    Not updated version number yet, but i now have a gui!! :) thankz Nitro!!! :)
// 
// v0.1.0.2    -    Fixed another memory leak, sonf should not stop now if on loop mode.
// 
// v0.1.0.1    -    Fixed stupid memory leak, should fix some of those crashes, also fixed the ym 
//                  corruption bug (i.e. instruments from the last ym file, playing in the next 
//                  one, missing channels, etc)
// 
// v0.1.0.0    -    It's done!!!  A GYM plugin for winamp!!!! :) hoo F***ing ray!!!! :)
// 
//////////////////////////////////////////////////////////////////////////////////////////
//int  mega_dacen;
//int  mega_dacout;
int  throttle = 1;

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <FCNTL.H>
#include <SYS\STAT.H>
#include <malloc.h>

#include <windows.h>

typedef unsigned short	UINT16;
typedef signed short	INT16;
typedef unsigned char	UINT8;
typedef signed char		INT8;

#define SIGNED_SAMPLES

#include "sn76496.h"
#include "fm.h"
#include "ym2151.h"
//#include "ym2413.h"
#include "emu2413.h"


//#include "sound.c"   
int fps = 60;
//int sample_rate = 44100;
int buffer_size;
void* sound_buffers[2];
void* sound_buffer[2];
void* dac_buffer[2];
void* dac_buffers[2];
void* blanksBuffer = 0;
void* blanksHandle = 0;
char gymTitle[64] = "\0";

unsigned char *data;
int size;

unsigned long blankCount = 0;

//void *load_file(const char *filename, int *size);
//void play(unsigned char *data, int size);

int counter = 0;                        // needed for help :)

int vBlank = 0;
//int buffer_size = 800;
int data_sample_rate;
int nReplayRate = 60;

int IsVGMFile = 0;

BOOL useMJazz = FALSE;

OPLL *opll;

long InitGYM(unsigned char * PSG_data, unsigned char * Buffer, long SampleR, long buff_size, long replayRate, BOOL mjazz)
{
	long PSG_data_pos = 0;
	int loop = 0;
	int ret = 0;
	int iChp = 0;
	data_sample_rate = SampleR;
	nReplayRate = replayRate;
	useMJazz = mjazz;

	buffer_size = buff_size/NCH;
	
	sound_buffers[0] = GlobalAlloc(GPTR,buffer_size*2);
	sound_buffers[1] = GlobalAlloc(GPTR,buffer_size*2);

    sound_buffer[0] = GlobalLock(sound_buffers[0]);
    sound_buffer[1] = GlobalLock(sound_buffers[1]);
	
	dac_buffers[0] = GlobalAlloc(GPTR,buffer_size*2);
	dac_buffers[1] = GlobalAlloc(GPTR,buffer_size*2);

    dac_buffer[0] = GlobalLock(dac_buffers[0]);
    dac_buffer[1] = GlobalLock(dac_buffers[1]);
    
    // clear the mem!
    memset(sound_buffer[0], 0, buffer_size );
    memset(sound_buffer[1], 0, buffer_size );

    memset(dac_buffer[0], 0, buffer_size );
    memset(dac_buffer[1], 0, buffer_size );

	
	//YM2612ResetChip(1);
	
	//search or GYM header
	if ((PSG_data[0] == 'G') && (PSG_data[1] == 'Y') && (PSG_data[2] == 'M') && (PSG_data[3] == 'X'))
	{
		int i = 0;
		for (i = 0; i < 64; i++) {
			gymTitle[i] = 0;
		}
		PSG_data_pos = 4;
		while (PSG_data[PSG_data_pos] != 0) {
			gymTitle[PSG_data_pos - 4] = PSG_data[PSG_data_pos];
			PSG_data_pos++;
		}
		PSG_data_pos = 428;
		IsVGMFile = 0;
	}

	if ((PSG_data[0] == 'V') && (PSG_data[1] == 'g') && (PSG_data[2] == 'm')) {
		unsigned int relOffset = (PSG_data[0x04]) +
								 (PSG_data[0x05] << 8) + 
								 (PSG_data[0x06] << 16) + 
								 (PSG_data[0x07] << 32);

		unsigned int vgmVersion= (PSG_data[0x08]) +
								 (PSG_data[0x09] << 8) +
								 (PSG_data[0x0a] << 16) +
								 (PSG_data[0x0b] << 32);

		unsigned int psgClock =  (PSG_data[0x0c]) +
								 (PSG_data[0x0d] << 8) + 
								 (PSG_data[0x0e] << 16) +
								 (PSG_data[0x0f] << 32);

		unsigned int fmClock =   (PSG_data[0x10]) +
								 (PSG_data[0x11] << 8) +
								 (PSG_data[0x12] << 16) +
								 (PSG_data[0x13] << 32);

		unsigned int g3TagOffset=(PSG_data[0x14]) +
								 (PSG_data[0x15] << 8) +
								 (PSG_data[0x16] << 16) +
								 (PSG_data[0x17] << 32);

		unsigned int totalvBlank=(PSG_data[0x18]) +
								 (PSG_data[0x19] << 8) + 
								 (PSG_data[0x1a] << 16) + 
								 (PSG_data[0x1b] << 32);

		unsigned int relLoopOff =(PSG_data[0x1c]) +
								 (PSG_data[0x1d] << 8) + 
								 (PSG_data[0x1e] << 16) +
								 (PSG_data[0x1f] << 32);

		unsigned int sampInLoop =(PSG_data[0x20]) +
								 (PSG_data[0x21] << 8) +
								 (PSG_data[0x22] << 16) +
								 (PSG_data[0x23] << 32);

		unsigned int playRate   =(PSG_data[0x24]) +
								 (PSG_data[0x25] << 8) +
								 (PSG_data[0x26] << 16) + 
								 (PSG_data[0x27] << 32);

		IsVGMFile = 1;

		PSG_data_pos = 0x40;

		if (psgClock) {
			ret = SN76496_init(0, /*(double)*/psgClock, SampleR, 16);
			ret = SN76496_init(1, /*(double)*/psgClock, SampleR, 16);
		} else {
			ret = SN76496_init(0, /*(double)*/fmClock, SampleR, 16);
			ret = SN76496_init(1, /*(double)*/fmClock, SampleR, 16);
		}
		//ret = YM2413Init(1, fmClock , SampleR);
		if (fmClock) {
			ret = YM2612Init(3, fmClock , SampleR, NULL, NULL);
			ret = YM2151Init(1, fmClock , SampleR);
			opll = OPLL_new (fmClock, SampleR);
		} else {
			ret = YM2612Init(3, psgClock , SampleR, NULL, NULL);
			ret = YM2151Init(1, psgClock , SampleR);
			opll = OPLL_new (psgClock, SampleR);
		}

	} else {
		ret = SN76496_init(0, /*(double)*/3579580, SampleR, 16);
		ret = YM2612Init(3, (53693100 / 7), SampleR, NULL, NULL);
	}

	return PSG_data_pos;
}

void QuitPSG(char * Buffer)
{
    // shutdown eveything and exit
	
	if (blanksBuffer != 0) {
		GlobalUnlock(blanksBuffer);
		GlobalFree(blanksHandle);
		blanksBuffer = 0;
		blanksHandle = 0;
	}
	
	if (IsVGMFile == 1) {
		OPLL_delete (opll);
	}

	YM2612Shutdown();

	GlobalUnlock(sound_buffer[0]);
    GlobalUnlock(sound_buffer[1]);

	GlobalFree(sound_buffers[0]);
	GlobalFree(sound_buffers[1]);

	GlobalUnlock(dac_buffer[0]);
    GlobalUnlock(dac_buffer[1]);

	GlobalFree(dac_buffers[0]);
	GlobalFree(dac_buffers[1]);

}

void main(int argc, char *argv[])
{
}

signed long RenderGYM(unsigned char * GYM_Data, unsigned long GYM__pos,  char * OutPutBuffer, long BufferLen, long BytesWritten)
{   
	long rett = 0;
    long GYM_data_pos = 0;
    int GYM_register = 0;
    int GYM_value = 0;
	long loop = 0;
	long bfPos = 0;
	signed short * OutB;
	signed short * BF1;
	signed short * BF2;
	
	signed short * DACB1;
	signed short * DACB2;

	int dac_max = 0;
	int dax_max = data_sample_rate / 30;
	
	int UpdateNow = 0;

	int ii = 0;
	int iChp = 0;

	static nWaitCount = 0;
	static nWaitPos = 0;

	OutB = (signed short *)OutPutBuffer;
	BF1 = (signed short *)sound_buffer[0];
	BF2 = (signed short *)sound_buffer[1];
	DACB1 = (signed short *)dac_buffer[0];
	DACB2 = (signed short *)dac_buffer[1];
    
	vBlank=0;

	GYM_data_pos = GYM__pos;

	
	if (!IsVGMFile) {
		while (vBlank==0)
		{
			if (GYM_data_pos < file_length)
			{   
				
				if ((GYM_Data[GYM_data_pos] == 0x00))// || (UpdateNow == 1))
				{
					SN76496Update_16(0, sound_buffer[0], buffer_size/2);
					memcpy(sound_buffer[1], sound_buffer[0], buffer_size);
					if(dac_max) 
					{
						short *dac_buf[2];
						double update_cycle = (double)(buffer_size/2) / dac_max; /* dac port write cycle */
						double stream_cnt = 0;
						for(loop = 0; loop < dac_max; loop++) {
							int old_cnt = (int)stream_cnt;
							int step;
							/* update DAC port */
							for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
								YM2612Write(iChp, 0, 0x2a);
								YM2612Write(iChp, 1, DACB1[loop]);
							}
							if(loop == dac_max - 1)
								step = (buffer_size/2) - old_cnt;
							else {
								stream_cnt += update_cycle;
								step = (int)stream_cnt - old_cnt;
							}
							/* update stream */
							dac_buf[0] = &BF1[old_cnt];
							dac_buf[1] = &BF2[old_cnt];
							for (iChp = (useMJazz?1:1); iChp < (useMJazz?3:2); iChp++) {
								YM2612UpdateOne(iChp, (void**)dac_buf, step, useMJazz);
							}
							YM2612UpdateOne(0, (void**)dac_buf, step, useMJazz);
						}
						dac_max = 0;
					}
					else
						for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
							YM2612UpdateOne(iChp, sound_buffer, buffer_size/2, useMJazz);
						}
					//for(loop=0; loop<=(BufferLen/2); loop++)
					//{
					//	BF1[loop] += DACB1[loop];
					//}
					//osd_play_streamed_sample_16(0, (signed short *)sound_buffer[0], buffer_size * 2, sample_rate, 50,  100);
					//osd_play_streamed_sample_16(1, (signed short *)sound_buffer[1], buffer_size * 2, sample_rate, 50, -100); 
					vBlank=1;
					UpdateNow = 0;
				}
				else if (GYM_Data[GYM_data_pos] == 0x01)
				{
					GYM_data_pos++;
					if (GYM_Data[GYM_data_pos] == 0x2a)
					{
						GYM_data_pos++;
						if (dac_max < buffer_size / 2 )
						{
							DACB1[dac_max++] = GYM_Data[GYM_data_pos];
						}
						else
						{
							//UpdateNow = 1;
						}
					}
					else
					{
						for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
							YM2612Write(iChp, 0, GYM_Data[GYM_data_pos]);
						}
						GYM_data_pos++;
						for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
							YM2612Write(iChp, 1, GYM_Data[GYM_data_pos]);
						}
					}
				}
				else if (GYM_Data[GYM_data_pos] == 0x02)
				{
					GYM_data_pos++;
					for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
						YM2612Write(iChp, 2, GYM_Data[GYM_data_pos]);
					}
					GYM_data_pos++;
					for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
						YM2612Write(iChp, 3, GYM_Data[GYM_data_pos]);
					}
				}
				else if (GYM_Data[GYM_data_pos] == 0x03)
				{
					GYM_data_pos++;
					SN76496Write(0, GYM_Data[GYM_data_pos]);
				}
				GYM_data_pos++;
			}
			else
			{
				vBlank=1;
			}
		}

		rett = GYM_data_pos-GYM__pos;
			
		if (rett == 0)
			rett = -1;

	} else {
		signed short *dac_buf[2];

 		while ((vBlank==0) && (GYM_data_pos < file_length))
		{
			if (nWaitCount > 0) {
				int nLeft = (buffer_size / 2) - nWaitPos;
				if (nWaitCount <= nLeft) {
					dac_buf[0] = &BF1[nWaitPos];
					dac_buf[1] = &BF2[nWaitPos];
					
					SN76496Update_16(0, dac_buf[0], nWaitCount );
					SN76496Update_16(1, dac_buf[1], nWaitCount );
					for (iChp = (useMJazz?1:1); iChp < (useMJazz?3:2); iChp++) {
						YM2612UpdateOne(iChp, (void**)dac_buf, nWaitCount, useMJazz);
					}
					YM2612UpdateOne(0, (void**)dac_buf, nWaitCount, useMJazz);
					YM2151UpdateOne(0, (void**)dac_buf, nWaitCount );
					//YM2413UpdateOne(0, (void**)dac_buf, nWaitCount );
                    for(ii = 0; ii < nWaitCount; ii ++) {
						unsigned short sTmp = OPLL_calc (opll);
						dac_buf[0][ii] += sTmp;
						dac_buf[1][ii] += sTmp;
					}
					nWaitPos += nWaitCount;
					nWaitCount = 0;
				} else {
					dac_buf[0] = &BF1[nWaitPos];
					dac_buf[1] = &BF2[nWaitPos];
					
					SN76496Update_16(0, dac_buf[0], nLeft );
					SN76496Update_16(1, dac_buf[1], nLeft );
					for (iChp = (useMJazz?1:1); iChp < (useMJazz?3:2); iChp++) {
						YM2612UpdateOne(iChp, (void**)dac_buf, nLeft, useMJazz);
					}
					YM2612UpdateOne(0, (void**)dac_buf, nLeft, useMJazz);
					YM2151UpdateOne(0, (void**)dac_buf, nLeft );
					//YM2413UpdateOne(0, (void**)dac_buf, nLeft );
                    for(ii = 0; ii < nLeft; ii ++) {
						unsigned short sTmp = OPLL_calc (opll);
						dac_buf[0][ii] += sTmp;
						dac_buf[1][ii] += sTmp;
					}
					 
					nWaitPos += nLeft;
					nWaitCount -= nLeft;
				}
				
				if (nWaitPos >= (buffer_size /2)) {
					vBlank = 1;
					nWaitPos = 0;
				}
			}
			
			if (!vBlank) {
				if (GYM_data_pos < file_length)
				{   
					unsigned int nType = GYM_Data[GYM_data_pos++];
					switch (nType) {
						case 0x4f: {
							//Game Gear PSG Stereo, write dd to port 0x06;
							unsigned int dd = GYM_Data[GYM_data_pos++];
							SN76496Write(1, dd);
							break;
						}
						case 0x50: {
							//PSG (SN76496) write value dd
							unsigned int dd = GYM_Data[GYM_data_pos++];
							SN76496Write(0, dd);
							SN76496Write(1, dd);
							break;
						}
						case 0x51: {
							//YM2413, write value dd to register aa
							unsigned int aa = GYM_Data[GYM_data_pos++];
							unsigned int dd = GYM_Data[GYM_data_pos++];
							//YM2413Write(0,aa,dd);
							OPLL_writeReg(opll, aa, dd);
							break;
						}
						case 0x52: {
							//YM2612 port 0, write value dd to register aa
							unsigned int aa = GYM_Data[GYM_data_pos++];
							unsigned int dd = GYM_Data[GYM_data_pos++];
							//if (aa == 0x2a) {
							//	if (dac_max < buffer_size / 2 )	{
							//		DACB1[dac_max++] = GYM_Data[GYM_data_pos];
							//	}
							//} else {
								for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
									YM2612Write(iChp, 0, aa);
									YM2612Write(iChp, 1, dd);
								}
							//}
							break;
						}
						case 0x53: {
							//YM2612 port 1, write value dd to register aa
							unsigned int aa = GYM_Data[GYM_data_pos++];
							unsigned int dd = GYM_Data[GYM_data_pos++];
							for (iChp = (useMJazz?0:1); iChp < (useMJazz?3:2); iChp++) {
								YM2612Write(iChp, 2, aa);
								YM2612Write(iChp, 3, dd);
							}
							break;
						}
						case 0x54: {
							//YM2151, write value dd to register aa
							unsigned int aa = GYM_Data[GYM_data_pos++];
							unsigned int dd = GYM_Data[GYM_data_pos++];
							YM2151WriteReg(0, aa, dd);
							break;
						}
						case 0x55:
						case 0x56:
						case 0x57:
						case 0x58:
						case 0x59: {
							//reserved for other chips/future, ignore
							unsigned int aa = GYM_Data[GYM_data_pos++];
							unsigned int dd = GYM_Data[GYM_data_pos++];
							break;
						}
						case 0x61: {
							//wait n samples, n can range from 0 to 65535 (approx 1.49 seconds)
							//Longer pauses than this are represented by multiple wait commands.
							unsigned int nWaitTmp = 0;
							unsigned int nTtt = 0;
							unsigned int numWait = (GYM_Data[GYM_data_pos++]);
								         numWait +=(GYM_Data[GYM_data_pos++] << 8);
							nWaitTmp = ((numWait) / ((float)44100 / data_sample_rate));
							nTtt = (((nWaitTmp * 1000) / 60) * nReplayRate) / 1000;
							//if (nWaitMax + numWait < buffer_size) {
							//	nWaitMax += numWait;
							//} else {
							//	numWait -= buffer_size;
							//}
							//vBlank = 1;
							nWaitCount += nTtt;
							break;
						}
						case 0x62: {
							unsigned int nWaitTmp = 0;
							unsigned int nTtt = 0;
							nWaitTmp = (735 / ((float)44100 / data_sample_rate));
							nTtt = (((nWaitTmp * 1000) / 60) * nReplayRate) / 1000;
							//wait 735 samples (60th of a second), a shortcut for 0x61 0xdf 0x02
						//	vBlank = 1;
						//	nWaitMax = 0;
							nWaitCount += nTtt;
							break;
						}
						case 0x63: {
							unsigned int nWaitTmp = 0;
							unsigned int nTtt = 0;
							nWaitTmp = (882 / ((float)44100 / data_sample_rate));
							nTtt = (((nWaitTmp * 1000) / 60) * nReplayRate) / 1000;
							//wait 882 samples (50th of a second), a shortcut for 0x61 0x72 0x03
							nWaitCount += nTtt;//buffer_size;
							break;
						}
						case 0x66: {
							//end of sound data
							vBlank = 1;
							break;
						}

					}
				}
			}
		}

		rett = GYM_data_pos-GYM__pos;

	}

	for(loop=0; loop<=(BufferLen+1)/2; loop+=2)
	{
		OutB[loop] = BF1[loop/2] ;//+ (DACB1[loop/2]*32);
		OutB[loop+1] = BF2[loop/2] ;// + (DACB1[loop/2]*32);

	}
	memset(dac_buffer[0], 0, buffer_size );
	memset(dac_buffer[1], 0, buffer_size );
	return rett;
}


//void updateBufs(signed short * sound_buffer1, signed short * sound_buffer2, int buffer_size) {
//	SN76496Update_16(0, sound_buffer[0], buffer_size/2);
//	SN76496Update_16(1, sound_buffer[1], buffer_size/2);
//	//memcpy(sound_buffer[1], sound_buffer[0], buffer_size);
//	if(dac_max) 
//	{
//		short *dac_buf[2];
//		double update_cycle = (double)(buffer_size/2) / dac_max; /* dac port write cycle */
//		double stream_cnt = 0;
//		for(loop = 0; loop < dac_max; loop++) {
//			int old_cnt = (int)stream_cnt;
//			int step;
//			/* update DAC port */
//			YM2612Write(0, 0, 0x2a);
///			YM2612Write(0, 1, DACB1[loop]);
//			if(loop == dac_max - 1)
//				step = (buffer_size/2) - old_cnt;
//			else {
//				stream_cnt += update_cycle;
//				step = (int)stream_cnt - old_cnt;
///			}
//			/* update stream */
//			dac_buf[0] = &BF1[old_cnt];
//			dac_buf[1] = &BF2[old_cnt];
//			YM2612UpdateOne(0, (void**)dac_buf, step);
//		}
//		dac_max = 0;
//	}
//	else
//		YM2612UpdateOne(0, sound_buffer, buffer_size/2);
//}

unsigned long CountVBlanks(unsigned char * GYM_Data, unsigned long GYM__pos)
{   
	long rett = 0;
    long GYM_data_pos = 0;
    int GYM_register = 0;
    int GYM_value = 0;
	long loop = 0;
	long bfPos = 0;
	
	int UpdateNow = 0;

	unsigned long numBlanks = 0;
	unsigned long blankPos = 0;
	unsigned long * blankPosition;

	int tmpIsVGMFile = 0;

	if ((GYM_Data[0] == 'V') && (GYM_Data[1] == 'g') && (GYM_Data[2] == 'm')) {
		tmpIsVGMFile = 1;
	}

	vBlank=0;

	GYM_data_pos = GYM__pos;
	
	if (!tmpIsVGMFile) {
		while (GYM_data_pos < file_length)
		{
				
			if ((GYM_Data[GYM_data_pos] == 0x00))// || (UpdateNow == 1))
			{
				numBlanks++;
			}
			else if (GYM_Data[GYM_data_pos] == 0x01)
			{
				GYM_data_pos++;
				if (GYM_Data[GYM_data_pos] == 0x2a)
				{
					GYM_data_pos++;
				}
				else
				{
					GYM_data_pos++;
				}
			}
			else if (GYM_Data[GYM_data_pos] == 0x02)
			{
				GYM_data_pos++;
				GYM_data_pos++;
			}
			else if (GYM_Data[GYM_data_pos] == 0x03)
			{
				GYM_data_pos++;
			}
			GYM_data_pos++;
		}
	} else {
		while (GYM_data_pos < file_length) {
			unsigned int nType = GYM_Data[GYM_data_pos++];
			switch (nType) {
				case 0x4f: {
					//Game Gear PSG Stereo, write dd to port 0x06;
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x50: {
					//PSG (SN76496) write value dd
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x51: {
					//YM2413, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x52: {
					//YM2612 port 0, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x53: {
					//YM2612 port 1, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x54: {
					//YM2151, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x55:
				case 0x56:
				case 0x57:
				case 0x58:
				case 0x59: {
					//reserved for other chips/future, ignore
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x61: {
					//wait n samples, n can range from 0 to 65535 (approx 1.49 seconds)
					//Longer pauses than this are represented by multiple wait commands.
					unsigned int numWait = (GYM_Data[GYM_data_pos++] << 8) +
										   (GYM_Data[GYM_data_pos++]);
					numBlanks++;
					break;
				}
				case 0x62: {
					//wait 735 samples (60th of a second), a shortcut for 0x61 0xdf 0x02
					numBlanks++;
					break;
				}
				case 0x63: {
					//wait 882 samples (50th of a second), a shortcut for 0x61 0x72 0x03
					numBlanks++;
					break;
				}
				case 0x66: {
					//end of sound data
					numBlanks++;
					break;
				}
			}
		}
	}

	GYM_data_pos = GYM__pos;
	
	if (blanksBuffer != 0) {
		GlobalUnlock(blanksBuffer);
		GlobalFree(blanksHandle);
		blanksBuffer = 0;
		blanksHandle = 0;
	}

	blanksHandle = GlobalAlloc(GPTR, sizeof(long) * (numBlanks + 1));
	blanksBuffer = GlobalLock(blanksHandle);

	blankPosition = (unsigned long *) blanksBuffer;
	
	if (!tmpIsVGMFile) {
		while (GYM_data_pos < file_length)
		{
				
			if ((GYM_Data[GYM_data_pos] == 0x00))// || (UpdateNow == 1))
			{
				blankPosition[blankPos] = GYM_data_pos + 1;
				blankPos++;
			}
			else if (GYM_Data[GYM_data_pos] == 0x01)
			{
				GYM_data_pos++;
				if (GYM_Data[GYM_data_pos] == 0x2a)
				{
					GYM_data_pos++;
				}
				else
				{
					GYM_data_pos++;
				}
			}
			else if (GYM_Data[GYM_data_pos] == 0x02)
			{
				GYM_data_pos++;
				GYM_data_pos++;
			}
			else if (GYM_Data[GYM_data_pos] == 0x03)
			{
				GYM_data_pos++;
			}
			GYM_data_pos++;
		}
	} else {
		while (GYM_data_pos < file_length) {
			unsigned int nType = GYM_Data[GYM_data_pos++];
			switch (nType) {
				case 0x4f: {
					//Game Gear PSG Stereo, write dd to port 0x06;
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x50: {
					//PSG (SN76496) write value dd
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x51: {
					//YM2413, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x52: {
					//YM2612 port 0, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x53: {
					//YM2612 port 1, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x54: {
					//YM2151, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x55:
				case 0x56:
				case 0x57:
				case 0x58:
				case 0x59: {
					//reserved for other chips/future, ignore
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x61: {
					//wait n samples, n can range from 0 to 65535 (approx 1.49 seconds)
					//Longer pauses than this are represented by multiple wait commands.
					unsigned int numWait = (GYM_Data[GYM_data_pos++] << 8) +
										   (GYM_Data[GYM_data_pos++]);
					blankPosition[blankPos] = GYM_data_pos + 1;
					blankPos++;
					break;
				}
				case 0x62: {
					//wait 735 samples (60th of a second), a shortcut for 0x61 0xdf 0x02
					blankPosition[blankPos] = GYM_data_pos + 1;
					blankPos++;
					break;
				}
				case 0x63: {
					//wait 882 samples (50th of a second), a shortcut for 0x61 0x72 0x03
					blankPosition[blankPos] = GYM_data_pos + 1;
					blankPos++;
					break;
				}
				case 0x66: {
					//end of sound data
					blankPosition[blankPos] = GYM_data_pos + 1;
					blankPos++;
					break;
				}
			}
		}
	}

	rett = numBlanks;
	blankCount = numBlanks;
		
	return rett;
}

unsigned long getBlankPos(unsigned long pos) {
	unsigned long * blankPosition;
	if ((pos < blankCount) && (blankCount > 0)) {
		blankPosition = (unsigned long *) blanksBuffer;
		return blankPosition[pos];
	} else {
		return 0xDEADBEEF;
	}

}

unsigned long CountVBlanks2(unsigned char * GYM_Data, unsigned long GYM__pos, unsigned long tmpLength)
{   
    long GYM_data_pos = 0;
	unsigned long numBlanks = 0;
	int tmpIsVGMFile = 0;

	if ((GYM_Data[0] == 'V') && (GYM_Data[1] == 'g') && (GYM_Data[2] == 'm')) {
		tmpIsVGMFile = 1;
	}

	GYM_data_pos = GYM__pos;
	
	if (!tmpIsVGMFile) {
		while (GYM_data_pos < tmpLength)
		{
				
			if ((GYM_Data[GYM_data_pos] == 0x00))// || (UpdateNow == 1))
			{
				numBlanks++;
			}
			else if (GYM_Data[GYM_data_pos] == 0x01)
			{
				GYM_data_pos++;
				if (GYM_Data[GYM_data_pos] == 0x2a)
				{
					GYM_data_pos++;
				}
				else
				{
					GYM_data_pos++;
				}
			}
			else if (GYM_Data[GYM_data_pos] == 0x02)
			{
				GYM_data_pos++;
				GYM_data_pos++;
			}
			else if (GYM_Data[GYM_data_pos] == 0x03)
			{
				GYM_data_pos++;
			}
			GYM_data_pos++;
		}
	} else {
		while (GYM_data_pos < tmpLength) {
			unsigned int nType = GYM_Data[GYM_data_pos++];
			switch (nType) {
				case 0x4f: {
					//Game Gear PSG Stereo, write dd to port 0x06;
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x50: {
					//PSG (SN76496) write value dd
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x51: {
					//YM2413, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x52: {
					//YM2612 port 0, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x53: {
					//YM2612 port 1, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x54: {
					//YM2151, write value dd to register aa
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x55:
				case 0x56:
				case 0x57:
				case 0x58:
				case 0x59: {
					//reserved for other chips/future, ignore
					unsigned int aa = GYM_Data[GYM_data_pos++];
					unsigned int dd = GYM_Data[GYM_data_pos++];
					break;
				}
				case 0x61: {
					//wait n samples, n can range from 0 to 65535 (approx 1.49 seconds)
					//Longer pauses than this are represented by multiple wait commands.
					unsigned int numWait = (GYM_Data[GYM_data_pos++] << 8) +
										   (GYM_Data[GYM_data_pos++]);
					numBlanks++;
					break;
				}
				case 0x62: {
					//wait 735 samples (60th of a second), a shortcut for 0x61 0xdf 0x02
					numBlanks++;
					break;
				}
				case 0x63: {
					//wait 882 samples (50th of a second), a shortcut for 0x61 0x72 0x03
					numBlanks++;
					break;
				}
				case 0x66: {
					//end of sound data
					numBlanks++;
					break;
				}
			}
		}
	}

	return numBlanks;
}