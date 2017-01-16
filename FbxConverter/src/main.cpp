//David Oguns
//December 27, 2012
//main.cpp -- FbxConverter

#include <iostream>
#include <fstream>
#include <string>
#include "main.h"
#include <fbxsdk.h>
#include <limits>
#include <algorithm>

using namespace std;

/* Tab character ("\t") counter */
int numTabs = 0;

/**
 * Print the required number of tabs.
 */
void PrintTabs() {
    for(int i = 0; i < numTabs; i++)
        printf("\t");
}

/**
 * Return a string-based representation based on the attribute type.
 */
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
    switch(type) {
        case FbxNodeAttribute::eUnknown: return "unidentified";
        case FbxNodeAttribute::eNull: return "null";
        case FbxNodeAttribute::eMarker: return "marker";
        case FbxNodeAttribute::eSkeleton: return "skeleton";
        case FbxNodeAttribute::eMesh: return "mesh";
		case FbxNodeAttribute::eNurbs: return "nurb";
        case FbxNodeAttribute::ePatch: return "patch";
        case FbxNodeAttribute::eCamera: return "camera";
		case FbxNodeAttribute::eCameraStereo:    return "stereo";
		case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
        case FbxNodeAttribute::eLight: return "light";
		case FbxNodeAttribute::eOpticalReference: return "optical reference";
		case FbxNodeAttribute::eOpticalMarker: return "marker";
		case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
		case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
		case FbxNodeAttribute::eBoundary: return "boundary";
		case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
		case FbxNodeAttribute::eShape: return "shape";
		case FbxNodeAttribute::eLODGroup: return "lodgroup";
		case FbxNodeAttribute::eSubDiv: return "subdiv";
        default: return "unknown";
    }
}

/**
 * Print an attribute.
 */
void PrintAttribute(FbxNodeAttribute* pAttribute) {
    if(!pAttribute) return;
 
    FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    FbxString attrName = pAttribute->GetName();
    PrintTabs();
    // Note: to retrieve the chararcter array of a FbxString, use its Buffer() method.
    printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

/**
  * [1] -- FBX file to load
  * [2] -- Name of mesh to export
  * [3] -- Output path to write to
  * 
  */
int main(int argc, char ** argv)
{
	for(int i = 0; i < argc; ++i)
	{
		cout << "arg[" << i << "]: " << argv[i] << endl;
	}

	if(argc < 3)
	{
		cout << "Must specify at least FBX file, name of mesh, and output file path" << endl;
	}
	string meshFileName(argv[1]);
	string outputFileName(argv[2]);

	

	cout	<< "Opening FBX file for reading: " << meshFileName << endl
			<< "Outputting result to AM file: " << outputFileName << endl;

	//open file for writing
	ofstream of(outputFileName, ios::binary | ios::trunc);
	FbxManager *pFbxMgr = FbxManager::Create();
	if(!of.is_open()) 
	{
		cout << "Could not open file for writing: " << outputFileName << endl;
		exit(-1);
	}
	else
	{
		//create settings
		FbxIOSettings *pFbxSettings = FbxIOSettings::Create(pFbxMgr, IOSROOT);
		//apply settings		
		pFbxMgr->SetIOSettings(pFbxSettings);
		//create importer
		FbxImporter *pFbxImp = FbxImporter::Create(pFbxMgr, "");
		//test import of file
		if(!pFbxImp->Initialize(meshFileName.c_str(), -1, pFbxMgr->GetIOSettings()))
		{
			cout << "Call to FbxImporter::Initialize() failed.\n";
			
			cout << "Error returned: " << pFbxImp->GetStatus().GetErrorString() << endl;
			exit(-1);	//return early, blow up
		}//as normal
		//initialize scene object
		FbxScene *pFbxScene = FbxScene::Create(pFbxMgr, "myScene");
		//import content of file into scene object
		pFbxImp->Import(pFbxScene);

		//don't need importer anymore
		pFbxImp->Destroy();
		pFbxImp = nullptr;

		Mesh mesh;
		mesh.nPolygons = 0;
		mesh.pStrDiffuseFile = nullptr;
		getMeshData(pFbxMgr, mesh, pFbxScene->GetRootNode());
		writeAccevoMesh(of, mesh);
	}
	of.close();	//close before exiting
	cout << std::flush;
	return 0;
}

void writeAccevoMesh(std::ofstream &of, Mesh const &mesh)
{
	cout << "Writing " << mesh.indices.size() << " control points and " << mesh.nPolygons << " polygons to file." << endl;
	std::list<FbxVector4>::const_iterator vItr = mesh.vertices.cbegin();
	std::list<FbxVector4>::const_iterator nItr = mesh.normals.cbegin();

	unsigned _int32 num = static_cast<unsigned _int32>(mesh.vertices.size());
	of.write((char const *)&num, sizeof(unsigned _int32));	//write number of vertices
	num = static_cast<unsigned _int32>(mesh.indices.size());
	of.write((char const *)&num, sizeof(unsigned _int32));	//write number of indices (control points)
	of.write((char const *)&mesh.nPolygons, sizeof(unsigned _int32));	//write number of polygons to file

	float maxX(std::numeric_limits<float>::min());
	float maxY(std::numeric_limits<float>::min());
	float maxZ(std::numeric_limits<float>::min());
	float minX(std::numeric_limits<float>::max());
	float minY(std::numeric_limits<float>::max());
	float minZ(std::numeric_limits<float>::max());

	//write vertices with normals
	while(vItr != mesh.vertices.cend())
	{
		maxX = std::max<float>(maxX, static_cast<float>(vItr->mData[0]));
		maxY = std::max<float>(maxY, static_cast<float>(vItr->mData[1]));
		maxZ = std::max<float>(maxZ, static_cast<float>(vItr->mData[2]));

		minX = std::min<float>(minX, static_cast<float>(vItr->mData[0]));
		minY = std::min<float>(minY, static_cast<float>(vItr->mData[1]));
		minZ = std::min<float>(minZ, static_cast<float>(vItr->mData[2]));

		float v[8] =
		{
			static_cast<float>(vItr->mData[0]),
			static_cast<float>(vItr->mData[1]),
			static_cast<float>(vItr->mData[2]),
			static_cast<float>(1.0f),
			static_cast<float>(nItr->mData[0]),
			static_cast<float>(nItr->mData[1]),
			static_cast<float>(nItr->mData[2]),
			static_cast<float>(0.0f)
		};
		of.write((char const *)v, sizeof(float)*8);

		++vItr; ++nItr;	;	//iterating over three structures at the same time
	}
	//write indices
	for(std::list<unsigned _int32>::const_iterator iItr = mesh.indices.cbegin();
		iItr != mesh.indices.cend(); ++iItr)
	{
		unsigned _int32 idx = *iItr;
		of.write((char const *)&idx, sizeof(unsigned _int32));
	}
	if(mesh.pStrDiffuseFile)
	{
		delete mesh.pStrDiffuseFile;
	}
	char buffer[1024];
	sprintf_s<1024>(buffer, "Min(%f,%f,%f) : Max(%f,%f,%f)",
			minX, minY, minZ, maxX, maxY, maxZ);
	cout << "Done writing vertices.  " << buffer << endl;
}

/**
  * Writes mesh out to stream in binary
  */
void writeAccevoMesh(ofstream &of, FbxMesh *pMesh)
{
	unsigned _int32 vCount = pMesh->GetControlPointsCount();	//number of unique vertex
	unsigned _int32 iCount = pMesh->mPolygonVertices.Size();	//number of indices
	unsigned _int32 pCount = iCount / 3;		//if we are dealing with triangles...

	FbxLayerElementArrayTemplate<FbxVector4> *normals = nullptr;	//create place to hold normals
	pMesh->GetNormals(&normals);	//write normals here

	cout << "Number of vertices: " << vCount << endl;
	cout << "Number of indices: " << iCount << endl;
	cout << "Number of normals: " << normals->GetCount() << endl;
	
	//write counts first, will allow straight read
	//can't use static casts here
	of.write((char *)&vCount, sizeof(unsigned _int32));
	of.write((char *)&iCount, sizeof(unsigned _int32));
	of.write((char *)&pCount, sizeof(unsigned _int32));
	//iterate over all of the vertices
	for(unsigned _int32 vIndex = 0; vIndex < vCount; ++vIndex)
	{
		FbxVector4 vertex = pMesh->mControlPoints[vIndex].mData;
		FbxVector4 normal = (*normals)[vIndex];	//use the vertex index to get the normal index
		float unitOut[8] =
		{
			static_cast<float>(vertex[0]),
			static_cast<float>(vertex[1]),
			static_cast<float>(vertex[2]),
			//static_cast<float>(vertex[3]),
			1.0f,
			static_cast<float>(normal[0]),
			static_cast<float>(normal[1]),
			static_cast<float>(normal[2]),
			//static_cast<float>(normal[3]),
			0.0f
		};
		of.write((const char *)&unitOut, sizeof(float)*8);
	}

	for(unsigned _int32 iIndex = 0; iIndex < iCount; ++iIndex)
	{
		unsigned _int32 indexOut = static_cast<unsigned _int32>(pMesh->mPolygonVertices[iIndex]);
		of.write((const char *)&indexOut, sizeof(unsigned _int32));
	}
}

/**
  * Get mesh data recursively.
  */
void getMeshData(FbxManager *pFbxMgr, Mesh &mesh, FbxNode *node)
{
	cout << "Processing node named: " << node->GetName() << endl;
	cout << "Found " << node->GetMaterialCount() << " materials." << endl;
	cout << "Type: " << node->GetTypeName() << endl;
	for(unsigned _int32 mIndex = 0; mIndex < static_cast<unsigned _int32>(node->GetMaterialCount()); ++mIndex)
	{
		FbxSurfaceMaterial *pMaterial = node->GetMaterial(mIndex);
		//FbxSurfacePhong *pSurface
		cout << "In theory material has: " << pMaterial->GetSrcObjectCount<FbxTexture>() << " textures..." << endl;
		FbxProperty property = pMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[mIndex]);
		cout << "Texture channel name: " << FbxLayerElement::sTextureChannelNames[mIndex] << endl;
		if(property.IsValid())
		{
			unsigned _int32 ltCount = property.GetSrcObjectCount<FbxFileTexture>();
			cout << "Num layered textures: " << ltCount << endl;
			for(unsigned _int32 ltIndex = 0; ltIndex < ltCount; ++ltIndex)
			{
				FbxFileTexture *lLayeredTexture = FbxCast <FbxFileTexture>(property.GetSrcObject<FbxFileTexture>(ltIndex));
				cout << "Filename: " << lLayeredTexture->GetFileName() << endl;
				switch(lLayeredTexture->GetTextureUse())
				{
				case FbxTexture::ETextureUse::eBumpNormalMap:
					cout << "bump normal" << endl;
					break;
				case FbxTexture::ETextureUse::eLightMap:
					cout << "light map" << endl;
					break;
				case FbxTexture::ETextureUse::eShadowMap:
					cout << "shadow map" << endl;
					break;
				case FbxTexture::ETextureUse::eSphereReflectionMap:
					cout << "sphere reflection" << endl;
					break;
				case FbxTexture::ETextureUse::eSphericalReflectionMap:
					cout << "spherical reflection" << endl;
					break;
				case FbxTexture::ETextureUse::eStandard:
					cout << "standard" << endl;
					break;
				}
			}

			unsigned _int32 tCount = property.GetSrcObjectCount<FbxFileTexture>();
			cout << "Num textures: " << tCount << endl;
			/*
			FbxFileTexture *texture = property.Get<FbxFileTexture*>();
			if(texture)
			{
				if(!mesh.pStrDiffuseFile)
				{
					cout << "Found initial diffuse texture file: " << texture->GetFileName() << endl;
					mesh.pStrDiffuseFile = new std::string(texture->GetFileName());
				}
				else if(mesh.pStrDiffuseFile->compare(std::string(texture->GetFileName())) != 0)
				{
					cout << "Found another diffuse channel file name.  Old: " << (*mesh.pStrDiffuseFile)
						<< " New: " << texture->GetFileName() << endl;
				}
			}
			else 
			{
				cout << "No texture" << endl;
			}
			*/
		}
	}

	FbxMesh *pMesh = node->GetMesh();
	if(pMesh)
	{
		//if we don't have a triangle mesh...make it one
		if(!pMesh->IsTriangleMesh())
		{
			FbxGeometryConverter fbxGc(pFbxMgr);
			//pMesh = fbxGc.Triangulate(pMesh, true);	//could triangulate in place
			fbxGc.Triangulate(pMesh, true);	//could triangulate in place
		}
		
		pMesh->GenerateNormals(true, true, false);	//compute normals
		FbxLayerElementArrayTemplate<FbxVector4> *normals = nullptr;	//create place to hold normals
		pMesh->GetNormals(&normals);	//write normals here

		unsigned _int32 indexOffset = static_cast<unsigned _int32>(mesh.indices.size());
		for(_int32 vIndex = 0; vIndex < pMesh->GetControlPointsCount(); ++vIndex)
		{
			mesh.vertices.push_back(MultiplyMatrixCM(node->EvaluateGlobalTransform(), pMesh->mControlPoints[vIndex]));
			mesh.normals.push_back(MultiplyMatrixCM(node->EvaluateGlobalTransform(), (*normals)[vIndex]));
		}

		for(_int32 iIndex = 0; iIndex < pMesh->mPolygonVertices.Size(); ++iIndex)
		{
			mesh.indices.push_back(indexOffset + pMesh->mPolygonVertices[iIndex]);
		}

		mesh.nPolygons += pMesh->GetPolygonCount();
	}
	//iterate over all children and recurse
	for(unsigned _int32 childIndex = 0; childIndex < static_cast<unsigned _int32>(node->GetChildCount()); ++childIndex)
	{
		getMeshData(pFbxMgr, mesh, node->GetChild(childIndex));
	}
}

/*
 *	Multiply a position vector by an affine matrix
 *
 */
FbxVector4 MultiplyMatrixCM(FbxAMatrix const &matrix, FbxVector4 const &vector)
{
	return FbxVector4(
		matrix.GetColumn(0).DotProduct(vector),
		matrix.GetColumn(1).DotProduct(vector),
		matrix.GetColumn(2).DotProduct(vector),
		matrix.GetColumn(3).DotProduct(vector));
}

