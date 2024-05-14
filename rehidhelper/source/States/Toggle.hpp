#pragma once
#include <vector>
#include <string>
#include <optional>
#include "../ui.hpp"
#include "../workerthread.hpp"

class Toggle : public ui::State {
public:
    Toggle();
    ~Toggle() override;
    std::optional<ui::States> HandleEvent() override;
    void OnStateEnter(App *app) override;
    void OnStateExit(App *app) override;
    void RenderLoop() override;

    void SetString(const std::string& str);

private:
    std::string m_message;
    /* UI */
    C2D_TextBuf m_textbuf;
    C2D_Text m_text;
};