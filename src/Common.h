/*
 * Common.h
 *
 *  Created on: 20170919
 *      Author: sfeq
 */

#ifndef COMMON_H_
#define COMMON_H_
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<io.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<map>
#include<unordered_map>
#include<vector>
#include<algorithm>
#include<string>
#include"List.h"
//#include"Map.h"

//#include <sys/stat.h>
//size_t getFileSize(const char *path) {
//	size_t filesize = -1;
//	struct stat statbuff;
//	if (stat(path, &statbuff) < 0) {
//		return filesize;
//	} else {
//		filesize = statbuff.st_size;
//	}
//	return filesize;
//}

#define XENOAMESS_RELEASE

bool COMPRESSING;

char INPUT[2000];
char OUTPUT[2000];
char* INPUT_FINAL;
char* OUTPUT_FINAL;

size_t inputFileSize;
size_t getFileSize(char *filename) {
	FILE *fp = fopen(filename, "rb");
	size_t res = _filelength(_fileno(fp));
	fclose(fp);
	return res;
}

unsigned char *fileContent;
size_t fileContentIndex;

struct BitWriter {
	
	unsigned char buffer;
	unsigned char nowlen;
	void init() {
		//this->outputFile = outputFile;
		nowlen = 0;
		buffer = 0;
	}

	void writeBit(unsigned char c) {
		if (nowlen == 8) {
			flush();
		}
		buffer <<= 1;
		buffer |= c;
		nowlen++;
	}

	void writeInt(size_t num) {
		int len;
		if (num < (1 << 4)) {
			writeBit(0);
			writeBit(0);
			len = 4;
		} else if (num < (1 << 8)) {
			writeBit(0);
			writeBit(1);
			len = 8;
		} else if (num < (1 << 16)) {
			writeBit(1);
			writeBit(0);
			len = 16;
		} else {
			writeBit(1);
			writeBit(1);
			len = 32;
		}
		while (len--) {
			writeBit((unsigned char) (num & 1));
			num >>= 1;
		}
	}

	void flush() {
		buffer <<= (8 - nowlen);
		fileContent[fileContentIndex++] = buffer;
		//fwrite(&buffer, sizeof(unsigned char), 1, outputFile);
		nowlen = 0;
	}

} BIT_WRITER;

struct BitReader {
	FILE *inputFile;
	unsigned char buffer;
	unsigned char nowlen;
	void init(FILE *inputFile) {
		this->inputFile = inputFile;
		nowlen = 0;
		buffer = 0;
	}

	unsigned char readBit() {
		if (nowlen == 0) {
			fetch();
		}
		unsigned char res = (buffer & 128u) ? 1 : 0;
		buffer <<= 1;
		nowlen--;
		return res;
	}

	size_t readInt() {
		unsigned char l = readBit();
		l <<= 1;
		l |= readBit();

		size_t num = 0;

		size_t len;
		if (l == 0) {
			len = 4;
		} else if (l == 1) {
			len = 8;
		} else if (l == 2) {
			len = 16;
		} else {
			len = 32;
		}

		size_t ti = 1;
		while (len--) {
			l = readBit();
			if (l) {
				num |= ti;
			}
			ti <<= 1;
		}

		//		while (len--) {
		//			num |= readBit();
		//			num <<= 1;
		//		}
		return num;
	}

	void fetch() {
		fread(&buffer, sizeof(unsigned char), 1, inputFile);
		nowlen = 8;
	}

} BIT_READER;

#include"LZH.h"
#include"BWT.h"
#endif /* COMMON_H_ */
