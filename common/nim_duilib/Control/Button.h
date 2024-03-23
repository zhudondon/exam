#ifndef UI_CONTROL_BUTTON_H_
#define UI_CONTROL_BUTTON_H_

#pragma once

namespace ui
{
    template<typename InheritType = Control>
    class UILIB_API ButtonTemplate : public LabelTemplate<InheritType>
{
    public:
    ButtonTemplate();

    /// 重写父类方法，提供个性化功能，请参考父类声明
    virtual void Activate() override;
    virtual void HandleMessage(EventArgs& event) override;

    /**
     * @brief 绑定鼠标点击处理函数
     * @param[in] callback 要绑定的回调函数
     * @return 无
     */
    void AttachClick(const EventCallback& callback)	{ this->OnEvent[kEventClick] += callback;	}
};

template<typename InheritType>
ButtonTemplate<InheritType>::ButtonTemplate()
{
    this->m_uTextStyle = DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS | DT_NOCLIP | DT_SINGLELINE;
}

template<typename InheritType>
void ButtonTemplate<InheritType>::HandleMessage(EventArgs& event)
{
    if (!this->IsMouseEnabled() && event.Type > kEventMouseBegin && event.Type < kEventMouseEnd) {
        if (this->m_pParent != NULL) this->m_pParent->HandleMessageTemplate(event);
        else __super::HandleMessage(event);
        return;
    }
    if (event.Type == kEventKeyDown)
    {
        if (this->IsKeyboardEnabled()) {
            if (event.chKey == VK_SPACE || event.chKey == VK_RETURN) {
                Activate();
                return;
            }
        }
    }
    if (event.Type == kEventInternalMenu)
    {
        if (this->IsContextMenuUsed()) {
            this->m_pWindow->SendNotify(this, kEventMouseMenu, event.wParam, event.lParam);
        }
        return;
    }

    __super::HandleMessage(event);
}

template<typename InheritType>
void ButtonTemplate<InheritType>::Activate()
{
    if (!this->IsActivatable()) return;
    if (this->m_pWindow != NULL) this->m_pWindow->SendNotify(this, kEventClick);
}

typedef ButtonTemplate<Control> Button;
typedef ButtonTemplate<Box> ButtonBox;

}	// namespace ui

#endif // UI_CONTROL_BUTTON_H_