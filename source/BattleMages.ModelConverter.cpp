// BattleMages.ModelConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>

struct GSMHeader {
	int   header;
	int   unk;
	int   verts;
	int   tris;
	char  pad;
	// string here
};

struct MDLHeader {
	int    header1;
	int    header2;
	int    type;
	int    version;
};

struct StaticHeader {
	MDLHeader   mdl;
	int         headerSize;
	int         headerOffset;
	char        _pad[8];
	int         vertCount;
	int         vertStart;
	char        __pad[8];
	int         faceCount;
	int         faceStart;
	char        ___pad[36];
	GSMHeader   gsm;
};



int main(int argc, char* argv[])
{

	if (argc != 2) {
		std::cout << "Usage: modelcon <input>" << std::endl;
		return 1;
	}

	std::ifstream pFile(argv[1], std::ifstream::binary);

	if (!pFile) {
		std::cout << "ERROR: Could not open " << argv[1] << "!" << std::endl;
		return 1;
	}

	if (pFile)
	{
		StaticHeader sth;
		pFile.read((char*)&sth, sizeof(StaticHeader));
	    
		if (sth.mdl.header1 != 'nbce') {
			std::cout << "ERROR: " << argv[1] << " is not a BM model file!" << std::endl;
			return 1;
		}

		if (sth.mdl.type != 5) {
			std::cout << "ERROR: " << argv[1] << " is not a static model!" << std::endl;
			return 1;
		}

		std::cout << "Processing..." << std::endl;

		std::string textureName;
		pFile.seekg(-3, pFile.cur);
		std::getline(pFile, textureName, '\0');

		// create output
		std::string output = argv[1];
		output += "_conv.obj";
		std::string material = argv[1];
		material += "_conv.mtl";
		std::ofstream oFile(output, std::ofstream::binary);
		std::ofstream mtlFile(material, std::ofstream::binary);

		// get verts/normals/uvs
		pFile.seekg(sth.vertStart, pFile.beg);

		oFile << "# Converted using Battle Mages Model Converter" << std::endl;
		oFile << "mtllib " << material << std::endl;

		for (int i = 0; i < sth.gsm.verts; i++)
		{
			// 3 verts, 3 normals, 2 uv
			float temp[8];
			pFile.read((char*)&temp, sizeof(temp));
			oFile << "v " <<  temp[0] << " " << temp[1] << " " << temp[2] << std::endl;
			oFile << "vn " <<  temp[3] << " " << temp[4] << " " << temp[5] << std::endl;
			// - 1 to flip y axis
			oFile << "vt " <<  temp[6] << " " << 1.0 - temp[7] << std::endl;

		}

		pFile.seekg(sth.faceStart, pFile.beg);

		// create name
		std::string name = argv[1];
		int dot = name.find_last_of(".");
		std::string groupname = name.substr(0, dot);



		oFile << "g " << groupname << std::endl;
		oFile << "usemtl mat" << std::endl;


		for (int i = 0; i < sth.gsm.tris; i++)
		{
			short temp[3];
			pFile.read((char*)&temp, sizeof(temp));
			oFile << "f " << temp[0] + 1 << "/" << temp[0] + 1 
				<<  " " 
				<< temp[1] + 1 << "/"  << temp[1] + 1 
				<< " " 
				<< temp[2] + 1 << "/" <<temp[2] +1 << std::endl;

		}

		std::cout << "Generating mtl.." << std::endl;

		mtlFile << "# Created using Battle Mages Model Converter" << std::endl;
		mtlFile << "newmtl mat" << std::endl;
		mtlFile << "Kd 1.00 1.00 1.00" << std::endl;
		mtlFile << "map_Kd " << textureName << std::endl;
		std::cout << "Finished." << std::endl;

	}
    return 0;
}

