#ifndef __PPMOPR_H_
#define __PPMOPR_H_

#include <vector>
#include <string>
#include <fstream>

class inputPara {
public:
	std::string inFilepath;
	std::string outFilepath;
	int top_left_x;
	int top_lfet_y;
};

/* class ppm - store ppm data */
class ppm {
public:
	int type;
	std::string comment;
	int width;
	int height;
	unsigned char maxgraydepth;
	size_t headerover; /* soon to be deprecated */ 
	std::vector<unsigned char> data;
};

/* class ppmop - ppm operations */
class ppmop {
public:
	ppmop() {}
	~ppmop() {}
	int read(std::string filepath, ppm& inppm);
	int ppmScale(ppm& inppm, int sc_width, int sc_height, ppm& outppm);
	int writeppm(std::string filepath, ppm& outppm);
	
	int writeppmData(ppm& ppm, std::string str) {
		for (size_t i = 0; i < str.size(); i++) {
			ppm.data.emplace_back(str[i]);
		}

		return 0;
	}
	int writeppmData(ppm& ppm, unsigned char data) {
			ppm.data.emplace_back(data);
			return 0;
	}
	int writeNewDatatoppm(ppm& addppm, ppm& targetppm, int left, int top, int width, int height);
	ppm creatBackground(unsigned int width, unsigned int height, unsigned char r, unsigned char g, unsigned char b);
	ppm writePgm(std::string filepath, unsigned char data);
	
private:
	int getppmInfo(std::vector<unsigned char>data, ppm& outppm);
	int readPpmToBuffer(std::string filepath, ppm& inppm, std::vector<unsigned char>& in_data);
};

/* getppmInfo() function
 * from input data create and fill an inppm class
 * @ in  - in_data: read from input file
 * @ out - inppm : get the ppm class
*/
int ppmop::getppmInfo(std::vector<unsigned char> in_data, ppm& inppm) {
	std::vector<unsigned char>::iterator ppm_iter;
	std::vector<unsigned char>::iterator tmp_iter;
	std::string width_str;
	std::string height_str;
	std::string gray_str;

	if (in_data[0] != 'P') {
		std::cout << "ERROR: wrong image format input, ONLY SUPPORT PPM" << std::endl;
	}
	inppm.type = in_data[1] - '0';

	// skip the header of ppm
	ppm_iter = in_data.begin() + 2; // skip magic number

	while (ppm_iter != in_data.end() && isspace(*(ppm_iter))) { ppm_iter++; } //skip whitespace

	//skip header comments
	while (ppm_iter != in_data.end() && *ppm_iter == '#') {
		while (ppm_iter != in_data.end()) {
			ppm_iter++;
			if (*ppm_iter == '\n' || *ppm_iter == '\r')
				break;
		}
		ppm_iter++;
	}
	inppm.comment = "# power by Chong \n";

	//skip white space
	while (ppm_iter != in_data.end() && isspace(*(ppm_iter))) { ppm_iter++; }

	//skip width space height 
	tmp_iter = find(ppm_iter, in_data.end(), ' ');  // width
	inppm.width = stoi(width_str.assign(ppm_iter, tmp_iter));
	ppm_iter = tmp_iter++;

	tmp_iter = find_if(ppm_iter, in_data.end(), [](unsigned char a) {return (a == '\n') || (a == '\r'); }); // height
	inppm.height = stoi(height_str.assign(ppm_iter, tmp_iter));
	ppm_iter = tmp_iter++;

	tmp_iter = find_if(++ppm_iter, in_data.end(), [](unsigned char a) {return (a == '\n') || (a == '\r'); }); // maxgraydepth
	inppm.maxgraydepth = stoi(gray_str.assign(ppm_iter, tmp_iter));
	ppm_iter = tmp_iter++;

	inppm.headerover = distance(in_data.begin(), ppm_iter);
	inppm.headerover ++;

	inppm.data.insert(inppm.data.end(), ++ppm_iter, in_data.end());

	return 0;
}

/* readPpmToBuffer() function
 * read a ppm file to buffer 
 * @ in  - filepath: the input file path
 * @ out - inppm : get the ppm class
 * @ out - in_data : get the data buffer
*/
int ppmop::readPpmToBuffer(std::string filepath, ppm& inppm, std::vector<unsigned char>& in_data)
{
	typedef std::istream_iterator<unsigned char> istream_iterator;
	std::ifstream in_fd(filepath, std::ios::in | std::ios::binary);
	//in_fd.seekg(0, in_fd.end);
	//long filesize = in_fd.tellg();
	//unsigned char buffer[] = { 0 };
	//if (filesize <= 0) {
	//	std::cout << "filesize is " << filesize << " <= 0" << std::endl;
	//	return -1;
	//}

	// write ppm to buffer
	//in_fd >> std::skipws; //skip space
	//std::copy(istream_iterator(in_fd), istream_iterator(), std::back_inserter(inppm.data)); //will skip \n \r
	in_fd.unsetf(std::ifstream::skipws);

	while (in_fd.good()) {
		in_data.emplace_back(in_fd.get());
	}

	if (in_data[0] != 'P' || in_data[1] != '6')
	{
		std::cout << "ERROR: Image is not P6 ppm format\n" << std::endl;
		return -1;
	}
	
	//read the header of the ppm
	int ret = getppmInfo(in_data, inppm);
	if (ret < 0) {
		std::cout << "ERROR: didn't get the head message from ppm" << std::endl;
		return -1;
	}
	in_fd.close();
	return 0;
}

/* read() function
 * read a ppm file to ppm class
 * @ in  - filepath: the input file path
 * @ out - inppm : get the ppm class
*/
int ppmop::read(std::string filepath, ppm& inppm)
{
	std::size_t split_num;
	std::string path;   // no include name of file
	std::string filename;
	std::string fileext;
	std::vector<unsigned char> inData;
	
	// get file extension string
	split_num = filepath.find_last_of("/\\"); //find_last_of will match any characters in its arguments.
	path = filepath.substr(0, split_num);
	filename = filepath.substr(split_num + 1);

	split_num = filename.find_last_of(".");
	fileext = filename.substr(split_num + 1);

	std::cout << "Will read file at: " << path << "\nfilename: " << filename << std::endl;

	// will support .bz2
	if (fileext.compare("ppm") == 0) {
		readPpmToBuffer(filepath, inppm, inData);
	}
	else {
		std::cout << "\nERROR: Unsupported input image file format:" << fileext << std::endl;
		return -1;
	}

	return 0;
}

/* writeppm() function
 * write a ppm to a file
 * @ in - filepath: the output file path
 * @ in - outppm :  the ppm class
*/
int ppmop::writeppm(std::string filepath, ppm& outppm) {
	std::ofstream out_fd;
	std::vector<unsigned char> out_data(outppm.data);

	out_fd.open(filepath, std::ios::out | std::ios::binary);
	std::string ppm_header = "P6\n";
	ppm_header += "# power by chong\n";
	ppm_header += std::to_string(outppm.width) + " " + std::to_string(outppm.height) + '\n';
	ppm_header += std::to_string(outppm.maxgraydepth) + '\n';

	out_data.insert(out_data.begin(), ppm_header.begin(), ppm_header.end());

	std::copy(out_data.begin(), out_data.end(), std::ostream_iterator<unsigned char>(out_fd));
	std::cout << "Done!" << std::endl;

	return 0;
}

/*
	sc_width  - target width
	sc_height - target height
*/
int ppmop::ppmScale(ppm& inppm, int sc_width, int sc_height, ppm& outppm) {
	outppm.type = inppm.type;
	outppm.comment = "# power by chong";
	outppm.width = sc_width;
	outppm.height = sc_height;
	outppm.maxgraydepth = inppm.maxgraydepth;
	outppm.data.resize(sc_width * sc_height * 3);

	/* Bilinear scale algorithm */
	int sc_w = (inppm.width - 2) * 1000 / sc_width;
	int sc_h = (inppm.height - 2)  * 1000 / sc_height;
	
	double sc_x, sc_y, f1, f2, result;
	int bq00, bq01, bq10, bq11;
	unsigned char fq00, fq01, fq10, fq11;
	int bx, by;
	for (int j = 0; j < sc_height; ++j) {
		for (int i = 0; i < sc_width; ++i) {
			sc_y = sc_h * j / 1000;
			sc_x = sc_w * i / 1000;
			by = int(sc_y);    //向下取整
 			bx = int(sc_x);    
			bq00 = by * inppm.width * 3 + bx * 3;
			bq01 = (by + 1) * inppm.width * 3 + bx * 3;
			bq10 = by * inppm.width * 3 + (bx + 1) * 3;
			bq11 = (by + 1) * inppm.width * 3 + (bx + 1) * 3;

			/* write three RGB channel to vector */
			for (int k = 0; k < 3; ++k) {
				fq00 = inppm.data[bq00] & 0xFF;
				fq01 = inppm.data[bq01] & 0xFF;
				fq10 = inppm.data[bq10] & 0xFF;
				fq11 = inppm.data[bq11] & 0xFF;

				f1 = (bx + 1 - sc_x) * fq00 + (sc_x - bx) * fq10; // bq10 - bq00 = 1
				f2 = (bx + 1 - sc_x) * fq01 + (sc_x - bx) * fq11; // bq01 - bq00 = 1

				result = (by + 1 - sc_y) * f1 + (sc_y - by) * f2;

				outppm.data[j * sc_width * 3 + i * 3 + k] = result;
				bq00++;
				bq01++;
				bq10++;
				bq11++;
			}
		}
	}

	return 0;
}


int ppmop::writeNewDatatoppm(ppm& addppm, ppm& targetppm, int left, int top, int width, int height)
{
	//std::size_t add_p = addppm.headerover;
	//std::size_t target_p = targetppm.headerover;
	std::string tmp_str;

	// replace data newppm.data ==> targetppm.data
	if (left + width > targetppm.width || top + height > targetppm.height)
		std::cout << "It will not finish displaying the picture " << std::endl;

	for (int j = 0; j < height && (top + j) < targetppm.height; ++j)
	{
		for (int i = 0; i < width && (left + i) < targetppm.width; ++i) {
			size_t target_loc =  (j + top) * targetppm.width * 3 + (i + left) * 3;
			size_t add_loc =  j * addppm.width * 3 + i * 3;
			if (!(addppm.data[add_loc] < 0x14 && addppm.data[add_loc + 1] < 0x14 && addppm.data[add_loc + 2] < 0x14)) {
				targetppm.data[target_loc] = addppm.data[add_loc];
				targetppm.data[target_loc + 1] = addppm.data[add_loc + 1];
				targetppm.data[target_loc + 2] = addppm.data[add_loc + 2];
			}
				
		}
	}
	
	return 0;
}

ppm ppmop::creatBackground(unsigned int width, unsigned int height, unsigned char r, unsigned char g, unsigned char b)
{
	ppm background;
	std::ofstream out_fd;
	int background_size = width * height * 3;

	background.type = 6;
	background.comment = "# power by Chong";
	background.height = height;
	background.width = width;
	background.maxgraydepth = 255;

	//writeppmData(background, "P6\n");
	//writeppmData(background, "# power by Chong\n");
	//writeppmData(background, std::to_string(width) + " ");
	//writeppmData(background, std::to_string(height) + '\n');
	//writeppmData(background, "255\n");

	background.headerover = std::distance(background.data.begin(), background.data.end());

	for (int i = 0; i < background_size; i = i + 3) {
		writeppmData(background, r);
		writeppmData(background, g);
		writeppmData(background, b);
	}

	return background;
}

ppm ppmop::writePgm(std::string filepath, unsigned char data)
{
	ppm pgm;

	pgm.type = 5;
	pgm.width = 1920;
	pgm.height = 1440;
	pgm.maxgraydepth = 255;
	
	for (int i = 0; i < pgm.width * pgm.height * 3; ++i)
		pgm.data.emplace_back(data);

	std::ofstream out_fd;
	std::vector<unsigned char> out_data(pgm.data);

	out_fd.open(filepath, std::ios::out | std::ios::binary);
	std::string ppm_header = "P5\n";
	ppm_header += std::to_string(pgm.width) + " " + std::to_string(pgm.height) + '\n';
	ppm_header += std::to_string(pgm.maxgraydepth) + '\n';

	out_data.insert(out_data.begin(), ppm_header.begin(), ppm_header.end());

	std::copy(out_data.begin(), out_data.end(), std::ostream_iterator<unsigned char>(out_fd));
	std::cout << "Done!" << std::endl;

	return pgm;
}

#endif
