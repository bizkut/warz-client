#ifndef WAVE_H
#define WAVE_H

struct WaveHeader {
	// Riff chunk
	char riffId[4];  // 'RIFF'
	unsigned int len;
	char riffType[4];  // 'WAVE'

	// Format chunk
	char fmtId[4];  // 'fmt '
	unsigned int fmtLen;
	unsigned short formatTag;
	unsigned short channels;
	unsigned int samplesPerSec;
	unsigned int avgBytesPerSec;
	unsigned short blockAlign;
	unsigned short bitsPerSample;

	// Data chunk
	char dataId[4];  // 'data'
	unsigned int dataLen;
};
#endif //WAVE_H

void writeWave(const char* filename, int freq, int channels, short* buffer, int samples);

//this reads a 16 bit 1 or 2 channel wave file. returns 0 on error, 1 on success
int readWave(const char* filename, int* freq, int* channels, short** buffer, int* samples);