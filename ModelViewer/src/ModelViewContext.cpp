//David Oguns
//February 17, 2011
//ModelViewContext.cpp

#include "ModelViewContext.h"
#include <boost\format.hpp>
#include <accevo\EngineKernel.h>

using boost::format;
using boost::wformat;
using namespace Accevo;

ModelViewContext::ModelViewContext(Accevo::Logger *pLogger) :
	m_pLogger(pLogger),
	m_pMesh(nullptr),
	m_pPS(nullptr),
	m_pVS(nullptr),
	world(XMMatrixIdentity()),
	viewDistance(-80.0f),
	viewHeight(0.0f),
	rotation(0.0f),
	m_pFpsNotifier(nullptr)
{
	AELOG_INFO(m_pLogger, "Starting model view context.");
}

//called when context is about to be run
void ModelViewContext::Start()
{
	m_pKernel = EngineKernel::GetKernel();
	m_pGraphics = (GraphicsLayer*)m_pKernel->GetGraphicsSubSystem();
	m_pInput = (DirectInput*)m_pKernel->GetInputSubSystem();

	//load vertex shader
	wchar_t const * vsFilename = L"effects\\render_mesh.vs";
	m_pVS = new VertexShader(m_pGraphics->GetDevice(), vsFilename);
	AELOG_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, ((wformat(L"Could not load or compile VS from file: %s")%vsFilename).str().c_str()), !m_pVS->IsInitialized(), 
		m_pKernel->Stop();
		return;
	);
	//load pixel shader
	wchar_t const * psFilename = L"effects\\render_mesh.ps";
	m_pPS = new PixelShader(m_pGraphics->GetDevice(), psFilename);
	AELOG_CONDITIONAL_CODE_ERROR(ENGINE_LOGGER, ((wformat(L"Could not load or compile PS from file: %s")%psFilename).str().c_str()), !m_pPS->IsInitialized(), 
		m_pKernel->Stop();
		return;
	);

	//might have to call this more than once per frame...but since we know this his the only thing running
	m_pVS->AttachShader(m_pGraphics->GetImmediateDeviceContext());
	m_pPS->AttachShader(m_pGraphics->GetImmediateDeviceContext());

	//Load mesh from file
	m_pMesh = new Mesh(m_pGraphics, L"models\\sonya.am");
	if(!m_pMesh->IsInitialized())
	{
		AELOG_ERROR(m_pLogger, L"Could not load model from disk -- models\\sphere.am");
		m_pKernel->Stop();
	}

	//triangle list
	m_pGraphics->GetImmediateDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//initialize fps notifier process
	m_pFpsNotifier = new Accevo::FpsNotifierProcess();
	m_pFpsNotifier->RegisterNotifications(2.0f, this);
	m_pKernel->GetProcessManager()->AddProcess(m_pFpsNotifier, true);
}

//called when context is about to not be run
void ModelViewContext::Stop()
{
	if(m_pFpsNotifier)
	{
		delete m_pFpsNotifier;
		m_pFpsNotifier = nullptr;
	}
	if(m_pVS)
	{
		delete m_pVS;
		m_pVS = nullptr;
	}
	if(m_pPS)
	{
		delete m_pPS;
		m_pPS = nullptr;
	}
	if(m_pMesh)
	{	
		delete m_pMesh;
		m_pMesh = nullptr;
	}
}

//called before kernel processes are 
void ModelViewContext::PreUpdate(float dt)
{
	if(m_pInput->IsControl(AEINPUT_GAMECTRL_KEYBOARD | AEINPUT_CTRLACTION_DOWN | DIK_LCONTROL))
	{
		dt *= 5.0f;	//accelerate everything by 5 if control is held down
	}

	if(m_pInput->IsControl(AEINPUT_GAMECTRL_KEYBOARD | DIK_W | AEINPUT_CTRLACTION_DOWN))
	{
		viewHeight += dt * 10.0f;
	}
	if(m_pInput->IsControl(AEINPUT_GAMECTRL_KEYBOARD | DIK_S | AEINPUT_CTRLACTION_DOWN))
	{
		viewHeight += dt * -10.0f;
	}
	if(m_pInput->IsControl(AEINPUT_GAMECTRL_KEYBOARD | DIK_UP | AEINPUT_CTRLACTION_DOWN))
	{
		viewDistance += dt * 10.0f;
	}
	if(m_pInput->IsControl(AEINPUT_GAMECTRL_KEYBOARD | DIK_DOWN | AEINPUT_CTRLACTION_DOWN))
	{
		viewDistance += dt * -10.0f;
	}
	if(m_pInput->IsControl(AEINPUT_GAMECTRL_KEYBOARD | DIK_LEFT | AEINPUT_CTRLACTION_DOWN))
	{	
		rotation += (dt * XM_PI) / 4.0f;
	}
	if(m_pInput->IsControl(AEINPUT_GAMECTRL_KEYBOARD | DIK_RIGHT | AEINPUT_CTRLACTION_DOWN))
	{
		rotation += (dt * -XM_PI) / 4.0f;
	}

	world = XMMatrixRotationY(rotation);
}

void ModelViewContext::Update(float dt)
{
	AFLOAT32 clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	m_pGraphics->Clear(clearColor);

	//XNA Math != succint expression of simple matrices
	XMFLOAT4 eye(0.0f, viewHeight, viewDistance, 0.0f);	//eye at origin
	XMFLOAT4 lookat(0.0f, viewHeight, 1.0f, 0.0f);	//looking down positive z
	XMFLOAT4 up(0.0f, 1.0f, 0.0f, 0.0f);	//standard up (Y axis)

	//three matrices...
	XMMATRIX worldViewProj[3] = 
	{
		XMMatrixTranspose(world),	//move back 65 units into scene
		XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&eye), XMLoadFloat4(&lookat), XMLoadFloat4(&up))),
		XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI/3.0f, 16.0f/9.0f, 0.1f, 1000.0f))
	};
	
	XMFLOAT4	color(0.8f, 0.1f, 0.2f, 1.0f);	//color for PS
	CB_Lights cbLights;
	cbLights.ambient = XMFLOAT4(0.85f, 0.85f, 0.85f, 1.0f);
	cbLights.pointPosition = XMFLOAT4(30.0f, 0.0f, -70.0f, 1.0f);
	cbLights.pointColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbLights.pointAttenuation = XMFLOAT4(3.0f, 0.4f, 0.001f, 1.0f);
		
	XMFLOAT4	lightAmbient(0.15f, 0.15f, 0.15f, 1.0f);	//ambient light
	
	//now ready to make some calls
	ID3D11DeviceContext *pDev = m_pGraphics->GetImmediateDeviceContext();
	m_pVS->SetConstantBufferDataByName(pDev, "cbWorldViewProj", (void const *)worldViewProj);
	m_pPS->SetConstantBufferDataByName(pDev, "cbOutColor", (void const *)&color);
	m_pPS->SetConstantBufferDataByName(pDev, "cbLights", (void const *)&cbLights);

	m_pMesh->Draw(pDev);

	m_pGraphics->Present();
}

void ModelViewContext::PostUpdate(float dt)
{

}

//From Accevo::FpsNotifierProcess::FpsListener
void ModelViewContext::NotifyFps(AFLOAT32 fps)
{
	AELOG_INFO(m_pLogger, (format("Current FPS from ModelViewer: %1%")%fps).str().c_str());
}
