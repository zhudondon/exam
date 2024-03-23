#include "stdafx.h"
#include "cef_control_base.h"
#include "../util/util.h"
#include "base/thread/thread_manager.h"
#include "base/util/string_util.h"
#include "./include/cef_parser.h"
namespace ui
{

    void CefControlBase::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const std::string* buffer, int width, int height)
{
    return;
}

void CefControlBase::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show)
{
    return;
}

void CefControlBase::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect)
{
    return;
}

void CefControlBase::UpdateWindowPos()
{
    this->SetPos(this->GetPos());
}
#define CLIENT_ID_RELOAD_PAGE			1000
#define CLIENT_ID_GOBACK				1001
#define CLIENT_ID_GOFORWARD				1002
#define CLIENT_ID_SHOW_DEVTOOLS			1003
void CefControlBase::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
    CefRefPtr<CefBrowser> b = GetCefBrowser();
    int a = b->GetIdentifier();
    int c = browser->GetIdentifier();
    if (b.get() && (b->GetIdentifier() == browser->GetIdentifier()))
    {
        model->Clear();
        model->InsertItemAt(0, CLIENT_ID_RELOAD_PAGE, L"刷新(&R)");
        model->InsertItemAt(1, CLIENT_ID_GOBACK, L"后退(&B)");
        model->InsertItemAt(2, CLIENT_ID_GOFORWARD, L"前进(&F)");
        model->SetEnabled(CLIENT_ID_GOBACK, CanGoBack());
        model->SetEnabled(CLIENT_ID_GOFORWARD, CanGoForward());
        if (is_debug_)
        {
            if ((params->GetTypeFlags() & (CM_TYPEFLAG_PAGE | CM_TYPEFLAG_FRAME)) != 0)
            {
                model->AddSeparator();

                // Add DevTools items to all context menus.
                model->AddItem(CLIENT_ID_SHOW_DEVTOOLS, L"显示开发工具(&S)");

            }
        }
    }

    if (cb_before_menu_)
        cb_before_menu_(browser, frame, params, model);
}

bool CefControlBase::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, CefContextMenuHandler::EventFlags event_flags)
{
    bool ret = true;
    CefRefPtr<CefBrowser> b = GetCefBrowser();
    if (b.get() && b->GetIdentifier() == browser->GetIdentifier())
    {
        switch (command_id)
        {
            case CLIENT_ID_SHOW_DEVTOOLS:
            {
                AttachDevTools(NULL);
            }
                break;
            case CLIENT_ID_GOBACK:
                GoBack();
                break;
            case CLIENT_ID_GOFORWARD:
                GoForward();
                break;
            case CLIENT_ID_RELOAD_PAGE:
                Refresh();
                break;
            default:
                ret = false;
        }
    }
    if (ret)
        return true;

    if (cb_menu_command_)
        return cb_menu_command_(browser, frame, params, command_id, event_flags);

    return false;
}

void CefControlBase::OnAddressChange(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& url)
{
    if (cb_url_change_)
        cb_url_change_(browser, frame, url);

    if (frame->IsMain())
    {
        auto old_url = url_;
        url_ = frame->GetURL();
        if (cb_main_url_change_ != nullptr && GetMainURL(old_url).compare(GetMainURL(url_)) != 0)
        {
            cb_main_url_change_(browser, frame, old_url, url_);
        }
    }
}

void CefControlBase::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    if (cb_title_change_)
        cb_title_change_(browser, title);
}

void CefControlBase::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    if (cb_fullscreen_mode_change_)
        cb_fullscreen_mode_change_(browser, fullscreen);
}
void CefControlBase::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward)
{
    if (cb_loadstate_change_)
        cb_loadstate_change_(browser, isLoading, canGoBack, canGoForward);
}

void CefControlBase::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType transition_type)
{
    if (cb_load_start_)
        cb_load_start_(browser, frame, transition_type);
}

void CefControlBase::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    if (cb_load_end_)
        cb_load_end_(browser, frame, httpStatusCode);
}

void CefControlBase::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
    if (cb_load_error_)
        cb_load_error_(browser, frame, errorCode, errorText, failedUrl);
    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;
    LoadErrorPage(frame, failedUrl.ToString(), errorCode, errorText);
}

bool CefControlBase::OnBeforePopup(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   const CefString& target_url,
                                   const CefString& target_frame_name,
                                   CefLifeSpanHandler::WindowOpenDisposition target_disposition,
                                   bool user_gesture,
                                   const CefPopupFeatures& popupFeatures,
                                   CefWindowInfo& windowInfo,
                                   CefRefPtr<CefClient>& client,
                                   CefBrowserSettings& settings,
                                   bool* no_javascript_access)
{
    if (cb_link_click_ && !target_url.empty())
        return cb_link_click_(browser, frame, target_url);

    return false;
}

bool CefControlBase::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    if (cb_after_created_)
        cb_after_created_(browser);

    return false;
}

void CefControlBase::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    if (cb_before_close_)
        cb_before_close_(browser);
}

bool CefControlBase::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
    bool result = false;

    if (cb_before_browser_)
        result = cb_before_browser_(browser, frame, request, is_redirect);

    return result;
}

void CefControlBase::OnProtocolExecution(CefRefPtr<CefBrowser> browser, const CefString& url, bool& allow_os_execution)
{
    if (cb_protocol_execution_)
        cb_protocol_execution_(browser, url, allow_os_execution);
}

CefRequestHandler::ReturnValue CefControlBase::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
    if (cb_before_resource_load_)
        return cb_before_resource_load_(browser, frame, request, false);

    return RV_CONTINUE;
}

void CefControlBase::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser, CefRequestHandler::TerminationStatus status)
{
    return;
}

void CefControlBase::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString& suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
{
    if (cb_before_download_)
        cb_before_download_(browser, download_item, suggested_name, callback);
}

void CefControlBase::OnDownloadUpdated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, CefRefPtr<CefDownloadItemCallback> callback)
{
    if (cb_download_updated_)
        cb_download_updated_(browser, download_item, callback);
}

bool CefControlBase::OnFileDialog(CefRefPtr<CefBrowser> browser, CefDialogHandler::FileDialogMode mode, const CefString& title, const CefString& default_file_path, const std::vector<CefString>& accept_filters, int selected_accept_filter, CefRefPtr<CefFileDialogCallback> callback)
{
    if (cb_file_dialog_)
        return cb_file_dialog_(browser, mode, title, default_file_path, accept_filters, selected_accept_filter, callback);
    else
        return false;
}

//bool CefControlBase::OnExecuteCppFunc(const CefString& function_name, const CefString& params, int js_callback_id, CefRefPtr<CefBrowser> browser)
//{
//	if (js_bridge_.get())
//	{
//		js_callback_thread_id_ = nbase::FrameworkThread::GetManagedThreadId();
//		return js_bridge_->ExecuteCppFunc(function_name, params, js_callback_id, browser);
//	}
//
//	return false;
//}

bool CefControlBase::OnExecuteCppFunc(const CefString& function_name, const CefRefPtr<CefListValue>& argList, int js_callback_id, CefRefPtr<CefBrowser> browser)
{
    if (js_bridge_.get())
    {
        js_callback_thread_id_ = nbase::FrameworkThread::GetManagedThreadId();
        return js_bridge_->ExecuteCppFunc(function_name, argList, js_callback_id, browser);
    }

    return false;
}
bool CefControlBase::OnExecuteCppCallbackFunc(int cpp_callback_id, const CefString& json_string)
{
    if (js_bridge_.get())
    {
        if (js_callback_thread_id_ != -1)
        {
            nbase::ThreadManager::PostTask(js_callback_thread_id_, [this, cpp_callback_id, json_string]
            {
                js_bridge_->ExecuteCppCallbackFunc(cpp_callback_id, json_string);
            });
        }
        else
        {
            return js_bridge_->ExecuteCppCallbackFunc(cpp_callback_id, json_string);
        }

    }

    return false;
}


std::string CefControlBase::GetDataURI(const std::string& data, const std::string& mime_type) {
    return "data:" + mime_type + ";base64," +
           CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
                   .ToString();
}

std::string CefControlBase::GetErrorString(cef_errorcode_t code) {
    // Case condition that returns |code| as a string.
#define CASE(code) \
  case code:       \
    return #code

    switch (code) {
        CASE(ERR_NONE);
        CASE(ERR_FAILED);
        CASE(ERR_ABORTED);
        CASE(ERR_INVALID_ARGUMENT);
        CASE(ERR_INVALID_HANDLE);
        CASE(ERR_FILE_NOT_FOUND);
        CASE(ERR_TIMED_OUT);
        CASE(ERR_FILE_TOO_BIG);
        CASE(ERR_UNEXPECTED);
        CASE(ERR_ACCESS_DENIED);
        CASE(ERR_NOT_IMPLEMENTED);
        CASE(ERR_CONNECTION_CLOSED);
        CASE(ERR_CONNECTION_RESET);
        CASE(ERR_CONNECTION_REFUSED);
        CASE(ERR_CONNECTION_ABORTED);
        CASE(ERR_CONNECTION_FAILED);
        CASE(ERR_NAME_NOT_RESOLVED);
        CASE(ERR_INTERNET_DISCONNECTED);
        CASE(ERR_SSL_PROTOCOL_ERROR);
        CASE(ERR_ADDRESS_INVALID);
        CASE(ERR_ADDRESS_UNREACHABLE);
        CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED);
        CASE(ERR_TUNNEL_CONNECTION_FAILED);
        CASE(ERR_NO_SSL_VERSIONS_ENABLED);
        CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH);
        CASE(ERR_SSL_RENEGOTIATION_REQUESTED);
        CASE(ERR_CERT_COMMON_NAME_INVALID);
        CASE(ERR_CERT_DATE_INVALID);
        CASE(ERR_CERT_AUTHORITY_INVALID);
        CASE(ERR_CERT_CONTAINS_ERRORS);
        CASE(ERR_CERT_NO_REVOCATION_MECHANISM);
        CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION);
        CASE(ERR_CERT_REVOKED);
        CASE(ERR_CERT_INVALID);
        CASE(ERR_CERT_END);
        CASE(ERR_INVALID_URL);
        CASE(ERR_DISALLOWED_URL_SCHEME);
        CASE(ERR_UNKNOWN_URL_SCHEME);
        CASE(ERR_TOO_MANY_REDIRECTS);
        CASE(ERR_UNSAFE_REDIRECT);
        CASE(ERR_UNSAFE_PORT);
        CASE(ERR_INVALID_RESPONSE);
        CASE(ERR_INVALID_CHUNKED_ENCODING);
        CASE(ERR_METHOD_NOT_SUPPORTED);
        CASE(ERR_UNEXPECTED_PROXY_AUTH);
        CASE(ERR_EMPTY_RESPONSE);
        CASE(ERR_RESPONSE_HEADERS_TOO_BIG);
        CASE(ERR_CACHE_MISS);
        CASE(ERR_INSECURE_RESPONSE);
        default:
            return "UNKNOWN";
    }
}

void CefControlBase::LoadErrorPage(CefRefPtr<CefFrame> frame,
                                   const std::string& failed_url,
                                   cef_errorcode_t error_code,
                                   const std::string& other_info) {
    std::stringstream ss;
    ss << "<html><head><title>加载网页失败</title></head>"
          "<body bgcolor=\"white\">"
          "<h3>加载网页失败</h3>"
          "URL: <a href=\""
       << failed_url << "\">" << failed_url
       << "</a><br/>Error: " << GetErrorString(error_code) << " ("
       << error_code << ")";

    if (!other_info.empty())
        ss << "<br/>" << other_info;

    ss << "</body></html>";
    frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}
}