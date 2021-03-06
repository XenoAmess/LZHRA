#ifndef RE_H_
#define RE_H_

namespace RE {

	struct BIT {
		unsigned long long sum;
		unsigned long long a[260];
		inline int lowbit(int &x) {
			return x & (-x);
		}
		void init() {
			memset(a, 0, sizeof(a));
			sum = 0;
		}
		void inc(int x) {
			for (int i = x; i < 260; i += lowbit(i)) {
				++a[i];
			}
			++sum;
		}
		void dec(int x) {
			for (int i = x; i < 260; i += lowbit(i)) {
				--a[i];
			}
			--sum;
		}
		unsigned long long getsum(int x) {
			if (x == 0)return 0;
			unsigned long long res = 0;
			for (int i = x; i; i -= lowbit(i))
				res += a[i];
			return res;
		}
	};

	void compress(const char* inputFileName, const char* outputFileName = NULL) {

		if (outputFileName == NULL) {
			sprintf(OUTPUT, "%s.lzhra", INPUT_FINAL);
		}else {
			sprintf(OUTPUT, "%s", outputFileName);
		}

		FILE *inputFile = fopen(inputFileName, "rb");
		FILE *outputFile = fopen(OUTPUT, "wb");



		inputFileSize = getFileSize(inputFileName);
		BIT_WRITER.init(outputFile);
		BIT_WRITER.writeInt(inputFileSize);

		BIT bit;
		bit.init();
		for (int i = 0; i < 256; i++) {
			bit.inc(i + 1);
		}

		//for (int i = 0; i < 256; i++) {
		//	printf("%d\n",bit.getsum(i+1));
		//}

		unsigned long long high = 255;
		unsigned long long low = 0;

		unsigned long long tmphigh;
		unsigned long long tmplow;

		unsigned char chr;
		std::deque<unsigned char> chars;
		chars.clear();

		for (size_t i = 0; i < inputFileSize; i++) {
			while (high - low < bit.sum * 2) {
				if (low & (1ull << 45)) {
					BIT_WRITER.writeBit(1);
					//printf("1");
					high -= (1ull << 45);
					low -= (1ull << 45);
				}
				else {
					BIT_WRITER.writeBit(0);
					//printf("0");
				}

				high <<= 1;
				//high |= 1;
				low <<= 1;
			}

			fread(&chr, sizeof(chr), 1, inputFile);

			unsigned long long lows = bit.getsum(chr);
			unsigned long long highs = bit.getsum(chr + 1);
			tmplow = low + (high - low) * lows / bit.sum;

			if (low < (1ull << 45) && tmplow>= (1ull << 45)) {
				puts("fatal error");
				system("pause");
			}

			tmphigh = low + (high - low) * highs / bit.sum;
			low = tmplow;
			high = tmphigh;

			//printf("%d\n",low);
			//printf("%d\n", high);

			if (high == low) {
				puts("error");
			}
			/*
			if (low >= (1ull<<45+1)) {
				int i = 63;
				for (; ;i--) {
					if (low & (1ull << i)) {
						break;
					}
				}
				for (; i >= 45+1; i--) {
					if (low & (1ull << i)) {
						BIT_WRITER.writeBit(1);
						//printf("1");
						high -= (1ull << i);
						low -= (1ull << i);
					}
					else {
						BIT_WRITER.writeBit(0);
					}
				}
			}
			*/

			
			//puts("");

			bit.inc(chr + 1);
			chars.push_back(chr);
			if (chars.size() == 2048) {
				bit.dec(chars.front() + 1);
				chars.pop_front();
			}
		}

		fclose(inputFile);

		int enda = 0;
		for (enda = 0; enda < 45; enda++) {
			if ((1ull << enda) & low) {
				break;
			}
		}
		for (int i = 45; i >= enda; i--) {
			BIT_WRITER.writeBit(((1ull << i) & low) ? 1 : 0);
		}
		BIT_WRITER.flush();

		fclose(outputFile);


		//aaa
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
		}
		else {
			sprintf(OUTPUT, "%s", outputFileName);
		}

		FILE *inputFile = fopen(inputFileName, "rb");
		FILE *outputFile = fopen(OUTPUT, "wb");

		BIT_READER.init(inputFile);



		//BIT_WRITER.init(outputFile);
		inputFileSize = BIT_READER.readInt();

		//printf("%lld\n", inputFileSize);

		BIT bit;
		bit.init();
		for (int i = 0; i < 256; i++) {
			bit.inc(i + 1);
		}


		unsigned long long high = 255;
		unsigned long long low = 0;
		unsigned long long now = 0;
		unsigned long long tmphigh;
		unsigned long long tmplow;

		for (int i = 0; i < 45+1; i++) {
			now <<= 1;
			now |= BIT_READER.readBit();
		}
		//now <<= 1;

		//printf("%d\n", high);
		//printf("%d\n", low);
		//printf("%d\n", now);


		unsigned char chr;
		std::deque<unsigned char> chars;
		chars.clear();

		for (size_t i = 0; i < inputFileSize; i++) {
			while (high - low < bit.sum*2) {

				if (low & (1ull << 45)) {
					high -= (1ull << 45);
					low -= (1ull << 45);
					now -= (1ull << 45);
				}
				else {

				}

				high <<= 1;
				//high |= 1;
				low <<= 1;
				now <<= 1;
				now |= BIT_READER.readBit();
			}


			unsigned int l = 0;
			unsigned int r = 256;
			unsigned int mid;
			while (1) {
				if (r - l == 1) {
					break;
				}
				mid = ((l + r) >> 1);
				if (low + (high - low) * bit.getsum(mid) / bit.sum > now) {
					r = mid;
				}
				else {
					l = mid;
				}
			}

			chr = l;
			fwrite(&chr, sizeof(unsigned char), 1, outputFile);
			//printf("%d\n",(int)chr);

			unsigned long long lows = bit.getsum(chr);
			unsigned long long highs = bit.getsum(chr + 1);
			tmplow = low + (high - low) * lows / bit.sum;
			tmphigh = low + (high - low) * highs / bit.sum;
			low = tmplow;
			high = tmphigh;

			if (high == low) {
				puts("error");
			}

			/**
			if (low >= (1ull << 45+1)) {
				int i = 63;
				for (; ; i--) {
					if (low & (1ull << i)) {
						break;
					}
				}
				for (; i >= 45+1; i--) {
					if (low & (1ull << i)) {
						high -= (1ull << i);
						low -= (1ull << i);
						now -= (1ull << i);
					}
					else {
					}
				}
			}
			*/


			bit.inc(chr + 1);
			chars.push_back(chr);
			if (chars.size() == 2048) {
				bit.dec(chars.front() + 1);
				chars.pop_front();
			}
		}

		fclose(inputFile);
		fclose(outputFile);
	}
}

#endif