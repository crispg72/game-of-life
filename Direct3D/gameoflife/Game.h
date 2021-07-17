//
// Game.h
//

#pragma once

#include "StepTimer.h"

// A basic game implementation that creates a D3D12 device and
// provides a game loop.
class Game
{
public:

    Game() noexcept;
    ~Game();

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void WaitForPreviousFrame();
    void Clear();
    void CreateCubeBuffers(DXGI_FORMAT, UINT, UINT);
    void DrawCubes();
    void Present();

    void CreateDevice();
    void CreateResources();

    void WaitForGpu() noexcept;
    void MoveToNextFrame();
    void GetAdapter(IDXGIAdapter1** ppAdapter);

    void OnDeviceLost();

    int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow);

    // Application state
    HWND                                                m_window;
    int                                                 m_outputWidth;
    int                                                 m_outputHeight;

    // Direct3D Objects
    D3D_FEATURE_LEVEL                                   m_featureLevel;
    static const UINT                                   c_swapBufferCount = 2;
    UINT                                                m_backBufferIndex;
    UINT                                                m_rtvDescriptorSize;
    Microsoft::WRL::ComPtr<ID3D12Device>                m_d3dDevice;
    Microsoft::WRL::ComPtr<IDXGIFactory4>               m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>          m_commandQueue;
    ID3D12RootSignature*                                m_rootSignature;
    ID3D12PipelineState*                                m_pipelineStateObject;
    ID3D12Resource*                                     m_cubeVertexBuffer;
    ID3D12Resource*                                     m_cubeIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        m_rtvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        m_dsvDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        m_dsDescriptorHeap;
    ID3D12DescriptorHeap*                               m_textureDescriptorHeap;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      m_commandAllocators[c_swapBufferCount];
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>   m_commandList;
    Microsoft::WRL::ComPtr<ID3D12Fence>                 m_fences[c_swapBufferCount];
    UINT64                                              m_fenceValues[c_swapBufferCount];
    //Microsoft::WRL::Wrappers::Event                     m_fenceEvent;
    HANDLE                                              m_fenceEvent;

    // Rendering resources
    Microsoft::WRL::ComPtr<IDXGISwapChain3>             m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource>              m_renderTargets[c_swapBufferCount];
    Microsoft::WRL::ComPtr<ID3D12Resource>              m_depthStencil;
    Microsoft::WRL::ComPtr<ID3D12Resource>              m_constantBufferUploadHeaps[c_swapBufferCount];
    UINT8*                                              m_cbvGPUAddress[c_swapBufferCount];

    int                                                 m_numCubeIndices;

    D3D12_VERTEX_BUFFER_VIEW                            m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW                             m_indexBufferView;



    // Game state
    DX::StepTimer                                       m_timer;
};
