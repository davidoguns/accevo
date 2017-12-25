//David Oguns
//December 27, 2012

#pragma once

#include <fbxsdk.h>
#include <fstream>
#include <string>
#include <list>
#include <stack>

struct Mesh
{
	std::list<FbxVector4>			vertices;
	std::list<FbxVector2>			uv;
	std::list<FbxVector4>			normals;
	std::list<unsigned _int32>		indices;
	unsigned _int32					nPolygons;
	std::string						*pStrDiffuseFile;
};	//growing mesh

/**
  * Get mesh data recursively.
  */
void getMeshData(FbxManager *pFbxMgr, Mesh &mesh, FbxNode *node);

/*
 * Writes the supplied mesh from the FBX SDK.
 *
 */
void writeAccevoMesh(std::ofstream &of, Mesh const &mesh);

/*
 *	Multiply a position vector by an affine matrix
 *  CM - column-major
 */
FbxVector4 MultiplyMatrixCM(FbxAMatrix const &matrix, FbxVector4 const &vector);
