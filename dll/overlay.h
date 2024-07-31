#pragma once
#include <dxgi.h>
#include <d3d11.h>
#include <mutex>

#include "hook.h"
#include "mt/mtobject.h"

class Overlay
{
public:
    Overlay(MtObject *skeleton_main);
    ~Overlay();

private:
    static HRESULT present_replacement(HRESULT original_present(IDXGISwapChain *, UINT, UINT), IDXGISwapChain *_this, UINT sync_interval, UINT flags);
    RuntimeAddressFunctionHook<0, HRESULT, IDXGISwapChain *, UINT, UINT> m_present_hook;

    static HRESULT resize_buffers_replacement(
        HRESULT original_resize_buffers(IDXGISwapChain *, UINT, UINT, UINT, DXGI_FORMAT, UINT),
        IDXGISwapChain *_this,
        UINT buffer_count,
        UINT width,
        UINT height,
        DXGI_FORMAT new_format,
        UINT swapchain_flags);
    RuntimeAddressFunctionHook<1, HRESULT, IDXGISwapChain *, UINT, UINT, UINT, DXGI_FORMAT, UINT> m_resize_buffers_hook;

    static void skeyboard_update_state_replacement(void original_update_state(MtObject *, void *), MtObject *_this, void *state);
    StaticFunctionHook<0x1403ff230, void, MtObject *, void *> m_skeyboard_update_state_hook;

    static void smouse_update_state_replacement(void original_update_state(MtObject *, void *), MtObject *_this, void *state);
    StaticFunctionHook<0x140400290, void, MtObject *, void *> m_smouse_update_state_hook;

    static LRESULT wnd_proc_replacement(HWND window, UINT msg, WPARAM w_param, LPARAM l_param);

    void initialize_imgui(IDXGISwapChain *swapchain);
    void setup_render_target();
    void draw();

    bool m_initialized = false;
    IDXGISwapChain *m_swapchain = nullptr;
    ID3D11Device *m_device = nullptr;
    ID3D11DeviceContext *m_device_context = nullptr;
    ID3D11RenderTargetView *m_render_target_view = nullptr;

    WNDPROC m_old_wnd_proc = nullptr;

    static Overlay *s_overlay;
};