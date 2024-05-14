#pragma once
#include <vector>
#include <string>
#include <optional>
#include "../ui.hpp"
#include "../workerthread.hpp"

class Download : public ui::State {
public:
    Download();
    ~Download() override;
    std::optional<ui::States> HandleEvent() override;
    void OnStateEnter(App *app) override;
    void OnStateExit(App *app) override;
    void RenderLoop() override;

    void SetString(const std::string& str);
private:
    std::string m_message;
    bool m_rebootrequired;
    bool m_fullreboot;
    LightLock m_lock;
    /* UI */
    C2D_TextBuf m_textbuf;
    C2D_Text m_text;
    WorkerThread<Download&, App*> worker;
};