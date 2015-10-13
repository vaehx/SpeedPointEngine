#include "SMBFile.h"
#include <iostream>

using namespace SpeedPoint;

void main()
{

	std::vector<SSMBMaterial> materials;
	CSMBLoader loader;
	loader.ReadSMBFile("test.smb", materials);

	std::cout << std::endl;



	CSMBWriter writer;
	//writer.Prepare("test2.smb", 0);
	
	//CSMBTestMaterialChunk testMatChunk;
	//testMatChunk.Write(&writer);

	//writer.Close();

	std::cin.ignore();
}