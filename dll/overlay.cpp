// TODO: fix crashing on resize

#include <mutex>
#include <memory>
#include <dxgi.h>
#include <d3d11.h>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "overlay.h"
#include "tgaamod.h"
#include "mt/mtobject.h"
#include "mt/mtdti.h"
#include "mt/mtproperty.h"

// Per ImGui header:
// Win32 message handler your application need to call.
// - Intentionally commented out in a '#if 0' block to avoid dragging dependencies on <windows.h> from this helper.
// - You should COPY the line below into your .cpp code to forward declare the function and then you can call it.
// - Call from your application's message handler. Keep calling your message handler unless this function returns TRUE.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Overlay *Overlay::s_overlay = nullptr;

Overlay::Overlay(MtObject *skeleton_main) : m_skeyboard_update_state_hook(skeyboard_update_state_replacement), m_smouse_update_state_hook(smouse_update_state_replacement)
{
    s_overlay = this;

    const std::unique_ptr<MtPropertyList> prop_list = MtDTI::from<MtPropertyList>()->new_instance_owned<MtPropertyList>();
    skeleton_main->create_property(prop_list.get());

    MtObject *srender_ptr = prop_list->find_property("mpRender")->get_classref();

    m_render_cs = reinterpret_cast<CRITICAL_SECTION *>(reinterpret_cast<uintptr_t>(srender_ptr) + 0x892f08);

    // mOutputs[0].pSwapChain
    IDXGISwapChain *swapchain = *(reinterpret_cast<IDXGISwapChain **>(reinterpret_cast<uintptr_t>(srender_ptr) + (0x892e98 + 0x10)));

    const uint32_t PRESENT_VTABLE_IDX = 8;
    const uint32_t RESIZE_BUFFERS_VTABLE_IDX = 13;
    void **swapchain_vtable = *reinterpret_cast<void ***>(swapchain);

    spdlog::info("Present is at {}", swapchain_vtable[PRESENT_VTABLE_IDX]);
    spdlog::info("Resize is at {}", swapchain_vtable[RESIZE_BUFFERS_VTABLE_IDX]);

    m_present_hook.setup(reinterpret_cast<uintptr_t>(swapchain_vtable[PRESENT_VTABLE_IDX]), &present_replacement);
    m_resize_buffers_hook.setup(reinterpret_cast<uintptr_t>(swapchain_vtable[RESIZE_BUFFERS_VTABLE_IDX]), &resize_buffers_replacement);
}

Overlay::~Overlay()
{
}

HRESULT Overlay::present_replacement(HRESULT original_present(IDXGISwapChain *, UINT, UINT), IDXGISwapChain *_this, UINT sync_interval, UINT flags)
{
    EnterCriticalSection(s_overlay->m_render_cs);

    if (!s_overlay->m_initialized)
    {
        s_overlay->initialize_imgui(_this);
        s_overlay->m_initialized = true;
    }

    s_overlay->draw();

    LeaveCriticalSection(s_overlay->m_render_cs);

    return original_present(_this, sync_interval, flags);
};

HRESULT Overlay::resize_buffers_replacement(
    HRESULT original_resize_buffers(IDXGISwapChain *, UINT, UINT, UINT, DXGI_FORMAT, UINT),
    IDXGISwapChain *_this,
    UINT buffer_count,
    UINT width,
    UINT height,
    DXGI_FORMAT new_format,
    UINT swapchain_flags)
{

    spdlog::info("ResizeBuffers called!");
    EnterCriticalSection(s_overlay->m_render_cs);

    if (s_overlay->m_render_target_view != nullptr)
    {
        s_overlay->m_device_context->OMSetRenderTargets(0, nullptr, nullptr);
        s_overlay->m_render_target_view->Release();
        s_overlay->m_render_target_view = nullptr;
    };
    LeaveCriticalSection(s_overlay->m_render_cs);

    HRESULT result = original_resize_buffers(_this, buffer_count, width, height, new_format, swapchain_flags);

    EnterCriticalSection(s_overlay->m_render_cs);

    s_overlay->setup_render_target();

    LeaveCriticalSection(s_overlay->m_render_cs);

    return result;
};

LRESULT Overlay::wnd_proc_replacement(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
{
    if (ImGui_ImplWin32_WndProcHandler(window, msg, w_param, l_param) == TRUE)
    {
        return TRUE;
    }

    return CallWindowProc(s_overlay->m_old_wnd_proc, window, msg, w_param, l_param);
}

void Overlay::skeyboard_update_state_replacement(void original_update_state(MtObject *, void *), MtObject *_this, void *state)
{
    if (!ImGui::GetIO().WantCaptureKeyboard)
    {
        original_update_state(_this, state);
    }
}

void Overlay::smouse_update_state_replacement(void original_update_state(MtObject *, void *), MtObject *_this, void *state)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        original_update_state(_this, state);
    }
}

void Overlay::initialize_imgui(IDXGISwapChain *swapchain)
{
    m_swapchain = swapchain;

    DXGI_SWAP_CHAIN_DESC swapchain_desc = {0};
    swapchain->GetDesc(&swapchain_desc);

    swapchain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void **>(&m_device));

    m_device->GetImmediateContext(&m_device_context);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsLight();
    ImGui_ImplWin32_Init(swapchain_desc.OutputWindow);
    ImGui_ImplDX11_Init(m_device, m_device_context);

    setup_render_target();

    m_old_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(swapchain_desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&wnd_proc_replacement)));
}

void Overlay::setup_render_target()
{
    ID3D11Texture2D *back_buffer = nullptr;
    m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&back_buffer));
    m_device->CreateRenderTargetView(back_buffer, NULL, &m_render_target_view);
    back_buffer->Release();
}

void Overlay::draw()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (std::unique_ptr<Module> &module : TGAAMod::s_tgaa_mod->m_modules)
    {
        module->draw();
    };

    ImGui::Render();

    m_device_context->OMSetRenderTargets(1, &m_render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}