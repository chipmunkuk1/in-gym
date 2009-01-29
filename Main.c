/*
** Example Winamp .RAW input plug-in
** Copyright (c) 1998, Justin Frankel/Nullsoft Inc.
*/
//#define __cplusplus

#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <mmreg.h>
#include <msacm.h>
#include <math.h>
#include "resource.h"
//#include "unrar\unrar.c"
#include "in2.h"

#define SAMPLERATE 44100

int actualSMPRate = SAMPLERATE;
int prefSampleRate = 44100;
int prefReplayRate = 60;
BOOL disableGYM = FALSE;
BOOL disableVGM = FALSE;
BOOL showGYMXTitle = FALSE;
BOOL enableMJAZZ = FALSE;

// avoid CRT. Evil. Big. Bloated.
BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	char tmpbuf[255];
	long ret = 0;

	ret = GetPrivateProfileString("IN_GYM", "Rate", "44100", tmpbuf, 255, "IN_GYM.INI");

	if (strcmp(tmpbuf, "4000") == 0) {
		//cool
	} else if (strcmp(tmpbuf, "8000") == 0) {
		prefSampleRate = 8000;
	} else if (strcmp(tmpbuf, "11025") == 0) {
		prefSampleRate = 11025;
	} else if (strcmp(tmpbuf, "22050") == 0) {
		prefSampleRate = 22050;
	} else if (strcmp(tmpbuf, "44100") == 0) {
		prefSampleRate = 44100;
	} else if (strcmp(tmpbuf, "48000") == 0) {
		prefSampleRate = 48000;
	} else if (strcmp(tmpbuf, "96000") == 0) {
		prefSampleRate = 96000;
	}

	ret = GetPrivateProfileString("IN_GYM", "Replay", "60", tmpbuf, 255, "IN_GYM.INI");

	if (strcmp(tmpbuf, "60") == 0) {
		prefReplayRate = 60;
	} else if (strcmp(tmpbuf, "50") == 0) {
		prefReplayRate = 70;
	} else if (strcmp(tmpbuf, "70") == 0) {
		prefReplayRate = 50;
	}

	ret = GetPrivateProfileString("IN_GYM", "DisableVGM", "0", tmpbuf, 255, "IN_GYM.INI");
	if (strcmp(tmpbuf, "1") == 0) {
		disableVGM = TRUE;
	} else {
		disableVGM = FALSE;
	}

	ret = GetPrivateProfileString("IN_GYM", "ShowGYMXTitle", "0", tmpbuf, 255, "IN_GYM.INI");
	if (strcmp(tmpbuf, "1") == 0) {
		showGYMXTitle = TRUE;
	} else {
		showGYMXTitle = FALSE;
	}

	ret = GetPrivateProfileString("IN_GYM", "EnableMJAZZ", "0", tmpbuf, 255, "IN_GYM.INI");
	if (strcmp(tmpbuf, "1") == 0) {
		enableMJAZZ = TRUE;
	} else {
		enableMJAZZ = FALSE;
	}

	return TRUE;
}

// post this to the main window at end of file (after playback as stopped)
#define WM_WA_MPEG_EOF WM_USER+2

// raw configuration
#define NCH 2
//#define SAMPLERATE 44100
//#define SAMPLERATE 96000
#define BPS 16
//#if SAMPLERATE = 8000
//	#define SMP 146
//#endif
//#if SAMPLERATE = 11025
//	#define SMP 200
//#endif
//#if SAMPLERATE = 22050	
//	#define SMP 400
//#endif
//#if SAMPLERATE = 44100
//#define SMP 735
#define SMP (1600)
#define SMP2 (calcSampleSize())
//#endif
//#define SMP 800

In_Module mod; // the output module (declared near the bottom of this file)
char lastfn[MAX_PATH]; // currently playing file (used for getting info on the current file)
unsigned long file_length; // file length, in bytes
long decode_pos_ms; // current decoding position, in milliseconds
int paused; // are we paused?
long seek_needed; // if != -1, it is the point that the decode thread should seek to, in ms.
char sample_buffer[SMP*NCH*(BPS/8)*2]; // sample buffer
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

HANDLE input_file=INVALID_HANDLE_VALUE; // input file handle

int killDecodeThread=0;					// the kill switch for the decode thread
HANDLE thread_handle=INVALID_HANDLE_VALUE;	// the handle to the decode thread

DWORD WINAPI __stdcall DecodeThread(void *b); // the decode thread procedure

unsigned char * CYMData;
unsigned char * CYMGlb;

unsigned long CYMPos;

unsigned long BlankCount = 0;

unsigned long BlankPos = 0;

#include "NeonPSG.c"
//#include "script1.rc"
#include "resource.h"


int getSampleRate() {
	return prefSampleRate;
}

int getReplayRate() {
	return prefReplayRate;
}

int calcSampleSize() {

	int smp = 735;
	int nTtt = 0;

	switch (actualSMPRate) {
	case 4000:
		smp = 67;
		break;
	case 8000:
		smp = 134;
		break;
	case 11025: 
		smp = 184;
		break;
	case 22050:
		smp = 368;
		break;
	case 44100:
		smp = 735;
		break;
	case 48000:
		smp = 800;
		break;
	case 96000:
		smp = 1600;
		break;
	}
	
	nTtt = (((smp * 1000) / 60) * prefReplayRate) / 1000;

	return nTtt;
	
}

BOOL CALLBACK DialogProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
	return TRUE;
}
/*
void config(HWND hwndParent)
{

	//LRESULT res;
	MessageBox(hwndParent,
		"No configuration (yet) *.GYM Just plays the files! :)",
		"Configuration",MB_OK);
	// if we had a configuration we'd want to write it here :)
	//res = DisplayMyMessage(0,hwndParent,"Hello World!");
}
*/


BOOL CALLBACK ConfigDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	LRESULT res;
	switch (msg)
	{
	case WM_INITDIALOG:
        SendDlgItemMessage (hwnd, IDC_LSTRATE, LB_RESETCONTENT, 0, 0L);
		//SendDlgItemMessage (hwnd, IDC_CBORATE, CB_ADDSTRING, 0, (LPARAM)"4000");
		//SendDlgItemMessage (hwnd, IDC_LSTRATE, LB_ADDSTRING, 0, (LPARAM)"8000");
		SendDlgItemMessage (hwnd, IDC_LSTRATE, LB_ADDSTRING, 0, (LPARAM)"11025");
		SendDlgItemMessage (hwnd, IDC_LSTRATE, LB_ADDSTRING, 0, (LPARAM)"22050");
		SendDlgItemMessage (hwnd, IDC_LSTRATE, LB_ADDSTRING, 0, (LPARAM)"44100");
		SendDlgItemMessage (hwnd, IDC_LSTRATE, LB_ADDSTRING, 0, (LPARAM)"48000");
		SendDlgItemMessage (hwnd, IDC_LSTRATE, LB_ADDSTRING, 0, (LPARAM)"96000");

		SendDlgItemMessage(hwnd, IDC_LSTRATE, LB_SETCURSEL, 2, 0L);

		switch (prefSampleRate) {
		case 11025: 
			SendDlgItemMessage(hwnd, IDC_LSTRATE, LB_SETCURSEL, 0, 0L);
			break;
		case 22050:
			SendDlgItemMessage(hwnd, IDC_LSTRATE, LB_SETCURSEL, 1, 0L);
			break;
		case 44100:
			SendDlgItemMessage(hwnd, IDC_LSTRATE, LB_SETCURSEL, 2, 0L);
			break;
		case 48000:
			SendDlgItemMessage(hwnd, IDC_LSTRATE, LB_SETCURSEL, 3, 0L);
			break;
		case 96000:
			SendDlgItemMessage(hwnd, IDC_LSTRATE, LB_SETCURSEL, 4, 0L);
			break;
		}
	
		if (prefReplayRate == 70) {
			SendDlgItemMessage (hwnd, IDC_REPLAYRATE1, BM_SETCHECK ,1, 0L);
		} else if (prefReplayRate == 60) {
			SendDlgItemMessage (hwnd, IDC_REPLAYRATE2, BM_SETCHECK ,1, 0L);
		} else if (prefReplayRate == 50) {
			SendDlgItemMessage (hwnd, IDC_REPLAYRATE3, BM_SETCHECK ,1, 0L);
		}

		if (disableVGM == TRUE) {
			SendDlgItemMessage (hwnd, IDC_DISABLE_VGM, BM_SETCHECK ,1, 0L);
		} else {
			SendDlgItemMessage (hwnd, IDC_DISABLE_VGM, BM_SETCHECK ,0, 0L);
		}

		if (showGYMXTitle == TRUE) {
			SendDlgItemMessage (hwnd, IDC_CHKGYMX, BM_SETCHECK ,1, 0L);
		} else {
			SendDlgItemMessage (hwnd, IDC_CHKGYMX, BM_SETCHECK ,0, 0L);
		}

		if (enableMJAZZ == TRUE) {
			SendDlgItemMessage (hwnd, IDC_CHKMJAZZ, BM_SETCHECK ,1, 0L);
		} else {
			SendDlgItemMessage (hwnd, IDC_CHKMJAZZ, BM_SETCHECK ,0, 0L);
		}
		return (TRUE);
		break;

	case WM_COMMAND:
      if (HIWORD(wparam) == BN_CLICKED)
      {
	      switch (LOWORD(wparam))
	      {
	      case IDCLOSE:
		      EndDialog(hwnd, 0);
		      break;
	      case IDCANCEL:
		      EndDialog(hwnd, 0);
		      break;
		  case IDOK: 
			  {
				char smpTxt[7];
				char rateTxt[2];
				char disVGMTxt[1];
				char showGYMXTitleTxt[1];
				char enableMJAZZTxt[1];
				int nRate = 0;
				int nItem = SendDlgItemMessage(hwnd, IDC_LSTRATE, LB_GETCURSEL, 5, 0L);
				switch (nItem) {
				case 0: {
					prefSampleRate = 11025;
					strcpy(smpTxt, "11025");
					break;
						}
				case 1:
					prefSampleRate = 22050;
					strcpy(smpTxt, "22050");
					break;
				case 2:
					prefSampleRate = 44100;
					strcpy(smpTxt, "44100");
					break;
				case 3:
					prefSampleRate = 48000;
					strcpy(smpTxt, "48000");
					break;
				case 4:
					prefSampleRate = 96000;
					strcpy(smpTxt, "96000");
					break;
				default:
					prefSampleRate = 44100;
					strcpy(smpTxt, "44100");
					break;
				}


				nRate = SendDlgItemMessage(hwnd, IDC_REPLAYRATE1, BM_GETCHECK, 0, 0L);
				if (nRate == 1) {
					prefReplayRate = 70;
					strcpy(rateTxt, "50");
				} 

				nRate = SendDlgItemMessage(hwnd, IDC_REPLAYRATE2, BM_GETCHECK, 0, 0L);
				if (nRate == 1) {
					prefReplayRate = 60;
					strcpy(rateTxt, "60");
				} 

				nRate = SendDlgItemMessage(hwnd, IDC_REPLAYRATE3, BM_GETCHECK, 0, 0L);
				if (nRate == 1) {
					prefReplayRate = 50;
					strcpy(rateTxt, "70");
				} 

				if (SendDlgItemMessage(hwnd, IDC_DISABLE_VGM, BM_GETCHECK, 0, 0L) == 1) {
					strcpy(disVGMTxt, "1");
					disableVGM = TRUE;
				} else {
					strcpy(disVGMTxt, "0");
					disableVGM = FALSE;
				}

				if (SendDlgItemMessage(hwnd, IDC_CHKGYMX, BM_GETCHECK, 0, 0L) == 1) {
					strcpy(showGYMXTitleTxt, "1");
					showGYMXTitle = TRUE;
				} else {
					strcpy(showGYMXTitleTxt, "0");
					showGYMXTitle = FALSE;
				}
				
				if (SendDlgItemMessage(hwnd, IDC_CHKMJAZZ, BM_GETCHECK, 0, 0L) == 1) {
					strcpy(enableMJAZZTxt, "1");
					enableMJAZZ = TRUE;
				} else {
					strcpy(enableMJAZZTxt, "0");
					enableMJAZZ = FALSE;
				}

				//prefSampleRate = GetDlgItemInt(hwnd, IDC_LSTRATE, NULL, FALSE);
				//GetDlgItemText(hwnd, IDC_LSTRATE, smpTxt, 6);
			    res = WritePrivateProfileString("IN_GYM","Rate",smpTxt,"IN_GYM.INI");
			    res = WritePrivateProfileString("IN_GYM","Replay",rateTxt,"IN_GYM.INI");
			    res = WritePrivateProfileString("IN_GYM","DisableVGM",disVGMTxt,"IN_GYM.INI");
				res = WritePrivateProfileString("IN_GYM","ShowGYMXTitle", showGYMXTitleTxt, "IN_GYM.INI");
				res = WritePrivateProfileString("IN_GYM","EnableMJAZZ", enableMJAZZTxt, "IN_GYM.INI");
				EndDialog(hwnd, 0);
				break;
			  }
		  default:
			  return(DefWindowProc(hwnd, msg, wparam, lparam)); 
			  break;
	      }
      } else {
		  return(DefWindowProc(hwnd, msg, wparam, lparam)); 
	  } 
	  return (FALSE);
	  break;      
    default:
	  return(DefWindowProc(hwnd, msg, wparam, lparam)); 
	  break;
	}
   return (FALSE);
}

void config(HWND hwndParent)
{
	DialogBox(mod.hDllInstance, MAKEINTRESOURCE(IDD_SETTINGS), hwndParent, ConfigDlgProc);	
}


void about(HWND hwndParent)
{
	DialogBox(mod.hDllInstance, MAKEINTRESOURCE(IDD_ABOUT), hwndParent, AboutDlgProc);

}

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return (TRUE);
		break;

	case WM_COMMAND:
      if (HIWORD(wparam) == BN_CLICKED)
      {
	      switch (LOWORD(wparam))
	      {
	      case IDCLOSE:
		      EndDialog(hwnd, 0);
		      break;
	      case IDCANCEL:
		      EndDialog(hwnd, 0);
		      break;
	      case IDC_WWWZOPHAR:
		      ShellExecute(hwnd, NULL, "http://www.zophar.net", NULL, NULL, 0);
		      break;
		  case IDC_CHIPMUNK:
		      ShellExecute(hwnd, NULL, "http://www.kysoft.net", NULL, NULL, 0);
		      break;
	      }
      }
	break;      
	}
   return (FALSE);
}


void init() { /* any one-time initialization goes here (configuration reading, etc) */ }

void quit() { /* one-time deinit, such as memory freeing */ }

int isOurList2(char *fn, char *us, char *ls)
{
	char *p;
	if (fn == NULL) return 0;
	for (p = fn; *p != '\0'; p++)
	{
		if (p[0] != p[1]) continue;
		if (p[0] != ':'/* && p[0] != '\\' && p[0] != '/'*/) continue;
		if (p[2] != us[0] && p[2] != ls[0]) continue;
		if (p[3] != us[1] && p[3] != ls[1]) continue;
		return p + 4 - fn;
	}
	return 0;
}
int isOurList(char *fn, char *us, char *ls)
{
	char *p;
	if (fn == NULL) return 0;
	for (p = fn; *p != '\0'; p++)
	{
		if (p[0] != p[1]) continue;
		if (p[0] != ':'/* && p[0] != '\\' && p[0] != '/'*/) continue;
		if (p[2] != us[0] && p[2] != ls[0]) continue;
		if (p[3] != us[1] && p[3] != ls[1]) continue;
		if (p[4] != us[2] && p[4] != ls[2]) continue;
		return p + 5 - fn;
	}
	return 0;
}

int isOurLists(char *fn)
{
	int p;
	if (!disableVGM && !!(p = isOurList(fn, "VGM", "vgm"))) return p;
	if (!disableGYM && !!(p = isOurList(fn, "GYM", "gym"))) return p;
//	if (!setting.disable_gbr && !!(p = isOurList(fn, "GBR", "gbr"))) return p;
//	if (!setting.disable_gbs && !!(p = isOurList(fn, "GBS", "gbs"))) return p;
//	if (!setting.disable_hes && !!(p = isOurList(fn, "HES", "hes"))) return p;
//	if (!setting.disable_hes && !!(p = isOurList(fn, "PCE", "pce"))) return p;
//	if (!setting.disable_ay && !!(p = isOurList2(fn, "AY", "ay"))) return p;
	return 0;
}

int isourfile(char *fn) { 
	
	int isit = isOurLists(fn);
	return isit;

} 
// used for detecting URL streams.. unused here. strncmp(fn,"http://",7) to detect HTTP streams, etc





int play(char *fn) 
{ 
	int maxlatency;
	int thread_id;
	long nBytesRead = 0;
	actualSMPRate = getSampleRate();
	
	input_file = CreateFile(fn,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (input_file == INVALID_HANDLE_VALUE) // error opening file
	{
		return 1;
	}
	file_length=GetFileSize(input_file,NULL);

	CYMGlb = GlobalAlloc(GPTR, file_length);
	CYMData = GlobalLock(CYMGlb);

	ReadFile(input_file, CYMData, file_length, &nBytesRead, NULL);
	//_hread(input_file,CYMData, file_length);
	CYMPos = 0;

	strcpy(lastfn,fn);
	paused=0;
	decode_pos_ms=0;
	seek_needed=-1;

	maxlatency = mod.outMod->Open(actualSMPRate,NCH,BPS, -1,-1);
	if (maxlatency < 0) // error opening device
	{
		CloseHandle(input_file);
		input_file=INVALID_HANDLE_VALUE;
		return 1;
	}
	// dividing by 1000 for the first parameter of setinfo makes it
	// display 'H'... for hundred.. i.e. 14H Kbps.
	mod.SetInfo(0,actualSMPRate/1000,NCH,1);

	// initialize vis stuff
	mod.SAVSAInit(maxlatency,actualSMPRate);
	mod.VSASetInfo(actualSMPRate,NCH);

	mod.outMod->SetVolume(-666); // set the output plug-ins default volume

	CYMPos = InitGYM(CYMData, sample_buffer, actualSMPRate, SMP2*NCH*(BPS/8), getReplayRate(), enableMJAZZ);
	
	BlankCount = CountVBlanks(CYMData, CYMPos);
	BlankPos = 0;
	//CYMPos = 4;
	
	killDecodeThread=0;
	thread_handle = (HANDLE) CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) DecodeThread,(void *) &killDecodeThread,0,&thread_id);

	return 0; 
}

void pause() { paused=1; mod.outMod->Pause(1); }
void unpause() { paused=0; mod.outMod->Pause(0); }
int ispaused() { return paused; }

void stop() { 
	if (thread_handle != INVALID_HANDLE_VALUE)
	{
		killDecodeThread=1;
		if (WaitForSingleObject(thread_handle,INFINITE) == WAIT_TIMEOUT)
		{
			MessageBox(mod.hMainWindow,"error asking thread to die!\n","error killing decode thread",0);
			TerminateThread(thread_handle,0);
		}
		CloseHandle(thread_handle);
		thread_handle = INVALID_HANDLE_VALUE;
	}

	QuitPSG(sample_buffer);

	GlobalUnlock(CYMData);
	GlobalFree(CYMGlb);

	if (input_file != INVALID_HANDLE_VALUE)
	{
		CloseHandle(input_file);
		input_file=INVALID_HANDLE_VALUE;
	}

	mod.outMod->Close();

	mod.SAVSADeInit();
}

long getlength() { 
	return BlankCount * ((SMP2 * 1000) / actualSMPRate) ;//(file_length*10);///(SAMPLERATE/100*NCH*(BPS/8)); 
}

long getoutputtime() { 
	return decode_pos_ms+(mod.outMod->GetOutputTime()-mod.outMod->GetWrittenTime()); 
}

void setoutputtime(long time_in_ms) { 
	seek_needed=time_in_ms; 
}

void setvolume(int volume) { mod.outMod->SetVolume(volume); }
void setpan(int pan) { mod.outMod->SetPan(pan); }

int infoDlg(char *fn, HWND hwnd)
{
	// TODO: implement info dialog. 
	return 0;
}

void getfileinfo(char *filename, char *title, int *length_in_ms)
{
	char lookup[3] = "\0";
	char lookup2[3] = "\0";
	char OAYType[64] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	long tnBytesRead = 0;
	int jk = 0;
	void * tmpHandle;
	void * tmpData;
	long tmpLength = 0;
	long byteRd = 0;
	unsigned long numBl = 0;
	unsigned char strChar = ' ';
	unsigned long tmpPos = 4;
	unsigned char * tmpData1;
	int nTitleLen = 0;
	char *tit;

//	if (FileExist(filename))
//	{
//	}

	if (!filename || !*filename)  // currently playing file
	{
		//if (length_in_ms) 
			*length_in_ms = BlankCount * ((SMP2 * 1000) / actualSMPRate);
		//if (title) 
		{
			char *p=lastfn+strlen(lastfn);
			while (*p != '\\' && p >= lastfn) p--;
			strset(title, '\0');
			if (showGYMXTitle == TRUE) {
				if (strlen(gymTitle) > 0) {
					strcat(title, gymTitle);
				} else {
					strcat(title, ++p);
				}
				//strcat(title, " - ");	
			} else {
				strcat(title, ++p);
			}
		}
	}
	else // some other file
	{
		//if (length_in_ms) 
		//{
			HANDLE hFile;
			*length_in_ms=-1000;
			hFile = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,
				OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				tmpLength=GetFileSize(hFile,NULL);

				tmpHandle = GlobalAlloc(GPTR, tmpLength);
				tmpData = GlobalLock(tmpHandle);
				ReadFile(hFile, tmpData, tmpLength, &byteRd, NULL);
				tmpData1 = (unsigned char *)tmpData;
				if (showGYMXTitle == TRUE) {
					if ((tmpData1[0] == 'G') && (tmpData1[1] == 'Y') && (tmpData1[2] == 'M') && (tmpData1[3] == 'X')) {
						while ((strChar = tmpData1[tmpPos++]) != 0) {
							OAYType[nTitleLen] = strChar;
							nTitleLen++;
						}
					}
				}
				numBl = CountVBlanks2(tmpData, 428, tmpLength);
				*length_in_ms = numBl * ((SMP2 * 1000) / actualSMPRate);//(SAMPLERATE/100*NCH*(BPS/8));
				
				GlobalUnlock(tmpData);
				GlobalFree(tmpHandle);
			//	ReadFile(hFile, lookup2, 3, &tnBytesRead, NULL);
			//	for(jk=0;jk<=2;jk++)
			//		lookup[jk]=lookup2[jk];
			}
			CloseHandle(hFile);
		//}
		if (title) 
		{
			char *p=filename+strlen(filename);
			while (*p != '\\' && p >= filename) p--;
			//strset(title, '\0');
			if (showGYMXTitle == TRUE) {
				if (nTitleLen > 0) {
					strcpy(title, OAYType);
				} else {
					strcat(title, ++p);
				}
				//strcat(title, " - ");
			} else {
				strcat(title, ++p);
			}
		}
	}
}

void eq_set(int on, char data[10], int preamp) 
{ 
	// most plug-ins can't even do an EQ anyhow.. I'm working on writing
	// a generic PCM EQ, but it looks like it'll be a little too CPU 
	// consuming to be useful :)
}


// render 576 samples into buf. 
// note that if you adjust the size of sample_buffer, for say, 1024
// sample blocks, it will still work, but some of the visualization 
// might not look as good as it could. Stick with 576 sample blocks
// if you can, and have an additional auxiliary (overflow) buffer if 
// necessary.. 
long get_576_samples(char *buf, long File1Pos)
{
	signed long l = 0;
	signed long Offs = 0;
	int b = 0 ;
	char tmpBuf = '\0\0';
	int i = 0;
	signed short sLeft = 0;
	signed short sRight = 0;
	signed short * gOut = (signed short *)buf;
	int lll = (SMP2*NCH*(BPS/8)) ;

	//ReadFile(input_file,buf,576*NCH*(BPS/8),&l,NULL);

	if (CYMPos < file_length)
	{	
//		for(i = 0; i < lll; i++) {
//
			Offs=RenderGYM(CYMData, File1Pos, buf, lll, l);
//			if (i == 0) {
//				sLeft = gOut[0];
//				sRight = gOut[1];
//			}
//			gOut[i*2] = gOut[0];
//			gOut[(i*2)+1] = gOut[1];
//		}
//		gOut[0] = sLeft;
//		gOut[1] = sRight;
//
		if (Offs>=0)
		{
			CYMPos+=Offs;
			l=(SMP2*NCH*(BPS/8));
			BlankPos++;
		}
	}
	
	//PSGUpdate();
	return l;
}

DWORD WINAPI __stdcall DecodeThread(void *b)
{
	int done=0;
	while (! *((int *)b) ) 
	{
		if (seek_needed != -1)
		{
			unsigned long pp = seek_needed / ((SMP2 * 1000) / actualSMPRate);
			unsigned long pp2 = getBlankPos(pp);
			if (pp2 != 0xDEADBEEF) {
				BlankPos = pp;
				decode_pos_ms = seek_needed;//-(seek_needed%1000);
				done=0;
				mod.outMod->Flush(decode_pos_ms);
				CYMPos = pp2;
			}
			seek_needed=-1;
		}
		if (done)
		{
			mod.outMod->CanWrite();
			if (!mod.outMod->IsPlaying())
			{
				PostMessage(mod.hMainWindow,WM_WA_MPEG_EOF,0,0);
				return 0;
			}
			Sleep(10);
		}
		else if (mod.outMod->CanWrite() >= ((SMP2*NCH*(BPS/8))<<(mod.dsp_isactive()?1:0)))
		{	
			long l=SMP2*NCH*(BPS/8);
			l=get_576_samples(sample_buffer, CYMPos);
			//l=(576*NCH*(BPS/8));
			if (!l) 
			{
				done=1;
			}
			else
			{
				decode_pos_ms = BlankPos * ((SMP2 * 1000) / actualSMPRate);
				mod.SAAddPCMData((char *)sample_buffer,NCH,BPS,decode_pos_ms);
				mod.VSAAddPCMData((char *)sample_buffer,NCH,BPS,decode_pos_ms);
				//decode_pos_ms+=(SMP*1000)/SAMPLERATE;

				if (mod.dsp_isactive()) 
					l=mod.dsp_dosamples((short *)sample_buffer,l/NCH/(BPS/8),BPS,NCH,actualSMPRate)*(NCH*(BPS/8));
				mod.outMod->Write(sample_buffer,l);
			}
		}
		else Sleep(20);
	}
	return 0;
}



In_Module mod = 
{
	IN_VER,
	"GYM file Player v0.1.6.4 "
#ifdef __alpha
	"(AXP)"
#else
	"(x86)"
#endif
	,
	0,	// hMainWindow
	0,  // hDllInstance
	0, //"GYM\0Genecyst Dump Files (*.GYM)\0GMR\0Rar Compressed Genecyst Dump Files (*.GMR)\0VGM\0Video Game Music Files (*.VGM)\0"
	1,	// is_not_seekable
	1, // uses output
	config,
	about,
	init,
	quit,
	getfileinfo,
	infoDlg,
	isourfile,
	play,
	pause,
	unpause,
	ispaused,
	stop,
	
	getlength,
	getoutputtime,
	setoutputtime,

	setvolume,
	setpan,

	0,0,0,0,0,0,0,0,0, // vis stuff


	0,0, // dsp

	eq_set,

	NULL,		// setinfo

	0 // out_mod

};

char *RegistWinampFormat(char *p, char *ext, char *desc)
{
	lstrcpy(p, ext);
	p += lstrlen(p) + 1;
	lstrcpy(p, desc);
	p += lstrlen(p) + 1;
	return p;
}

char *StartWinamp(void)
{
	static char extbuf[1024];
	char *extp, *fntop;
	char tmpbuf[255];
	long ret = 0;

	ret = GetPrivateProfileString("IN_GYM", "DisableVGM", "0", tmpbuf, 255, "IN_GYM.INI");
	if (strcmp(tmpbuf, "1") == 0) {
		disableVGM = TRUE;
	} else {
		disableVGM = FALSE;
	}

	extp = extbuf;
	if (!disableVGM)
		extp = RegistWinampFormat(extp, "VGM", "VGM Sound Files (*.vgm)");
	if (!disableGYM)
		extp = RegistWinampFormat(extp, "GYM", "GYM Sound Files (*.gym)");

	*extp = '\0';
	return extbuf;
}

__declspec( dllexport ) In_Module * winampGetInModule2()
{
	mod.FileExtensions = StartWinamp();
	return &mod;
}



 