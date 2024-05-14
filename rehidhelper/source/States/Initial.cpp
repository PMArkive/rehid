#include "Initial.hpp"
#include "../app.hpp"
#include "../Utils/Misc.hpp"

Initial::Initial() {
    LightLock_Init(&m_lock);
}

Initial::~Initial() {
}

void Initial::OnStateEnter(App *app) {
    m_textbuf = C2D_TextBufNew(1000);
    std::string str = "Initial State";
    SetString(str);
    m_broken = false;
    worker.CreateThread([](Initial& initial, App *app) -> void{
        std::string s = "Getting Title List...";
        initial.SetString(s);
        auto& titlemanager = app->GetTitleManager();
        titlemanager.PopulateTitleList();
        titlemanager.FilterOutTWLAndUpdate();
        auto& titles = titlemanager.GetFilteredTitles();
        titlemanager.PopulateIcons(titles.data(), titles.size());
        std::string str = "Checking if Internet is connected.";

        if (app->IsConnected()) {
            str = "Connected.";
            initial.SetString(str);
        }
        str = "Loading Data..";
        initial.SetString(str);
    }, *this, app);
}

void Initial::OnStateExit(App *app) {
    /* This could occur on home menu press */
    while (!worker.IsDone()) {
        svcSleepThread(0.05e+9);
    }

    (void)app;
    C2D_TextBufDelete(m_textbuf);
}

std::optional<ui::States> Initial::HandleEvent() {
    if (worker.IsDone() && !m_broken)
        return ui::States::MainMenu;

    return {};
}

void Initial::RenderLoop() {
    auto top = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Top);
    auto bottom = ui::g_RenderTarget.GetRenderTarget(ui::Screen::Bottom);

    /* Top */
    C2D_SceneBegin(top);
    C2D_TargetClear(top, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Top);
    ui::Elements::IconAndText::GetInstance().DrawIconAndTextInMiddle(ui::Screen::Top, true);

    /* Bottom */
    C2D_SceneBegin(bottom);
    C2D_TargetClear(bottom, C2D_Color32(0xEC, 0xF0, 0xF1, 0xFF));
    ui::Elements::DrawBars(ui::Screen::Bottom);
    auto height = 0.0f, width = 0.0f;
    C2D_TextGetDimensions(&m_text, 0.5f, 0.5f, &width, &height);
    auto y = (ui::Dimensions::GetHeight() - height) / 2;
    LightLock_Lock(&m_lock);
    C2D_DrawText(&m_text, C2D_AlignCenter, 160.0f, y, 1.0f, 0.5f, 0.5f);
    LightLock_Unlock(&m_lock);
}

void Initial::SetString(const std::string &str) {
    LightLock_Lock(&m_lock);
    m_message = str;
    C2D_TextParse(&m_text, m_textbuf, m_message.c_str());
    C2D_TextOptimize(&m_text);
    LightLock_Unlock(&m_lock);
}