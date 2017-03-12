#ifndef DX11ViewportRenderer_h_
#define DX11ViewportRenderer_h_

#include <maya/MImage.h>
#include <maya/MViewportRenderer.h>
#include <maya/MDagPath.h>
#include <maya/MObjectHandle.h>
#include <maya/MMessage.h> // For monitoring geometry list nodes
#include <maya/MStringArray.h>
#include <maya/MMatrix.h>
#include <list>

// Resources
#include "DX11ResourceManager.h"

//#define DX11_SUPPORTED
#if defined(DX11_SUPPORTED)
	#define WIN32_LEAN_AND_MEAN
	#include <d3d11.h>
	// for VS 2012, Win8 SDK includes DX sdk with some headers removed
	#if _MSC_VER >= 1700
		#include <dxgi.h>
	#else
		#include <d3dx11.h>
	#endif
	#include <DirectXMath.h>
	using namespace DirectX;

	//VR
	#include "openvr.h"
	using namespace vr;
#endif

#if defined(DX11_SUPPORTED)
	struct FixedFunctionVertex
	{
		XMFLOAT3	fPosition;
		XMFLOAT3	fNormal;
		XMFLOAT2	fUV;
	};
	struct FixedFunctionConstants
	{
		XMMATRIX	wvIT;
		XMMATRIX	wvp;
		XMMATRIX	wv;
		
		XMFLOAT4	lightDir;
		XMFLOAT4	lightColor;
		XMFLOAT4	ambientLight;
		XMFLOAT4	diffuseMaterial;
		XMFLOAT4	specularColor;
		float		diffuseCoeff;
		float		shininess;
		float		transparency;
	};
#endif

class MBoundingBox;
class MDagPath;
class MObject;

#pragma warning (disable:4324)
#pragma warning (disable:4239)
#pragma warning (disable:4701)

//
// Sample plugin viewport renderer using the Direct3D api.
//
class DX11ViewportRenderer : public MViewportRenderer
{
public:
	DX11ViewportRenderer();
	virtual ~DX11ViewportRenderer();

	// Required virtual overrides from MViewportRenderer
	//
	virtual	MStatus	initialize();
	virtual	MStatus	uninitialize();
	virtual MStatus	render( const MRenderingInfo &renderInfo );
	virtual bool	nativelySupports( MViewportRenderer::RenderingAPI api, 
										  float version );
	virtual bool	override( MViewportRenderer::RenderingOverride override );

	virtual unsigned int	overrideThenStandardExclusion() const;

protected:

	RenderingAPI	m_API;		// Rendering API
	float			m_Version;	// Direct3D version number as float.

	// Last / current render dimensions
	unsigned int			m_renderWidth;
	unsigned int			m_renderHeight;

#if defined(DX11_SUPPORTED)
public:
	bool					buildRenderTargets(unsigned int width, unsigned int height);

	bool					translateCamera( const MRenderingInfo &renderInfo );
	bool					setupMatrices( const MRenderingInfo &renderInfo );

	// Main entry point to render
	bool					renderToTarget( const MRenderingInfo &renderInfo );

	// Regular scene drawing routines
	bool					drawScene( const MRenderingInfo &renderInfo );
	bool					drawBounds( const MMatrix &matrix, const MBoundingBox &box, float color[3] );
	bool					drawSurface( const MDagPath &dagPath, bool active, bool templated);

	// Readback to system memory from target
	bool					readFromTargetToSystemMemory();

	// Resource management
	const DX11ResourceManager&	resourceManager() const { return m_resourceManager; }
	void					clearResources(bool onlyInvalidItems, bool clearShaders);

	bool					createBoundsBuffers();
	bool					createRasterizerStates();

protected:
	// Basics to setup D3D
	//
	HWND					m_hWnd;			// Handle to window
	ID3D11Device*			m_pD3DDevice;	// Reference to a device 
	ID3D11DeviceContext*	m_pD3DDeviceCtx;

	// Rasterizer states
	//
	ID3D11RasterizerState*	m_pNormalRS;
	ID3D11RasterizerState*	m_pWireframeRS;

	// Off screen render targets
	//
	bool					m_wantFloatingPointTargets; // Do we want floating point render targets (intermediates)
	DXGI_FORMAT				m_intermediateTargetFormat; // Format used for intermediate render targets.
	DXGI_FORMAT				m_outputTargetFormat;  // Output color buffer format.
	ID3D11Texture2D*		m_pTextureInterm;	// Intermediate texture render target
	ID3D11RenderTargetView* m_pTextureIntermView;
	ID3D11Texture2D*		m_pTextureOutput;	// Output texture render target
	ID3D11RenderTargetView* m_pTextureOutputView;

	// Offscreen depth / stencil. We create one for general purpose usage
	DXGI_FORMAT				m_depthStencilFormat; // Depth+stencil format. Same for all targets.
	bool					m_requireDepthStencilReadback;
	ID3D11Texture2D*		m_pDepthStencil;
	ID3D11DepthStencilView*	m_pDepthStencilView;

	ID3D11Texture2D*		m_pTextureReadBack; // Texture for readback.

	// Camera and model matrices
	//
    XMMATRIX				m_matWorld;		// Object to world matrix

	// System memory for colour buffer readback
	MImage					m_readBackBuffer;

	struct BoundsVertex
	{
		XMFLOAT3	fPosition;
	};
	struct BoundsConstants
	{
		XMMATRIX	fWVP;
		XMFLOAT3	fDiffuseMaterial;
	};
	ID3D11Buffer*			m_pBoundsVertexBuffer;
	ID3D11Buffer*			m_pBoundsIndexBuffer;
	ID3D11Buffer*			m_pBoundsConstantBuffer;

	ID3D11Buffer*			m_pFixedFunctionConstantBuffer;

	// D3D Resources. Geometry, textures, lights, shaders etc.
	//
	DX11ResourceManager		m_resourceManager;

	// Temporaries
	XMMATRIX				m_currentViewMatrix;
	MMatrix					mm_currentViewMatrix;
	XMMATRIX				m_currentProjectionMatrix;

	//VR Resources
	IVRSystem* m_hmd;
#endif
};

#endif /* DX11ViewportRenderer_h_ */

//-
// ==========================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

