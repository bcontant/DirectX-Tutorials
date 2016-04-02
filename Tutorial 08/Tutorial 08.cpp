#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "vertex_shader.h"
#include "pixel_shader.h"

int windowWidth = 640;
int windowHeight = 480;

ID3D11Device* g_pD3D11Device = NULL;
ID3D11DeviceContext* g_pD3D11DeviceContext = NULL;

IDXGISwapChain* g_pSwapChain = NULL;
ID3D11RenderTargetView* g_pRenderTarget = NULL;
ID3D11BlendState* g_pBlendState = NULL;
ID3D11RasterizerState* g_pRasterizerState = NULL;

ID3D11VertexShader* g_pVertexShader = NULL;
ID3D11PixelShader* g_pPixelShader = NULL;

ID3D11Buffer* g_pVertexBuffer = NULL;
ID3D11Buffer* g_pIndexBuffer = NULL;
ID3D11Buffer* g_pWVPConstantBuffer = NULL;
ID3D11InputLayout* g_pInputLayout = NULL;

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

Vertex quad[] =
{
	//Top Left
	DirectX::XMFLOAT3(-0.5f, 0.5f, 0.f),
	DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f),

	//Top Right
	DirectX::XMFLOAT3(0.5f, 0.5f, 0.f),
	DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f),

	//Bottom Left
	DirectX::XMFLOAT3(-0.5f, -0.5f, 0.f),
	DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f),

	//Bottom Right
	DirectX::XMFLOAT3(0.5f, -0.5f, 0.f),
	DirectX::XMFLOAT4(1.f, 0.f, 1.f, 1.f),
};

unsigned int indexBuffer[] =
{
	0, 1, 3,
	0, 2, 3,
};

struct CB_WVP
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	BOOL handled = FALSE;
	
	switch (message)
	{
	case WM_SIZE:
	{
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);

		if (g_pD3D11Device != NULL)
		{
			g_pRenderTarget->Release();
			HRESULT hr = g_pSwapChain->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			ID3D11Texture2D* pBackBuffer;
			hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

			hr = g_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTarget);
			pBackBuffer->Release();

			g_pD3D11DeviceContext->OMSetRenderTargets(1, &g_pRenderTarget, NULL);

			D3D11_VIEWPORT viewport = { };
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;
			viewport.Width = (FLOAT)windowWidth;
			viewport.Height = (FLOAT)windowHeight;

			g_pD3D11DeviceContext->RSSetViewports(1, &viewport);
		}

		result = 1;
		handled = true;
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		handled = TRUE;
		break;
	}

	if (!handled)
		result = DefWindowProc(hwnd, message, wParam, lParam);

	return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = hInstance;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	wcex.lpszClassName = L"DirectXTutorial";

	RegisterClassEx(&wcex);

	RECT rc = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND hwnd = CreateWindow(
		L"DirectXTutorial", 
		L"DirectX Tutorial 08", 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		rc.right - rc.left, 
		rc.bottom - rc.top, 
		NULL, 
		NULL, 
		hInstance, 
		NULL);

	ShowWindow(hwnd, SW_SHOWNORMAL);
	UpdateWindow(hwnd);

	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = windowWidth;
	swapChainDesc.BufferDesc.Height = windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, 0, D3D11_SDK_VERSION, &swapChainDesc, &g_pSwapChain, &g_pD3D11Device, NULL, &g_pD3D11DeviceContext);

	ID3D11Texture2D* pBackBuffer;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);

	hr = g_pD3D11Device->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTarget);
	pBackBuffer->Release();

	D3D11_BLEND_DESC BlendState = {};
	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pD3D11Device->CreateBlendState(&BlendState, &g_pBlendState);

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = g_pD3D11Device->CreateRasterizerState(&rasterizerDesc, &g_pRasterizerState);

	hr = g_pD3D11Device->CreateVertexShader(g_VertexShader, sizeof(g_VertexShader), NULL, &g_pVertexShader);
	hr = g_pD3D11Device->CreatePixelShader(g_PixelShader, sizeof(g_PixelShader), NULL, &g_pPixelShader);

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = 4 * sizeof(Vertex);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferInitData;
	vertexBufferInitData.pSysMem = quad;
	vertexBufferInitData.SysMemPitch = 0;
	vertexBufferInitData.SysMemSlicePitch = 0;

	hr = g_pD3D11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferInitData, &g_pVertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = 6 * sizeof(unsigned int);
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexBufferInitData;
	indexBufferInitData.pSysMem = indexBuffer;
	indexBufferInitData.SysMemPitch = 0;
	indexBufferInitData.SysMemSlicePitch = 0;

	hr = g_pD3D11Device->CreateBuffer(&indexBufferDesc, &indexBufferInitData, &g_pIndexBuffer);
	
	CB_WVP WVP;
	WVP.world = DirectX::XMMatrixIdentity();
	WVP.view = DirectX::XMMatrixIdentity();
	WVP.proj = DirectX::XMMatrixIdentity();

	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.ByteWidth = sizeof(CB_WVP);
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA constantBufferInitData;
	constantBufferInitData.pSysMem = &WVP;
	constantBufferInitData.SysMemPitch = 0;
	constantBufferInitData.SysMemSlicePitch = 0;

	hr = g_pD3D11Device->CreateBuffer(&constantBufferDesc, &constantBufferInitData, &g_pWVPConstantBuffer);

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = g_pD3D11Device->CreateInputLayout(inputElementDescs, 2, g_VertexShader, sizeof(g_VertexShader), &g_pInputLayout);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pD3D11DeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pD3D11DeviceContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pD3D11DeviceContext->IASetInputLayout(g_pInputLayout);

	g_pD3D11DeviceContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &g_pWVPConstantBuffer);

    D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)windowWidth;
	viewport.Height = (FLOAT)windowHeight;

	g_pD3D11DeviceContext->RSSetViewports(1, &viewport);
	g_pD3D11DeviceContext->RSSetState(g_pRasterizerState);

	g_pD3D11DeviceContext->PSSetShader(g_pPixelShader, 0, 0);

	g_pD3D11DeviceContext->OMSetRenderTargets(1, &g_pRenderTarget, NULL);
	g_pD3D11DeviceContext->OMSetBlendState(g_pBlendState, NULL, 0xffffffff);
	
	MSG msg;
	float angle = 0.0f;
	do
	{
		if (PeekMessage(&msg, NULL, 0, 0, TRUE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			hr = g_pD3D11DeviceContext->Map(g_pWVPConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			CB_WVP* dataPtr = (CB_WVP*)mappedResource.pData;
			dataPtr->world = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(angle));
			dataPtr->view = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.f, 0.f, -1.f, 0.f), DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f), DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f)));
			//dataPtr->proj = DirectX::XMMatrixOrthographicLH(2.f, 2.f, 0.f, 10.f);
			dataPtr->proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 2.f, float(windowWidth) / windowHeight, 0.1f, 1000.f));

			g_pD3D11DeviceContext->Unmap(g_pWVPConstantBuffer, 0);

			DirectX::XMFLOAT4 clearColor(0.f, 0.2f, 0.4f, 0.f);
			g_pD3D11DeviceContext->ClearRenderTargetView(g_pRenderTarget, (CONST FLOAT*)&clearColor);
			g_pD3D11DeviceContext->DrawIndexed(6, 0, 0);
			g_pSwapChain->Present(1, 0);

			angle += DirectX::XM_2PI / 60.f;
		}
	} while (msg.message != WM_QUIT);

	g_pRasterizerState->Release();
	g_pBlendState->Release();
	g_pWVPConstantBuffer->Release();
	g_pIndexBuffer->Release();
	g_pVertexBuffer->Release();
	g_pInputLayout->Release();
	g_pVertexShader->Release();
	g_pPixelShader->Release();
	g_pRenderTarget->Release();
	g_pSwapChain->Release();
	g_pD3D11DeviceContext->Release();
	g_pD3D11Device->Release();

	return 0;
}