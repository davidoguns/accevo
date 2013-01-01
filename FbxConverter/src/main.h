//David Oguns
//December 27, 2012

#pragma once

#include <fbxsdk.h>
#include <fstream>
#include <string>

/*
 * Writes the supplied mesh from the FBX SDK.
 *
 */
void writeAccevoMesh(std::ofstream &of, FbxMesh *pMesh);


/*
 * Given a root node imported by the importer, returns the
 * mesh node with the given name.
 */
FbxMesh * getMeshByName(FbxManager *& pFbxMgr, std::string const & fbxFileName, std::string const & meshName);

