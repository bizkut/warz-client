#include "wave.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char riff[4] = { 'R', 'I', 'F', 'F' };
char wave[4] = { 'W', 'A', 'V', 'E' };
char fmt[4]  = { 'f', 'm', 't', ' ' };
char dat[4]  = { 'd', 'a', 't', 'a' };

void writeWave(const char* filename, int freq, int channels, short* buffer, int samples) {
	struct WaveHeader wh;
	int i;
	int elemWritten;
	FILE *f;

	for(i=0; i<4; i++) {
		wh.riffId[i] = riff[i];
		wh.riffType[i] = wave[i];
		wh.fmtId[i] = fmt[i];
		wh.dataId[i] = dat[i];
	}

	/* Format chunk */
	wh.fmtLen = 16;
	wh.formatTag = 1;  /* PCM */
	wh.channels = channels;  
	wh.samplesPerSec =  freq;
	wh.avgBytesPerSec = freq * channels * sizeof(short);
	wh.blockAlign = channels * sizeof(short);
	wh.bitsPerSample = sizeof(short)*8;
	wh.dataLen = samples * channels * sizeof(short);
	wh.len = 36 + wh.dataLen;

	f = fopen(filename,"wb");
	if (!f) {
		printf("error: could not write wave\n");
		return;
	}

	elemWritten = fwrite(&wh, sizeof(wh), 1, f);
	if (elemWritten) elemWritten = fwrite(buffer, wh.dataLen, 1, f);
	fclose(f);
	
	if (!elemWritten){
		printf("error: could not write wave\n");
	}
}

int readWave(const char* filename, int* freq, int* channels, short** buffer, int* samples) {
	struct WaveHeader wh;
	FILE *f;
	int i;
	int elemsRead;

	memset(&wh, 0, sizeof(wh));

	f = fopen(filename,"rb");
	if (!f) {
		printf("error: could not open wave %s\n",filename);
		return 0;
	}

	fread(&wh, sizeof(wh), 1, f);
	
	for(i=0; i<4; i++) {
		if ((wh.riffId[i] != riff[i]) ||
			(wh.riffType[i] != wave[i]) ||
			(wh.fmtId[i] != fmt[i]) ||
			(wh.dataId[i] != dat[i])){
				goto closeError;
		}
	}
	// Format chunk
	if (wh.fmtLen != 16) goto closeError;
	if (wh.formatTag != 1) goto closeError;
	*channels = wh.channels;
	if (*channels <1 || *channels >2) goto closeError;
	*freq = wh.samplesPerSec;
	if (wh.blockAlign != *channels * sizeof(short)) goto closeError;
	*samples = wh.dataLen / (*channels * sizeof(short));

	if (*samples < *freq) {
		fclose(f);
		printf("error: wave file is too short\n");
		return 0;
	}
	
	(*buffer) = (short*) malloc(wh.dataLen);
	if (!*buffer){
		printf("error: could not allocate memory for wave\n");
		return 0;
	}

	elemsRead = fread(*buffer, wh.dataLen, 1, f);
	fclose(f);
	if (elemsRead != 1){
		printf("error: reading wave file\n");
		return 0;
	}

	return 1;

closeError:
	fclose(f);
	printf("error: invalid wave file %s\n",filename);
	return 0;
}