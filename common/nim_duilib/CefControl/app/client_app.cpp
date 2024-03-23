#include "stdafx.h"
#include "client_app.h"
#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"
#include "../manager/cef_manager.h"
#include "../util/util.h"

namespace nim_cef
{

    ClientApp::ClientApp()
    {
        last_node_is_editable_ = false;
        remote_debugging_port_ = 0;
        renderer_startup_dialog_ = false;
    }

    void ClientApp::SetRemoteDebugPort(int port)
    {
        remote_debugging_port_ = port;
    }
    void ClientApp::SetRenderStartupDialog(bool b)
    {
        renderer_startup_dialog_ = b;
    }
//////////////////////////////////////////////////////////////////////////////////////////
// CefApp methods.
    void ClientApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
    {
        // Pass additional command-line flags to the browser process.
        if (process_type.empty())
        {
            command_line->AppendSwitchWithValue("ppapi-flash-version", "20.0.0.228");
            command_line->AppendSwitchWithValue("ppapi-flash-path", "PepperFlash\\pepflashplayer.dll");

            //同一个域下的使用同一个渲染进程
            command_line->AppendSwitch("process-per-site");
            command_line->AppendSwitch("disable-gpu");
            command_line->AppendSwitch("disable-gpu-compositing");
            //command_line->AppendSwitchWithValue("proxy-server", "SOCKS5://127.0.0.1:1080");

            // 开启离屏渲染
            if (CefManager::GetInstance()->IsEnableOffsetRender())
            {
                command_line->AppendSwitch("disable-surfaces");
                command_line->AppendSwitch("enable-begin-frame-scheduling");
            }

            if (remote_debugging_port_ != 0)
            {
                char buf[64] = { 0 };
                _itoa(remote_debugging_port_, buf, 10);
                command_line->AppendSwitchWithValue("remote-debugging-port", buf);
            }

            if (renderer_startup_dialog_ && !command_line->HasSwitch("renderer-startup-dialog"))
                command_line->AppendSwitch("renderer-startup-dialog");
        }
    }

//////////////////////////////////////////////////////////////////////////////////////////
    void ClientApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
    {
        // Default schemes that support cookies.
        cookieable_schemes_.push_back("http");
        cookieable_schemes_.push_back("https");
    }

}