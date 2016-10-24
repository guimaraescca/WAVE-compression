#ifndef FILERESOURCES_H
#define FILERESOURCES_H

/* Struct that defines the header data of a WAVE file */
typedef struct waveHeader {
    char chunkID[4];
    int chunckSize;
    char format[4];

    char subChunk1ID[4];
    int subChunk1Size;
    short int audioFormat;
    short int numChannels;
    int sampleRate;
    int byteRate;
    short int blockAlign;
    short int bitsPerSample;

    char subChunck2ID[4];
    int subChunk2Size;
} waveHeader;

/* Struct that defines the header containing info about the compression methods used */
typedef struct compressionHeader {
    int originalSize;       // Size of the audio data inside de compressed file
    int options;            // Compressions methods used, stored as bits
}compressionHeader;

off_t getFileSize( char* );

int recursiveCharToIntSample( char*, int, int  );

#endif
