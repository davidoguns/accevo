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
void PrintTabs()
{
    for(int i = 0; i < numTabs; i++)
        printf("\t");
}

/**
 * Return a string-based representation based on the attribute type.
 */
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type)
{
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
void PrintAttribute(FbxNodeAttribute* pAttribute)
{
    if(!pAttribute) return;
 
    FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    FbxString attrName = pAttribute->GetName();
    PrintTabs();
    // Note: to retrieve the chararcter array of a FbxString, use its Buffer() method.
    printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

/**
  * [1] -- FBX file to load
  * [2] -- Output path to write to
  * 
  */
int main(int argc, char ** argv)
{
	for(int i = 0; i < argc; ++i)
	{
		cout << "arg[" << i << "]: " << argv[i] << endl;
	}

	if(argc < 2)
	{
		cout << "Must specify at least FBX file, name of mesh, and output file path" << endl;
		return -1;
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
	std::list<FbxVector2>::const_iterator uvItr = mesh.uv.cbegin();
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

	static int const vertexSizeNumFloats = 12;
	//write vertices with normals
	while(vItr != mesh.vertices.cend())
	{
		maxX = std::max<float>(maxX, static_cast<float>(vItr->mData[0]));
		maxY = std::max<float>(maxY, static_cast<float>(vItr->mData[1]));
		maxZ = std::max<float>(maxZ, static_cast<float>(vItr->mData[2]));

		minX = std::min<float>(minX, static_cast<float>(vItr->mData[0]));
		minY = std::min<float>(minY, static_cast<float>(vItr->mData[1]));
		minZ = std::min<float>(minZ, static_cast<float>(vItr->mData[2]));

		float v[vertexSizeNumFloats] =
		{
			static_cast<float>(vItr->mData[0]),
			static_cast<float>(vItr->mData[1]),
			static_cast<float>(vItr->mData[2]),
			1.0f,
			static_cast<float>(uvItr->mData[0]),
			static_cast<float>(uvItr->mData[1]),
			0.0f,
			0.0f,
			static_cast<float>(nItr->mData[0]),
			static_cast<float>(nItr->mData[1]),
			static_cast<float>(nItr->mData[2]),
			0.0f
		};
		of.write((char const *)v, sizeof(float)*vertexSizeNumFloats);

		++vItr; ++nItr;	++uvItr;	//iterating over three structures at the same time
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
		}
	}

	FbxMesh *pMesh = node->GetMesh();
	if(pMesh)
	{
		//if we don't have a triangle mesh...make it one
		if(!pMesh->IsTriangleMesh())
		{
			cout << "Performing triangulation..." << endl;
			FbxGeometryConverter fbxGc(pFbxMgr);
			pMesh = (FbxMesh*)fbxGc.Triangulate(pMesh, true);	//could triangulate in place
			//fbxGc.Triangulate(pMesh, true);	//could triangulate in place
		}

		pMesh->GenerateNormals(true, true, false);	//compute normals
		FbxLayerElementArrayTemplate<FbxVector4> *normals = nullptr;	//create place to hold normals
		pMesh->GetNormals(&normals);	//write normals here
		
		cout << "UV layer count " << pMesh->GetUVLayerCount() << endl;
		FbxStringList lUVSetNameList;
		pMesh->GetUVSetNames(lUVSetNameList);

		FbxLayerElementArrayTemplate<FbxVector2> *uvCoordinates = nullptr;

		FbxLayerElementArrayTemplate<int> *pTextureIndices = nullptr;


		//iterating over all uv sets -- TODO: limit to just one set? What useful situation would have more than one?
		for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
		{

			pMesh->InitTextureIndices(FbxLayerElement::EMappingMode::eByControlPoint, FbxLayerElement::eTextureDiffuse);
			//get lUVSetIndex-th uv set
			const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
			cout << "UV set name: {" << lUVSetIndex << "}" << lUVSetName << endl;
			pMesh->GetTextureUV(&uvCoordinates, FbxLayerElement::eTextureDiffuse);
			int uvCount = uvCoordinates->GetCount();
			cout << "Number of texture coordinates: " << uvCount  << endl;
			
			if(uvCount != pMesh->mPolygonVertices.Size())
			{
				cerr << "Number of indices (" << pMesh->mPolygonVertices.Size() <<
					") not equal to the number of texture coordinates (" << uvCount <<
					"). Skipping this set..." << endl;
				
			}
			pMesh->GetTextureIndices(&pTextureIndices, FbxLayerElement::eTextureDiffuse);
			cout << "Number of texture indices: " << pTextureIndices->GetCount() << endl;
		}
		
		unsigned _int32 indexOffset = static_cast<unsigned _int32>(mesh.indices.size());
		for(_int32 vIndex = 0; vIndex < pMesh->GetControlPointsCount(); ++vIndex)
		{
			mesh.vertices.push_back(MultiplyMatrixCM(node->EvaluateGlobalTransform(), pMesh->mControlPoints[vIndex]));
			mesh.normals.push_back(MultiplyMatrixCM(node->EvaluateGlobalTransform(), (*normals)[vIndex]));
			mesh.uv.push_back(FbxVector2((*uvCoordinates)[vIndex]));
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
 */
FbxVector4 MultiplyMatrixCM(FbxAMatrix const &matrix, FbxVector4 const &vector)
{
	return FbxVector4(
		matrix.GetColumn(0).DotProduct(vector),
		matrix.GetColumn(1).DotProduct(vector),
		matrix.GetColumn(2).DotProduct(vector),
		matrix.GetColumn(3).DotProduct(vector));
}