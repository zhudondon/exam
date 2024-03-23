/** @file client_app.h
  * @brief ����Browser���̵�CefApp�࣬����Cefģ�����������
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/7/19
*/
#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "include/cef_app.h"
#include "../app/cef_js_bridge.h"
#include "cef_render_v8handler.h"
namespace nim_cef
{
class ClientApp : public CefApp,
                  public CefBrowserProcessHandler,
                  public CefRenderProcessHandler
{
public:
	ClientApp();
	void SetRenderJsHandle(CefRefPtr<CefRenderV8Handler>& handle) { js_handler_ = handle;}
	void SetRemoteDebugPort(int port);
	void SetRenderStartupDialog(bool b);
protected:
	// CefApp methods.
	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) OVERRIDE;
	virtual void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) OVERRIDE;
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE{ return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE{ return this; }

	// CefBrowserProcessHandler methods.
	virtual void OnContextInitialized() OVERRIDE;
	virtual void OnBeforeChildProcessLaunch(
		CefRefPtr<CefCommandLine> command_line) OVERRIDE;
	virtual void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;

	// CefRenderProcessHandler methods.
	virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;
	virtual void OnWebKitInitialized() OVERRIDE;
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;
	virtual bool OnBeforeNavigation(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		NavigationType navigation_type,
		bool is_redirect) OVERRIDE;
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
	virtual void OnUncaughtException(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context,
		CefRefPtr<CefV8Exception> exception,
		CefRefPtr<CefV8StackTrace> stackTrace) OVERRIDE;
	virtual void OnFocusedNodeChanged(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node) OVERRIDE;
	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) OVERRIDE;

	
protected:
	std::shared_ptr<CefJSBridge>	render_js_bridge_;
	std::vector<CefString>			cookieable_schemes_;
	CefRefPtr<CefRenderV8Handler>	js_handler_;
	// Schemes that will be registered with the global cookie manager. Used in
	bool							last_node_is_editable_;
	int								remote_debugging_port_;
	bool							renderer_startup_dialog_;
	// both the browser and renderer process.
	IMPLEMENT_REFCOUNTING(ClientApp);
};
}