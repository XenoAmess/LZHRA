/*
 * BWT.h
 *
 *  Created on: 20170919
 *      Author: sfeq
 */

#ifndef BWT_H_
#define BWT_H_

#include"Common.h"

namespace BWT {

size_t* startpos;

void inputFileRead(char *inputFileName) {
	inputFileSize = getFileSize(inputFileName);
//	printf("%u\n", inputFileSize);
//	std::cout << inputFileSize << std::endl;
	FILE *inputFile = fopen(inputFileName, "rb");
	fileContent = (unsigned char*) malloc(inputFileSize + 4);
	fread(fileContent, sizeof(unsigned char), inputFileSize, inputFile);
	fclose(inputFile);
	startpos = (size_t*) malloc(sizeof(size_t) * (inputFileSize + 4));
}

bool startposCmp(size_t startposa, size_t startposb) {
	for (size_t i = 0; i < inputFileSize; i++, startposa++, startposb++) {
		if (startposa >= inputFileSize)
			startposa -= inputFileSize;
		if (startposb >= inputFileSize)
			startposb -= inputFileSize;
		if (fileContent[startposa] == fileContent[startposb]) {
			continue;
		}
		return fileContent[startposa] < fileContent[startposb];
	}
	return false;
}

void startposSort() {
	for (size_t i = 0; i < inputFileSize; i++) {
		startpos[i] = i;
	}
	std::sort(startpos, startpos + inputFileSize, startposCmp);
}

void outputFileWrite(const char *outputFileName) {
	size_t startpos0 = -1;
	for (size_t i = 0; i < inputFileSize; i++) {
		if (fileContent[i] == 0) {
			startpos0 = i;
			break;
		}
	}

	FILE *outputFile = fopen(outputFileName, "wb");
	BIT_WRITER.init(outputFile);
	BIT_WRITER.writeInt(startpos0);
	BIT_WRITER.flush();
	for (size_t i = 0; i < inputFileSize; i++) {
		size_t j;
		if (startpos[i] == 0) {
			j = inputFileSize - 1;
		} else {
			j = startpos[i] - 1;
		}
		fwrite(fileContent + j, sizeof(unsigned char), 1, outputFile);
	}
	fclose(outputFile);
}

void compress(const char * fileName) {
	INPUT_FINAL = (char*) malloc(sizeof(unsigned char) * (1000));
	sprintf(INPUT_FINAL, "%s", fileName);

	OUTPUT_FINAL = (char*) malloc(sizeof(unsigned char) * (1000));
	sprintf(OUTPUT_FINAL, "%s.bwt", fileName);

	inputFileRead(INPUT_FINAL);
	startposSort();
	outputFileWrite(OUTPUT_FINAL);

	free(INPUT_FINAL);
	free(OUTPUT_FINAL);
	free(fileContent);
	free(startpos);
}

}

#endif /* BWT_H_ */
