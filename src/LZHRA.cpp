
#include"Common.h"

int main(int argc, char* argv[]) {
	LZH::compress("D:/workspace/LZHRA/1.txt");
	//LZH::compress_DE((char*)"D:/workspace/LZHRA/2.txt.lzh");

	/*
	if (argc > 1) {
		if (strcmp(argv[1], "a") == 0) {
			LZH::compress(argv[2]);
			//BWT::compress((std::string(argv[2]) + std::string(".lzh")).c_str());
			//LZH::compress((std::string(argv[2]) + std::string(".lzh.bwt")).c_str());
		} else if (strcmp(argv[1], "e") == 0) {
			LZH::compress_DE(argv[2]);
		}
	} else {
		puts("help:");
		puts("a FILENAME -> compress");
		puts("e FILENAME -> extract");
	}
	*/

	//compressOneTurn();
	//compressionOneTurn_DE();
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
