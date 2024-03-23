#pragma once
#include "include/cef_base.h"
#include "include/cef_app.h"
#include "../app/cef_js_bridge.h"

namespace nim_cef
{
	class CefRenderV8Handler :
		public CefV8Handler
	{
	public:
		CefRenderV8Handler();
		~CefRenderV8Handler();
		void AttachJSBridge(std::shared_ptr<CefJSBridge> js_bridge) { js_bridge_ = js_bridge; }
		virtual void BindJsFunctions(CefRefPtr<CefV8Value>& object)=0;

	protected:
		std::shared_ptr<CefJSBridge> js_bridge_;
	};

}