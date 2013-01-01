//David Oguns
//December 28, 2012

#pragma once

#include "aetypes.h"
#include "GraphicsLayer.h"
#include <D3D11.h>
#include <boost\noncopyable.hpp>

namespace Accevo {

class Mesh : public boost::noncopyable
{
public:
	Mesh(GraphicsLayer *pGraphics, wchar_t const * file);	//self loading in this temporary solution approach

	virtual ~Mesh();

	ABOOL IsInitialized() { return m_bInitialized; }

	ID3D11Buffer* GetIndexBuffer() { return m_pIndexBuffer; }
	ID3D11Buffer* GetVertexBuffer() { return m_pVertexBuffer; }

	AUINT32 GetNumVerticies() { return m_nVertices; }
	AUINT32 GetNumIndices() { return m_nIndices; }
	AUINT32 GetNumPolygon() { return m_nPolygons; }

	//need to graduate from this...but for this level work...whatever
	virtual void Draw(ID3D11DeviceContext *pDevice);

protected:
	ABOOL				m_bInitialized;			//true if initialized successfully
	ID3D11Buffer		*m_pIndexBuffer;		//with normals
	ID3D11Buffer		*m_pVertexBuffer;
	AUINT32				m_nPolygons;
	AUINT32				m_nVertices;
	AUINT32				m_nIndices;
};

}