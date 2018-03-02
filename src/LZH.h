/*
 * LZH.h
 *
 *  Created on: 20170919
 *      Author: sfeq
 */



#ifndef LZH_H_
#define LZH_H_

namespace LZH {
#include<limits.h>
struct Node {
	size_t lastpos;
	size_t nextpos;
	bool alive;
	unsigned char chr;
	unsigned long long hasha;
};
Node* nodes;

size_t compairLength;

std::unordered_map<unsigned long long, List> mapa;

struct pii {
	size_t posa;
	size_t posb;
	size_t backDeleteLength;
};
std::vector<pii> veca;

size_t checkIfSame(size_t posa, size_t posb, size_t compairLength) {
	size_t nowposa = posa;
	size_t nowposb = posb;
	size_t i;
	for (i = 0; i < compairLength && nowposa >= 0 && nowposb >= 0;
			i++, nowposa = nodes[nowposa].lastpos, nowposb =
					nodes[nowposb].lastpos) {
		if (fileContent[nowposa] != fileContent[nowposb]) {
			return 0;
		}
	}

	nowposa = posa;
	nowposb = posb;

	for (i = 0; nowposa < inputFileSize && nowposb < inputFileSize;
			i++, nowposa = nodes[nowposa].nextpos, nowposb =
					nodes[nowposb].nextpos) {
		if (fileContent[nowposa] != fileContent[nowposb]) {
			return i;
		}
	}
	return i - 1;
}

size_t* reference;
size_t findReference(size_t x) {
	return reference[x] == x ? x : reference[x] = findReference(reference[x]);
}
void mergeReference(size_t x, size_t y) {
	if (nodes[x].chr && nodes[y].chr) {
		return;
	}

	if (nodes[x].chr) {
		nodes[y].chr = nodes[x].chr;
		return;
	}

	x = findReference(x);
	y = findReference(y);
	if (x != y)
		reference[y] = x;
}

size_t deleteSameString_DE(size_t posa, size_t posb, size_t frontDeleteLength,
		size_t backDeleteLength) {
	size_t nowposa = nodes[posa].nextpos;
	size_t nowposb = nodes[posb].nextpos;
	for (; backDeleteLength > 0;
			backDeleteLength--, nowposa = nodes[nowposa].nextpos, nowposb =
					nodes[nowposb].nextpos) {
		nodes[nowposb].alive = false;
		mergeReference(nowposa, nowposb);
	}
	size_t endb = nowposb;

	nowposa = posa;
	nowposb = posb;
	for (; frontDeleteLength > 0;
			frontDeleteLength--, nowposa = nodes[nowposa].lastpos, nowposb =
					nodes[nowposb].lastpos) {
		nodes[nowposb].alive = false;
		mergeReference(nowposa, nowposb);
	}

	nodes[nowposb].nextpos = endb;
	nodes[nodes[nowposb].nextpos].lastpos = nowposb;
	return nowposb;
}

size_t deleteSameString(size_t posb, size_t frontDeleteLength,
		size_t backDeleteLength) {
	size_t nowposb = nodes[posb].nextpos;
	for (; backDeleteLength > 0;
			backDeleteLength--, nowposb = nodes[nowposb].nextpos) {
		nodes[nowposb].alive = false;
		auto & nowList = mapa[nodes[nowposb].hasha];
		nowList.deleteValue(nowposb);
		if (nowList.empty()) {
			mapa.erase(nodes[nowposb].hasha);
		}
	}
	size_t endb = nowposb;

	nowposb = posb;
	for (; frontDeleteLength > 0;
			frontDeleteLength--, nowposb = nodes[nowposb].lastpos) {
		nodes[nowposb].alive = false;
		auto & nowList = mapa[nodes[nowposb].hasha];
		nowList.deleteValue(nowposb);
		if (nowList.empty()) {
			mapa.erase(nodes[nowposb].hasha);
		}
	}

	nodes[nowposb].nextpos = endb;
	nodes[nodes[nowposb].nextpos].lastpos = nowposb;
	return nowposb;
}

void inputFileRead(char *inputFileName) {
	inputFileSize = getFileSize(inputFileName);
	//printf("%u\n", inputFileSize);
	FILE *inputFile = fopen(inputFileName, "rb");
	fileContent = (unsigned char*) malloc(inputFileSize + 4);
	nodes = (Node*) malloc((sizeof(Node)) * (inputFileSize + 4));

	fread(fileContent, sizeof(unsigned char), inputFileSize, inputFile);
	for (size_t i = 0; i < inputFileSize; i++) {
		nodes[i].alive = true;
		nodes[i].chr = fileContent[i];
		nodes[i].lastpos = i - 1;
		nodes[i].nextpos = i + 1;
	}
	fclose(inputFile);
}

inline size_t costInt(size_t num) {
	size_t res;
	if (num < (1 << 4)) {
		res = 4;
	} else if (num < (1 << 8)) {
		res = 8;
	} else if (num < (1 << 16)) {
		res = 16;
	} else if (num <= INT_MAX ) {
		res = 32;
	} else {
		res = 64;
	}
	return res + 2;
}

bool checkIfProfit(size_t posa, size_t posb, size_t frontDeleteLength,
		size_t backDeleteLength) {
	return ((costInt(posb - posa)
			+ costInt(posb - (veca.empty() ? 0 : veca.back().posb))
			+ costInt(backDeleteLength)) >> 3)
			< (frontDeleteLength + backDeleteLength);
}

void registerSameString(size_t posa, size_t posb, size_t backDeleteLength) {
	veca.push_back( { posa, posb, backDeleteLength });
}

void writeSameStringHeaders(FILE *outputFile) {
	BIT_WRITER.writeInt(inputFileSize);
	BIT_WRITER.writeInt((size_t) veca.size());
	size_t lastposb = 0;
	for (const pii & au : veca) {
		if (au.posb < lastposb) {
			puts("Then I was wrong");
			exit(-1);
		}
		if (au.posb < au.posa) {
			puts("Then I was wrong again");
			exit(-1);
		}

		BIT_WRITER.writeInt(au.posb - au.posa);
		BIT_WRITER.writeInt(au.posb - lastposb);
		BIT_WRITER.writeInt(au.backDeleteLength);
		lastposb = au.posb;
	}
	BIT_WRITER.flush();
}

void outputFileWrite(const char *outputFileName) {
	FILE *outputFile = fopen(outputFileName, "wb");
	BIT_WRITER.init(outputFile);

	writeSameStringHeaders(outputFile);

	for (size_t posa = 0; posa < inputFileSize; posa = nodes[posa].nextpos) {
		fwrite(&nodes[posa].chr, sizeof(unsigned char), 1, outputFile);
	}
	fclose(outputFile);
}

void compressOneTurn() {
	COMPRESSING = true;
	veca.clear();
	mapa.clear();

	unsigned long long TI = 1;
	for (size_t i = 0; i < compairLength; i++) {
		TI *= 277;
	}

	inputFileRead (INPUT);
	unsigned long long hasha = 0;

	size_t lengthToHead = 0;
	size_t nowhead = 0;

	for (size_t i = 0; i < inputFileSize; i = nodes[i].nextpos) {
		//cout << "i : " << i << endl;
		hasha *= 277;
		hasha += nodes[i].chr;

		if (lengthToHead >= compairLength) {
			hasha -= nodes[nowhead].chr * TI;
			nowhead = nodes[nowhead].nextpos;
		}
		lengthToHead++;

		nodes[i].hasha = hasha;
		auto it = mapa.find(hasha);
		if (it == mapa.end()) {
			mapa[hasha].push_back(i);
		} else {
			for (ListNode* it2 = it->second.front; it2 != NULL; it2 =
					it2->next) {
				size_t pos = it2->val;
				//cout << "check : " << i << endl;
				size_t res = checkIfSame(pos, i, compairLength);
				//cout << "res : " << res << endl;
				if (res != 0) {
					if (checkIfProfit(pos, i, compairLength, res - 1)) {
						registerSameString(pos, i, res - 1);
						//cout << "delete : " << i << endl;
						i = deleteSameString(i, compairLength, res - 1);
						//cout << "res : " << i << endl;
						hasha = nodes[i].hasha;
						lengthToHead -= compairLength;
						if (lengthToHead <= compairLength) {
							lengthToHead = 0;
							nowhead = 0;
						} else {
							lengthToHead -= compairLength;
							for (size_t j = 0; j < compairLength; j++) {
								nowhead = nodes[nowhead].lastpos;
							}
						}
					} else {
						it2->val = i;
					}
					goto isSame;
				}
			}
			it->second.push_back(i);
			isSame: ;
		}
	}
	outputFileWrite (OUTPUT);

	free (fileContent);
	free(nodes);
}

void inputFileRead_DE(const char* inputFileName) {
	FILE *inputFile = fopen(inputFileName, "rb");
	BIT_READER.init(inputFile);
	inputFileSize = BIT_READER.readInt();
	//printf("%u\n", inputFileSize);

	reference = (size_t*) malloc(sizeof(size_t) * (inputFileSize + 16));
	nodes = (Node*) malloc((sizeof(Node)) * (inputFileSize + 16));

	for (size_t i = 0; i < inputFileSize; i++) {
		reference[i] = i;
		nodes[i].alive = true;
		nodes[i].chr = 0;
		nodes[i].lastpos = i - 1;
		nodes[i].nextpos = i + 1;
	}

	size_t siza = BIT_READER.readInt();
	size_t posa;
	size_t posb;
	size_t backDeleteLength;
	size_t lastposb = 0;
	while (siza--) {
		posa = BIT_READER.readInt();
		posb = lastposb + BIT_READER.readInt();
		posa = posb - posa;
		backDeleteLength = BIT_READER.readInt();
		deleteSameString_DE(posa, posb, compairLength, backDeleteLength);
		lastposb = posb;
	}

	for (size_t i = 0; i < inputFileSize; i = nodes[i].nextpos) {
		fread(&nodes[i].chr, sizeof(unsigned char), 1, inputFile);
	}
	fclose(inputFile);

	for (size_t i = 0; i < inputFileSize; i++) {
		if (nodes[i].chr) {
			continue;
		}
		findReference(i);
		nodes[i].chr = nodes[reference[i]].chr;
	}

}

void outputFileWrite_DE(const char *outputFileName) {
	FILE *outputFile = fopen(outputFileName, "wb");
	for (size_t i = 0; i < inputFileSize; i++) {
		fwrite(&nodes[i].chr, sizeof(unsigned char), 1, outputFile);
	}
	fclose(outputFile);
}

void compressOneTurn_DE() {
	COMPRESSING = false;
	veca.clear();
	mapa.clear();
	inputFileRead_DE (INPUT);
	outputFileWrite_DE (OUTPUT);
	free(reference);
	free(nodes);
}

void compress(const char* fileName) {
	INPUT_FINAL = (char *) fileName;

	size_t FILE_LENGTH_FINAL = getFileSize(INPUT_FINAL);
	compairLength = 1;
	while (compairLength <= (FILE_LENGTH_FINAL >> 1)) {
		compairLength <<= 1;
	}
	while (compairLength >= FILE_LENGTH_FINAL) {
		compairLength >>= 1;
	}

	size_t ti = 0;
	sprintf(INPUT, "%s", INPUT_FINAL);
	sprintf(OUTPUT, "%s.lzh%u", INPUT_FINAL, ti + 1);
	while (1) {
		if (compairLength < 4) {
			break;
		}

		compressOneTurn();
		compairLength >>= 1;
		ti++;

		sprintf(INPUT, "%s.lzh%u", INPUT_FINAL, ti);
		sprintf(OUTPUT, "%s.lzh%u", INPUT_FINAL, ti + 1);
	}

	sprintf(OUTPUT, "%s.lzh", INPUT_FINAL);
	FILE *outputFile = fopen(OUTPUT, "wb");
	BIT_WRITER.init(outputFile);
	BIT_WRITER.writeInt(ti);
	BIT_WRITER.flush();
	inputFileSize = getFileSize(INPUT);
	//printf("%u\n", inputFileSize);
	FILE *inputFile = fopen(INPUT, "rb");
	fileContent = (unsigned char*) malloc(inputFileSize + 16);

	fread(fileContent, sizeof(unsigned char), inputFileSize, inputFile);
	fwrite(fileContent, sizeof(unsigned char), inputFileSize, outputFile);

	fclose(inputFile);
	fclose(outputFile);
	free (fileContent);

#ifdef XENOAMESS_RELEASE
	while (ti) {
		sprintf(INPUT, "%s.lzh%u", INPUT_FINAL, ti);
		ti--;
		remove(INPUT);
	}
#endif
}
void compress_DE(char * fileName) {
	INPUT_FINAL = fileName;
	OUTPUT_FINAL = (char *) malloc(sizeof(unsigned char) * (1000));

	sprintf(OUTPUT_FINAL, "%s", INPUT_FINAL);
	size_t lena = strlen(OUTPUT_FINAL);
	OUTPUT_FINAL[lena - strlen(".lzh")] = 0;

	FILE *inputFile = fopen(INPUT_FINAL, "rb");
	BIT_READER.init(inputFile);
	size_t ti = BIT_READER.readInt();
	//printf("ti : %u\n", ti);
	inputFileSize = getFileSize(INPUT_FINAL);
	size_t cost = costInt(ti);
	inputFileSize -= (cost >> 3);
	if (cost & 255) {
		inputFileSize--;
	}
	//printf("%u\n", inputFileSize);

	sprintf(OUTPUT, "%s.lzh%u", OUTPUT_FINAL, ti);
	FILE *outputFile = fopen(OUTPUT, "wb");
	fileContent = (unsigned char*) malloc(inputFileSize + 4);

	fread(fileContent, sizeof(unsigned char), inputFileSize, inputFile);
	fwrite(fileContent, sizeof(unsigned char), inputFileSize, outputFile);

	fclose(inputFile);
	fclose(outputFile);
	free (fileContent);

	compairLength = 4;
	size_t tti = ti;
	while (ti > 0) {
		sprintf(INPUT, "%s.lzh%u", OUTPUT_FINAL, ti);
		if (ti != 1) {
			sprintf(OUTPUT, "%s.lzh%u", OUTPUT_FINAL, ti - 1);
		} else {
			sprintf(OUTPUT, "%s", OUTPUT_FINAL);
		}
		compressOneTurn_DE();
		compairLength <<= 1;
		ti--;
	}
#ifdef XENOAMESS_RELEASE
	while (tti) {
		sprintf(INPUT, "%s.lzh%u", OUTPUT_FINAL, tti);
		tti--;
		remove(INPUT);
	}
#endif
	free (OUTPUT_FINAL);
}

int main(int argc, char* argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "a") == 0) {
			compress(argv[2]);
		} else if (strcmp(argv[1], "e") == 0) {
			compress_DE(argv[2]);
		}
	} else {
		puts("help:");
		puts("a FILENAME -> compress");
		puts("e FILENAME -> extract");
	}
	//compressOneTurn();
	//compressionOneTurn_DE();
	return 0;
}

}

#endif /* LZH_H_ */
