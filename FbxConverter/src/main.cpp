//David Oguns
//December 27, 2012
//main.cpp -- FbxConverter

#include <iostream>
#include <fstream>
#include <string>
#include "main.h"
#include <fbxsdk.h>

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
	string meshName(argv[2]);
	string outputFileName(argv[3]);

	cout	<< "Opening FBX file for reading: " << meshFileName << endl
			<< "Using mesh in FBX file named: " << meshName << endl
			<< "Outputting result to AM file: " << outputFileName << endl;

	//open file for writing
	ofstream of(outputFileName, ios::binary | ios::trunc);
	FbxManager *pFbxMgr = nullptr;
	FbxMesh *pMesh = nullptr;
	if(!of.is_open()) 
	{
		cout << "Could not open file for writing: " << outputFileName << endl;
		exit(-1);
	}
	else
	{
		//retrieve specified mesh from file
		pMesh = getMeshByName(pFbxMgr, meshFileName, meshName);
		if(pMesh)
		{
			if(!pMesh->IsTriangleMesh())
			{
				FbxGeometryConverter fbxGc(pFbxMgr);
				//can't use triangulate in place unless we have original FbxNode
				pMesh = fbxGc.TriangulateMesh(pMesh); 
			}
			//compute normals, necessary information for lighting the mesh
			pMesh->ComputeVertexNormals();

			//write mesh to file
			writeAccevoMesh(of, pMesh);
		}
		else {
			cout << "No mesh found in FBX file named: [" << meshName << "]" << endl;
		}
	}

	of.close();	//close before exiting
	if(pMesh)
	{
		pMesh->Destroy();
		pMesh = nullptr;
	}
	std::system("PAUSE");
	return 0;
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
			static_cast<float>(vertex[3]),
			static_cast<float>(normal[0]),
			static_cast<float>(normal[1]),
			static_cast<float>(normal[2]),
			static_cast<float>(normal[3]),
		};
		of.write((const char *)&unitOut, sizeof(float)*8);
	}

	for(unsigned _int32 iIndex = 0; iIndex < iCount; ++iIndex)
	{
		unsigned _int32 indexOut = static_cast<unsigned _int32>(pMesh->mPolygonVertices[iIndex]);
		of.write((const char *)&indexOut, sizeof(unsigned _int32));
	}
}

/*
 * 
 */
FbxMesh * getMeshByName(FbxManager* &pFbxMgr, string const & fbxFileName, string const & meshName)
{
	FbxMesh *pMesh = nullptr;
	pFbxMgr = FbxManager::Create();
	if(pFbxMgr)
	{
		//create settings
		FbxIOSettings *pFbxSettings = FbxIOSettings::Create(pFbxMgr, IOSROOT);
		//apply settings
		
		pFbxMgr->SetIOSettings(pFbxSettings);
		//create importer
		FbxImporter *pFbxImp = FbxImporter::Create(pFbxMgr, "");
		//test import of file
		if(!pFbxImp->Initialize(fbxFileName.c_str(), -1, pFbxMgr->GetIOSettings()))
		{
			cout << "Call to FbxImporter::Initialize() failed.\n";
			cout << "Error returned: " << pFbxImp->GetLastErrorString() << endl;
			std::system("PAUSE");
			exit(-1);	//return early, blow up
		}//as normal
		//initialize scene object
		FbxScene *pFbxScene = FbxScene::Create(pFbxMgr, "myScene");
		//import content of file into scene object
		pFbxImp->Import(pFbxScene);

		//don't need importer anymore
		pFbxImp->Destroy();
		pFbxImp = nullptr;

		FbxNode *pFbxRootNode = pFbxScene->GetRootNode();
		if(pFbxRootNode) 
		{
			for(int i = 0; i < pFbxRootNode->GetChildCount(); i++)
			{
				FbxNode *pChild = pFbxRootNode->GetChild(i);
				cout << "Examining node with name: [" << pChild->GetName() << "]" << endl;
				if(meshName.compare(pChild->GetName()) == 0)
				{
					pMesh = pChild->GetMesh();		//may return null
				}
			}
		}
	}

	return pMesh;
}