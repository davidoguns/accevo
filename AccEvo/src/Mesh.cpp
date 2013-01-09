//David Oguns
//December 29, 2012
//Mesh.h

#include "Mesh.h"
#include "DXUtil.h"
#include <fstream>
#include "EngineKernel.h"
#include <boost\format.hpp>

using boost::wformat;
using namespace std;

namespace Accevo
{

//self loading in this temporary solution approach
Mesh::Mesh(GraphicsLayer *pGraphics, wchar_t const * file) :
	m_nVertices(0),
	m_nIndices(0),
	m_nPolygons(0),
	m_pVertexBuffer(nullptr),
	m_pIndexBuffer(nullptr),
	m_bInitialized(false)
{
	ID3D11Device *pDev = pGraphics->GetDevice();
	
	ifstream fin(file, ios::binary);
	if(fin)
	{
		//load counts
		fin.read((char*)&m_nVertices, sizeof(AUINT32));
		fin.read((char*)&m_nIndices, sizeof(AUINT32));
		fin.read((char*)&m_nPolygons, sizeof(AUINT32));

		AELOG_INFO(ENGINE_LOGGER, (wformat(L"Vertices[%1%], Indices[%2%], Polygons[%3%]")%m_nVertices%m_nIndices%m_nPolygons).str().c_str());
	
		D3D11_BUFFER_DESC vBufferDesc;
		ZeroMemory(&vBufferDesc, sizeof(D3D11_BUFFER_DESC));		//clear structure
		vBufferDesc.ByteWidth = m_nVertices * 8 * sizeof(AFLOAT32);	//8 = 4 xzyw pos, 4xyzw normal
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;	//initialized and otherwise not touched
		vBufferDesc.CPUAccessFlags = 0;	//initialized and otherwise not touched
		//create structure to hold initial data
		D3D11_SUBRESOURCE_DATA initVerts;
		ZeroMemory(&initVerts, sizeof(D3D11_SUBRESOURCE_DATA));
		initVerts.pSysMem = new AFLOAT32[m_nVertices * 8];	//allocate enough memory for all vertices
		fin.read((char *)initVerts.pSysMem, sizeof(AFLOAT32) * m_nVertices * 8);	//read all vertices
		AFLOAT32 const *fdata = (AFLOAT32 const *)initVerts.pSysMem;
		for(AUINT32 vIndex = 0; vIndex < m_nVertices; ++vIndex)
		{
			//AELOG_TRACE(ENGINE_LOGGER, (wformat(L"Point: (%1%,%2%,%3%,%4%)")%fdata[0]%fdata[1]%fdata[2]%fdata[3]).str().c_str());
			//ELOG_TRACE(ENGINE_LOGGER, (wformat(L"Normal: (%1%,%2%,%3%,%4%)")%fdata[4]%fdata[5]%fdata[6]%fdata[7]).str().c_str());
			fdata += 8;		//advance 8 float items
		}
		//create buffer
		pDev->CreateBuffer(&vBufferDesc, &initVerts, &m_pVertexBuffer);
		//deallocate system memory
		delete [] initVerts.pSysMem;
		initVerts.pSysMem = nullptr;

		D3D11_BUFFER_DESC iBufferDesc;
		ZeroMemory(&iBufferDesc, sizeof(D3D11_BUFFER_DESC));
		iBufferDesc.ByteWidth = m_nIndices * sizeof(AUINT32);
		iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		iBufferDesc.CPUAccessFlags = 0;	//no cpu access after creation
		D3D11_SUBRESOURCE_DATA initIdx;
		ZeroMemory(&initIdx, sizeof(D3D11_SUBRESOURCE_DATA));
		initIdx.pSysMem = new AUINT32[m_nIndices];
		fin.read((char *)initIdx.pSysMem, sizeof(AUINT32)*m_nIndices);
		//create buffer
		pDev->CreateBuffer(&iBufferDesc, &initIdx, &m_pIndexBuffer);
		//deallocate system memory
		delete [] initIdx.pSysMem;
		//and we're done
		fin.close();
		m_bInitialized = true;
	}
}

Mesh::~Mesh()
{
	DX_RELEASE(m_pVertexBuffer);
	DX_RELEASE(m_pIndexBuffer);
}

//need to graduate from this...but for this level work...whatever
void Mesh::Draw(ID3D11DeviceContext *pDevice)
{
	pDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	AUINT32 stride = 8 * sizeof(AFLOAT32);
	AUINT32 offset = 0;
	pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDevice->DrawIndexed(m_nIndices, 0, 0);
}

}