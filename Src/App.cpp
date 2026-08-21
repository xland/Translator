#include "pch.h"
#include "App.h"
#include "Setting.h"
#include "Tray.h"
#include "Update.h"
#include "./Win/WinSetting.h"

std::unique_ptr<App> app;


App::~App()
{
}

void App::init()
{
    auto ptr = new App();
    app.reset(ptr);
}

void App::dispose()
{
    WinSetting::dispose();
    Setting::dispose();
    app.reset();
}

App* App::get()
{
    return app.get();
}


App::App()
{
    Ling::init();
    auto app = Ling::App::get();
    app->initArgs();
    Ling::D2D::addFonts({ L"icon.ttf" });
    Setting::init();
    bool flag = app->refuseSecondInstance();
    if (flag) return;
    Tray::init();
    Update::checkLater();
}
