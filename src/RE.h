#ifndef RE_H_
#define RE_H_

namespace RE {
	struct BIT {
		long long sum;
		long long a[260];
		inline int lowbit(int &x) {
			return x & (-x);
		}
		void init() {
			memset(a, 0, sizeof(a));
			sum = 0;
		}

		void add(int x, long long t) {
			for (int i = x; i < 260; i += lowbit(i)) {
				a[i] += t;
			}
		}
		void inc(int x) {
			for (int i = x; i < 260; i += lowbit(i)) {
				++a[i];
			}
		}
		void dec(int x) {
			for (int i = x; i < 260; i += lowbit(i)) {
				--a[i];
			}
		}
		long long getsum(int x) {
			long long res = 0;
			for (int i = x; i; i -= lowbit(i))
				res += a[i];
			return res;
		}
	};

	void compress(const char* inputFileName, const char* outputFileName = NULL) {
		
		if (outputFileName == NULL) {
			sprintf(OUTPUT, "%s.lzhra", INPUT_FINAL);
		}
		else {
			sprintf(OUTPUT, "%s", outputFileName);
		}

		FILE *inputFile = fopen(inputFileName, "rb");
		FILE *outputFile = fopen(OUTPUT, "wb");
		
		
		BIT bit;
		bit.init();
		for (int i = 0; i < 255; i++) {
			bit.inc(i+1);
		}
		inputFileSize = getFileSize(inputFileName);

		unsigned long long high = 256;
		unsigned long long low = 0;


		unsigned char chr;
		for (size_t i = 0; i < inputFileSize;i++) {
			fread(&chr,sizeof(chr),1,inputFile);
			

		}


		//aaa
	}


}

#endif