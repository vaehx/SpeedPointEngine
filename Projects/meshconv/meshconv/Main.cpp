///////////////////////////////////////////////////////////////////////////////
//
//	MeshConv
//	Copyright (c) 2015, Pascal Rosenkranz
//
//	Mesh Converter for the SpeedPoint Engine
//
//	Takes 3DS, obj or fbx files, does some magic and outputs a .spm file
//
/////////////////////////////////////////////////////////////////////////////////

#include "FileSMB.h"
#include "File3DS.h"
#include "FileSPM.h"
#include <iostream>
#include <cstdarg>

using std::ifstream;
using std::cout;
using std::cin;

void Log(const char* msg, ...)
{
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);

	printf("\n");
}

// This function sets those index buffer pointers to 0 which have no index in it. You should move the fallback subset to the
// end of the subset array AFTER filling.
void ConvertSubsetIndicesFrom3DS(const S3DSNamedObject& object, SModelMeta& model)
{
	// Allocate buffers
	bool *faceAssigned = new bool[object.nFaces];
	memset(faceAssigned, 0, sizeof(bool) * object.nFaces);

	u16 nRemainingFaces = object.nFaces;
	for (u16 iSubset = 0; iSubset < model.nSubsets - 1; ++iSubset)
	{
		model.pSubsets[iSubset].materialName = object.meshMaterials[iSubset].name;
		model.pSubsets[iSubset].nIndices = object.meshMaterials[iSubset].nAffectedFaces * 3;
		if (model.pSubsets[iSubset].nIndices == 0)
		{
			model.pSubsets[iSubset].pIndices = 0;
			continue;
		}

		model.pSubsets[iSubset].pIndices = new u32[model.pSubsets[iSubset].nIndices];

		// Fill indices
		for (u16 iFace = 0; iFace < object.meshMaterials[iSubset].nAffectedFaces; ++iFace)
		{
			faceAssigned[object.meshMaterials[iSubset].pAffectedFaces[iFace]] = true;
			S3DSFace& affectedFace = object.pFaces[object.meshMaterials[iSubset].pAffectedFaces[iFace]];
			model.pSubsets[iSubset].pIndices[iFace * 3] = affectedFace.v1;
			model.pSubsets[iSubset].pIndices[iFace * 3 + 1] = affectedFace.v2;
			model.pSubsets[iSubset].pIndices[iFace * 3 + 2] = affectedFace.v3;
		}

		nRemainingFaces -= object.meshMaterials[iSubset].nAffectedFaces;	
	}

	// Assign those faces to the fallback subset, that have not been assigned to any subset already
	model.pSubsets[model.nSubsets - 1].materialName = "";
	model.pSubsets[model.nSubsets - 1].nIndices = nRemainingFaces * 3;	
	if (nRemainingFaces == 0)
	{
		model.pSubsets[model.nSubsets - 1].pIndices = 0;
		return;
	}

	model.pSubsets[model.nSubsets - 1].pIndices = new u32[nRemainingFaces * 3];
	u16 nAddedFaces = 0;
	for (u16 iFace = 0; iFace < object.nFaces; ++iFace)
	{
		if (faceAssigned[iFace])
			continue;

		model.pSubsets[model.nSubsets - 1].pIndices[nAddedFaces * 3] = object.pFaces[iFace].v1;
		model.pSubsets[model.nSubsets - 1].pIndices[nAddedFaces * 3 + 1] = object.pFaces[iFace].v2;
		model.pSubsets[model.nSubsets - 1].pIndices[nAddedFaces * 3 + 2] = object.pFaces[iFace].v3;

		nAddedFaces++;
		if (nAddedFaces >= nRemainingFaces)
			break; // there are definetly no more unassigned faces
	}

	delete[] faceAssigned;
}

// If necessary, moves the fallback subset to the end of the other subsets
void CompressFilledSubsets(SModelMeta& model)
{	
	if (model.pSubsets[model.nSubsets - 1].nIndices == 0 || model.pSubsets[model.nSubsets - 1].pIndices == 0)
		return;

	u16 numFilled = 0;
	for (u16 iSubset = 0; iSubset < model.nSubsets - 1; ++iSubset)
	{
		if (model.pSubsets[iSubset].nIndices == 0 || model.pSubsets[iSubset].pIndices == 0)	
			break;

		numFilled++;
	}

	// move fallback 
	if (numFilled < model.nSubsets - 1)
	{
		SSubset& fallback = model.pSubsets[model.nSubsets - 1];
		model.pSubsets[numFilled].materialName = fallback.materialName;
		model.pSubsets[numFilled].nIndices = fallback.nIndices;
		model.pSubsets[numFilled].pIndices = fallback.pIndices;

		fallback.materialName.clear();
		fallback.nIndices = 0;
		fallback.pIndices = 0; // pointer copied, no delete
	}
}

void CalculateActualSubsetCount(SModelMeta& model)
{
	u16 numFilled = 0;
	for (; numFilled < model.nSubsets; ++numFilled)
	{
		if (model.pSubsets[numFilled].nIndices == 0 || model.pSubsets[numFilled].pIndices == 0)
			break;
	}

	model.nLoadedSubsets = numFilled;
}

// This function assumes, that all faces are triangles! (numIndices = numFaces * 3)
void ConvertDataFrom3DS(const File3DS& i, std::vector<SModelMeta>& objects)
{
	objects.reserve(i.objects.size());

	for (auto itObject = i.objects.begin(); itObject != i.objects.end(); itObject++)
	{
		if (itObject->nVertices == 0)
		{
			Log("Skipped object '%s' as it has no vertices", itObject->name.c_str());
			continue;
		}		

		SModelMeta model;
		model.name = itObject->name;		

		model.nVertices = itObject->nVertices;
		model.pVertices = new SVertex[model.nVertices];
		for (u64 iVertex = 0; iVertex < model.nVertices; ++iVertex)
		{
			model.pVertices[iVertex].x = itObject->pVertices[iVertex].x;
			model.pVertices[iVertex].y = itObject->pVertices[iVertex].y;
			model.pVertices[iVertex].z = itObject->pVertices[iVertex].z;
			model.pVertices[iVertex].nx = itObject->pVertices[iVertex].nx;
			model.pVertices[iVertex].ny = itObject->pVertices[iVertex].ny;
			model.pVertices[iVertex].nz = itObject->pVertices[iVertex].nz;
			model.pVertices[iVertex].u = itObject->pVertices[iVertex].u;
			model.pVertices[iVertex].v = itObject->pVertices[iVertex].v;
		}

		// nLoadedSubsets will be set to the actual count of subsets later, in case there is a material that does not relate to any face
		model.nLoadedSubsets = 0;		

		model.nSubsets = itObject->meshMaterials.size() + 1; // + 1 for the fallback material at index length - 1
		model.pSubsets = new SSubset[model.nSubsets];
				
		for (u64 iSubset = 0; iSubset < model.nSubsets; ++iSubset)		
			model.pSubsets[iSubset].nIndices = 0;		
		
		ConvertSubsetIndicesFrom3DS(*itObject, model);	
		CompressFilledSubsets(model);
		CalculateActualSubsetCount(model);

		objects.push_back(model);
	}
}

void ConvertMaterialsFrom3DS(const File3DS& i, std::vector<SSMBMaterial>& materials)
{
	for (auto itMaterial = i.materials.begin(); itMaterial != i.materials.end(); itMaterial++)
	{
		SSMBMaterial mat;
		mat.name = itMaterial->material_name;
		mat.textureMap = itMaterial->texturemap.name;
		mat.normalMap = itMaterial->bumpmap.name;
		mat.diffuse = Vec3f(itMaterial->diffuse.r, itMaterial->diffuse.g, itMaterial->diffuse.b);
		mat.roughness = ShininessToRoughness(itMaterial->shininess);
		mat.emissive = Vec3f(0, 0, 0);
		mat.illumModel = eSMBILLUM_BLINN_PHONG;

		materials.push_back(mat);
	}
}

int main(int argc, char* argv[])
{
	ifstream myStream;
	myStream.open("barrel.3ds", ifstream::binary | ifstream::in);
	if (!myStream.is_open())
	{
		cout << "Cannot open file for reading!" << std::endl;
		cin.ignore();
	}

	File3DS myFile(myStream);
	myFile.Read();

	std::vector<SModelMeta> models;
	std::vector<SSMBMaterial> materials;
	ConvertDataFrom3DS(myFile, models);
	ConvertMaterialsFrom3DS(myFile, materials);

	std::cout << models.size() << " models" << std::endl;
	std::cout << materials.size() << " materials" << std::endl;


	// Save materials:
	CSMBWriter smbWriter;
	smbWriter.Prepare("test.smb");
	
	for (auto itMaterial = materials.begin(); itMaterial != materials.end(); itMaterial++)
	{
		CSMBMaterialChunk matChunk;
		matChunk.Write(&smbWriter, *itMaterial);
	}

	smbWriter.Close();


	// Save model:
	CSPMWriter spmWriter;
	for (auto itModel = models.begin(); itModel != models.end(); itModel++)
	{
		string filename = itModel->name + ".spm";
		std::cout << filename << std::endl;
		spmWriter.Write(filename.c_str(), *itModel);
	}



	// Clean up
	for (auto itModel = models.begin(); itModel != models.end(); itModel++)
	{
		delete[] itModel->pVertices;
		itModel->pVertices = 0;
		
		for (u16 iSubset = 0; iSubset < itModel->nSubsets; ++iSubset)
		{
			delete[] itModel->pSubsets[iSubset].pIndices;
			itModel->pSubsets[iSubset].pIndices = 0;
		}

		delete[] itModel->pSubsets;
		itModel->pSubsets = 0;
	}

	models.clear();
	materials.clear();

	myFile.materials.clear();
	for (auto itObject = myFile.objects.begin(); itObject != myFile.objects.end(); itObject++)
	{
		delete[] itObject->pFaces; itObject->pFaces = 0;
		delete[] itObject->pVertices; itObject->pVertices = 0;
		
		for (auto itMeshMat = itObject->meshMaterials.begin(); itMeshMat != itObject->meshMaterials.end(); itMeshMat++)
		{
			delete[] itMeshMat->pAffectedFaces;
			itMeshMat->pAffectedFaces = 0;
		}
	}

	cout << std::endl;
	cin.ignore();
}