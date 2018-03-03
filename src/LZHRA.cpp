
#include"Common.h"

int main(int argc, char* argv[]) {
	//RE::compress("D:\\workspace\\LZHRA\\1.pdf", "D:\\workspace\\LZHRA\\6.re");
	//RE::compress_DE("D:\\workspace\\LZHRA\\6.re", "D:\\workspace\\LZHRA\\2.pdf");

	if (argc == 1) {
		puts("help:");
		puts("a FILENAME -> compress");
		puts("e FILENAME -> extract");
	} else if (argc == 3) {
		char fileName[2000];
		if (strcmp(argv[1], "a") == 0) {
			sprintf(fileName, "%s.lzhra", argv[2]);
			LZH::compress(argv[2],"lzhra.tmp");
			RE::compress("lzhra.tmp", fileName);
			remove("lzhra.tmp");
		} else if (strcmp(argv[1], "e") == 0) {
			sprintf(fileName, "%s", argv[2]);
			for (int i = strlen(fileName);;i--) {
				if (fileName[i] == '.') {
					fileName[i] = 0;
					break;
				}
			}
			RE::compress_DE(argv[2], "lzhra.tmp");
			LZH::compress_DE("lzhra.tmp", fileName);
		}
	} else {
		
	}
	

	return 0;
}

//Map mapa;
//int main() {
//	while (1) {
//		for (int i = 0; i < 1000000; i++) {
//			//std::cout << i << " : " << std::endl;
//			mapa[i];
//		}
//		mapa.clear();
//	}
//}
