/*
	LZH algorithm
	a algorithm based on LZ77 but have somehow changed.
	for more informations,please read the code,or ask the author.

*/

#ifndef LZH_H_
#define LZH_H_

namespace LZH {

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
		fileContent = (unsigned char*)malloc(inputFileSize + 1000);
		nodes = (Node*)malloc((sizeof(Node)) * (inputFileSize + 1000));

		fread(fileContent, sizeof(unsigned char), inputFileSize, inputFile);
		//	for (size_t i = 0; i < inputFileSize; i++) {
		//		nodes[i].alive = true;
		//		nodes[i].chr = fileContent[i];
		//		nodes[i].lastpos = i - 1;
		//		nodes[i].nextpos = i + 1;
		//	}
		fclose(inputFile);
	}

	inline size_t costInt(size_t num) {
		size_t res;
		if (num < (1 << 4)) {
			res = 4;
		}
		else if (num < (1 << 8)) {
			res = 8;
		}
		else if (num < (1 << 16)) {
			res = 16;
		}
		else {
			res = 32;
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
		veca.push_back({ posa, posb, backDeleteLength });
	}

	void writeSameStringHeaders() {
		BIT_WRITER.init();
		BIT_WRITER.writeInt(inputFileSize);
		BIT_WRITER.writeInt((size_t)veca.size());
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
		BIT_WRITER.init();
		writeSameStringHeaders();

		for (size_t posa = 0; posa < inputFileSize; posa = nodes[posa].nextpos) {
			fwrite(&nodes[posa].chr, sizeof(unsigned char), 1, outputFile);
		}
		fclose(outputFile);
	}

	void compressOneTurn() {
		BIT_WRITER.init();
		fileContentIndex = 0;

		for (size_t i = 0; i < inputFileSize; i++) {
			nodes[i].alive = true;
			nodes[i].chr = fileContent[i];
			nodes[i].lastpos = i - 1;
			nodes[i].nextpos = i + 1;
		}

		COMPRESSING = true;
		veca.clear();
		mapa.clear();

		unsigned long long TI = 1;
		for (size_t i = 0; i < compairLength; i++) {
			TI *= 277;
		}

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
			}
			else {
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
							}
							else {
								lengthToHead -= compairLength;
								for (size_t j = 0; j < compairLength; j++) {
									nowhead = nodes[nowhead].lastpos;
								}
							}
						}
						else {
							it2->val = i;
						}
						goto isSame;
					}
				}
				it->second.push_back(i);
			isSame:;
			}
		}

		//
		writeSameStringHeaders();
		for (size_t posa = 0; posa < inputFileSize; posa = nodes[posa].nextpos) {
			fileContent[fileContentIndex++] = nodes[posa].chr;
		}
		//fileContent[fileContentIndex] = 0;
		inputFileSize = fileContentIndex;

		//outputFileWrite (OUTPUT);

		//free (fileContent);
		//free(nodes);
	}

	void inputFileRead_DE(const char* inputFileName) {
		BIT_READER.init();
		inputFileSize = BIT_READER.readInt();
		//printf("%u\n", inputFileSize);

		reference = (size_t*)malloc(sizeof(size_t) * (inputFileSize + 100));
		nodes = (Node*)malloc((sizeof(Node)) * (inputFileSize + 100));


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
			nodes[i].chr = fileContent[fileContentIndex++];
		}
		free(fileContent);

		for (size_t i = 0; i < inputFileSize; i++) {
			if (nodes[i].chr) {
				continue;
			}
			findReference(i);
			nodes[i].chr = nodes[reference[i]].chr;
		}

	}

	void outputFileWrite_DE(const char *outputFileName) {
		fileContent = (unsigned char*)malloc(sizeof(unsigned char) * (inputFileSize + 100));
		fileContentIndex = 0;
		for (size_t i = 0; i < inputFileSize; i++) {
			fileContent[i] = nodes[i].chr;
		}
	}

	void compressOneTurn_DE() {
		COMPRESSING = false;
		fileContentIndex = 0;
		veca.clear();
		mapa.clear();
		inputFileRead_DE(INPUT);
		outputFileWrite_DE(OUTPUT);
		free(reference);
		free(nodes);
	}

	void compress(const char* inputFileName, const char* outputFileName = NULL) {

		INPUT_FINAL = (char *)inputFileName;


		sprintf(INPUT, "%s", INPUT_FINAL);
		inputFileRead(INPUT);

		size_t FILE_LENGTH_FINAL = getFileSize(INPUT_FINAL);
		compairLength = 1;
		while (compairLength <= (FILE_LENGTH_FINAL >> 1)) {
			compairLength <<= 1;
		}
		while (compairLength >= FILE_LENGTH_FINAL) {
			compairLength >>= 1;
		}

		size_t ti = 0;

		//sprintf(OUTPUT, "%s.lzh%u", INPUT_FINAL, ti + 1);

		while (1) {
			if (compairLength < 4) {
				break;
			}
			compressOneTurn();
			compairLength >>= 1;
			ti++;

			//sprintf(INPUT, "%s.lzh%u", INPUT_FINAL, ti);
			//sprintf(OUTPUT, "%s.lzh%u", INPUT_FINAL, ti + 1);
		}

		if (outputFileName == NULL) {
			sprintf(OUTPUT, "%s.lzhra", INPUT_FINAL);
		}
		else {
			sprintf(OUTPUT, "%s", outputFileName);
		}




		FILE *outputFile = fopen(OUTPUT, "wb");
		BIT_WRITER.init(outputFile);
		BIT_WRITER.writeInt(ti);
		BIT_WRITER.flush();
		//fwrite(BIT_WRITER.writeTo, *BIT_WRITER.writeToIndex * sizeof(char), 1, outputFile);

		//printf("%u\n", inputFileSize);
	//	FILE *inputFile = fopen(INPUT, "rb");
	//	fileContent = (unsigned char*) malloc(inputFileSize + 16);

	//	fread(fileContent, sizeof(unsigned char), inputFileSize, inputFile);
		fwrite(fileContent, sizeof(unsigned char), inputFileSize, outputFile);

		//	fclose(inputFile);
		fclose(outputFile);
		free(fileContent);
	}

	void compress_DE(const char* inputFileName, const char* outputFileName = NULL) {

		if (outputFileName == NULL) {
			sprintf(OUTPUT, "%s", inputFileName);
			size_t lena = strlen(OUTPUT);
			for (int i = lena - 1; i >= 0; i--) {
				if (OUTPUT[i] == '.') {
					OUTPUT[i] = 0;
					break;
				}
			}
		}else {
			sprintf(OUTPUT, "%s", outputFileName);
		}

		
		
		FILE *inputFile = fopen(inputFileName, "rb");
		BIT_READER.init(inputFile);
		size_t ti = BIT_READER.readInt();

		//printf("ti : %u\n", ti);

		
		size_t cost = costInt(ti);
		inputFileSize = getFileSize(inputFileName);
		inputFileSize -= (cost >> 3);
		if (cost & 17) {
			inputFileSize--;
		}
		fileContent = (unsigned char*)malloc(inputFileSize + 100);
		fread(fileContent, sizeof(unsigned char), inputFileSize, inputFile);
		fclose(inputFile);

		compairLength = 4;
		size_t tti = ti;
		while (ti > 0) {
			compressOneTurn_DE();
			compairLength <<= 1;
			ti--;
		}
		
		FILE *outputFile = fopen(OUTPUT, "wb");
		fwrite(fileContent,sizeof(unsigned char), sizeof(unsigned char)*inputFileSize, outputFile);

		fclose(outputFile); 
		free(fileContent);

		
	}

	int main(int argc, char* argv[]) {
		if (argc > 1) {
			if (strcmp(argv[1], "a") == 0) {
				compress(argv[2]);
			}
			else if (strcmp(argv[1], "e") == 0) {
				compress_DE(argv[2]);
			}
		}
		else {
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
