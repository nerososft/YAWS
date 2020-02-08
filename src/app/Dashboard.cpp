//
// Created by XingfengYang on 2020/2/8.
//

#include <sstream>
#include "../../include/app/Dashboard.h"
#include "../../include/templateEngine/Loader.h"
#include "../../include/templateEngine/TemplateEngine.h"

Http::HandlerResponse App::Dashboard::Home(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);

    try {
        templateEngine.Load("www/html/dashboard.html");
        templateEngine.Set("dashboardClass", "active");
        templateEngine.Set("configClass", "");
        templateEngine.Set("aboutClass", "");

        char *status[7] = {"OK", "Connecting...", "LostConnection", "OK", "Connecting...", "OK", "OK"};
        char *statusClass[7] = {"statusGreen", "statusYellow", "statusRed", "statusGreen", "statusYellow", "statusGreen", "statusGreen"};
        templateEngine.SetBlock("configs").Repeat(7);
        for (int i = 0; i < 7; i++) {
            templateEngine.SetBlock("configs")[i].Set("statusClass", statusClass[i]);
            templateEngine.SetBlock("configs")[i].Set("status", status[i]);
            templateEngine.SetBlock("configs")[i].Set("role", "Follower");
            templateEngine.SetBlock("configs")[i].Set("id", "3");
            templateEngine.SetBlock("configs")[i].Set("host", "127.0.0.1");
            templateEngine.SetBlock("configs")[i].Set("port", "8898");
            templateEngine.SetBlock("configs")[i].Set("term", "5");
            templateEngine.SetBlock("configs")[i].Set("commissionId", "2343");
        }

        std::stringbuf buf;
        std::ostream sout(&buf);
        templateEngine.Render(sout);
        return {Http::OK, buf.str()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}

Http::HandlerResponse App::Dashboard::About(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);

    try {
        templateEngine.Load("www/html/about.html");
        templateEngine.Set("dashboardClass", "");
        templateEngine.Set("configClass", "");
        templateEngine.Set("aboutClass", "active");

        std::stringbuf buf;
        std::ostream sout(&buf);
        templateEngine.Render(sout);
        return {Http::OK, buf.str()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}

Http::HandlerResponse App::Dashboard::Config(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);

    try {
        templateEngine.Load("www/html/config.html");
        templateEngine.Set("dashboardClass", "");
        templateEngine.Set("configClass", "active");
        templateEngine.Set("aboutClass", "");

        std::stringbuf buf;
        std::ostream sout(&buf);
        templateEngine.Render(sout);
        return {Http::OK, buf.str()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}

void App::Dashboard::Init() {
    this->AddRoute({"/dashboard", Http::GET}, std::bind(&Dashboard::Home, this, std::placeholders::_1));
    this->AddRoute({"/about", Http::GET}, std::bind(&Dashboard::About, this, std::placeholders::_1));
    this->AddRoute({"/config", Http::GET}, std::bind(&Dashboard::Config, this, std::placeholders::_1));
}
