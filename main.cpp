#include <iostream>
#include <string>
//#include <unistd.h> // Linux access()
#include <io.h>     // std C++ _access() 
#include "ppmOpr.h"

const std::string usage = "Usage: pnp <input file name> <out file name> <left_top_x> <left_top_y>\n";

#define F_OK 00

int main(int argc, char** argv) {
	inputPara inpara;
	int arg_num = 0;

	//Usage: pnp <input file name> <out file name> <left_top_x> <left_top_y>
	//if (argc < 3) {
	//	std::cout << usage;
	//	return -1;
	//}

	//inpara.inFilepath  = argv[arg_num++];
	//inpara.outFilepath = argv[arg_num++];
	//inpara.top_left_x  = atoi(argv[arg_num++]);
	//inpara.top_lfet_y  = atoi(argv[arg_num++]);
	//if (argc > arg_num) {
	//	std::cout << usage;
	//	return -1;
	//}
	inpara.inFilepath = "nxp.ppm";
	inpara.outFilepath = "nxp_1920_1440.ppm";
	inpara.top_left_x  = 300; //if top_left_x%3 != 0 color will inright.
	inpara.top_lfet_y  = 500;


	//check file exist
	if (_access(inpara.inFilepath.c_str(), F_OK) == -1) {
		std::cout << "ERROR: Input PPM file " << inpara.inFilepath << " does not exist" << std::endl;
		return -1;
	}

	ppm inppm, midppm;
	ppmop ppmopr;
	ppmopr.read(inpara.inFilepath, inppm);
	ppmopr.ppmScale(inppm, 900, 300, midppm);
	ppm canvas = ppmopr.creatBackground(1920, 1440, 0xff, 0xef, 0x00);
	ppmopr.writeNewDatatoppm(midppm, canvas, inpara.top_left_x, inpara.top_lfet_y, midppm.width, midppm.height);

	ppmopr.writeppm("midppm", midppm);
	ppmopr.writeppm(inpara.outFilepath, canvas);

	return 0;
}
