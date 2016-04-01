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

ID3D11VertexShader* g_pVertexShader = NULL;
ID3D11PixelShader* g_pPixelShader = NULL;

ID3D11Buffer* g_pVertexBuffer = NULL;
ID3D11InputLayout* g_pInputLayout = NULL;

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

Vertex triangle[] =
{
	DirectX::XMFLOAT3(0.f, 0.5f, 0.f),
	DirectX::XMFLOAT4(1.f, 0.f, 0.f, 1.f),

	DirectX::XMFLOAT3(0.5f, -0.5f, 0.f),
	DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f),

	DirectX::XMFLOAT3(-0.5f, -0.5f, 0.f),
	DirectX::XMFLOAT4(0.f, 0.f, 1.f, 1.f),
};


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	BOOL handled = FALSE;
	
	switch (message)
	{
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
		L"DirectX Tutorial 05", 
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

	hr = g_pD3D11Device->CreateVertexShader(g_VertexShader, sizeof(g_VertexShader), NULL, &g_pVertexShader);
	hr = g_pD3D11Device->CreatePixelShader(g_PixelShader, sizeof(g_PixelShader), NULL, &g_pPixelShader);

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.ByteWidth = 3 * sizeof(Vertex);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferInitData;
	vertexBufferInitData.pSysMem = triangle;
	vertexBufferInitData.SysMemPitch = 0;
	vertexBufferInitData.SysMemSlicePitch = 0;

	hr = g_pD3D11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferInitData, &g_pVertexBuffer);

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = g_pD3D11Device->CreateInputLayout(inputElementDescs, 2, g_VertexShader, sizeof(g_VertexShader), &g_pInputLayout);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	g_pD3D11DeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pD3D11DeviceContext->IASetInputLayout(g_pInputLayout);

	g_pD3D11DeviceContext->VSSetShader(g_pVertexShader, 0, 0);

    D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)windowWidth;
	viewport.Height = (FLOAT)windowHeight;

	g_pD3D11DeviceContext->RSSetViewports(1, &viewport);
	g_pD3D11DeviceContext->PSSetShader(g_pPixelShader, 0, 0);

	g_pD3D11DeviceContext->OMSetRenderTargets(1, &g_pRenderTarget, NULL);
	g_pD3D11DeviceContext->OMSetBlendState(g_pBlendState, NULL, 0xffffffff);
	
	MSG msg;
	do
	{
		if (PeekMessage(&msg, NULL, 0, 0, TRUE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DirectX::XMFLOAT4 clearColor(0.f, 0.2f, 0.4f, 0.f);
			g_pD3D11DeviceContext->ClearRenderTargetView(g_pRenderTarget, (CONST FLOAT*)&clearColor);
			g_pD3D11DeviceContext->Draw(3, 0);
			g_pSwapChain->Present(0, 0);
		}
	} while (msg.message != WM_QUIT);

	g_pBlendState->Release();
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