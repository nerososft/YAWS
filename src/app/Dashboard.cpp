//
// Created by XingfengYang on 2020/2/8.
//

#include "../../include/app/Dashboard.h"
#include "../../include/templateEngine/Loader.h"
#include "../../include/templateEngine/TemplateEngine.h"

Http::HandlerResponse App::Dashboard::Home(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);

    try {
        templateEngine.Load("www/html/dashboard.html");
        templateEngine.Set("title", "DASHBOARD");
        templateEngine.Set("clientId", "5");

        templateEngine.Set("dashboardClass", "active");
        templateEngine.Set("configClass", "");
        templateEngine.Set("aboutClass", "");
        templateEngine.Set("logEntryClass", "");

        std::vector<NodeModel> nodeModels;
        nodeModels.push_back({"statusGreen", "OK", "Follower", "12", "127.0.0.1", "8890", "5", "3453"});
        nodeModels.push_back({"statusYellow", "Connecting...", "Follower", "1", "127.0.0.1", "8898", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "23", "127.0.0.1", "8891", "5", "3453"});
        nodeModels.push_back({"statusYellow", "Connecting...", "Follower", "1", "127.0.0.1", "8892", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "6", "127.0.0.1", "8893", "5", "3453"});
        nodeModels.push_back({"statusRed", "LostConnection", "Follower", "1", "127.0.0.1", "8894", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "7", "127.0.0.1", "8895", "5", "3453"});
        nodeModels.push_back({"statusGreen statusSelf", "OK", "Leader [ self ]", "5", "127.0.0.1", "8898", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "56", "127.0.0.1", "8897", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "88", "127.0.0.1", "8896", "5", "3453"});
        nodeModels.push_back({"statusYellow", "Connecting...", "Follower", "1", "127.0.0.1", "8889", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "82", "127.0.0.1", "8888", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "87", "127.0.0.1", "8887", "5", "3453"});
        nodeModels.push_back({"statusGreen", "OK", "Follower", "86", "127.0.0.1", "8886", "5", "3453"});

        templateEngine.SetBlock("configs").Repeat(nodeModels.size());
        for (int i = 0; i < nodeModels.size(); i++) {
            templateEngine.SetBlock("configs")[i].Set("statusClass", nodeModels[i].statusClass);
            templateEngine.SetBlock("configs")[i].Set("status", nodeModels[i].status);
            templateEngine.SetBlock("configs")[i].Set("role", nodeModels[i].role);
            templateEngine.SetBlock("configs")[i].Set("id", nodeModels[i].id);
            templateEngine.SetBlock("configs")[i].Set("host", nodeModels[i].host);
            templateEngine.SetBlock("configs")[i].Set("port", nodeModels[i].port);
            templateEngine.SetBlock("configs")[i].Set("term", nodeModels[i].term);
            templateEngine.SetBlock("configs")[i].Set("commissionId", nodeModels[i].commissionId);
        }

        return {Http::OK, templateEngine.RenderToText()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}

Http::HandlerResponse App::Dashboard::About(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);

    try {
        templateEngine.Load("www/html/about.html");
        templateEngine.Set("title", "ABOUT");
        templateEngine.Set("clientId", "5");

        templateEngine.Set("dashboardClass", "");
        templateEngine.Set("configClass", "");
        templateEngine.Set("aboutClass", "active");
        templateEngine.Set("logEntryClass", "");

        return {Http::OK, templateEngine.RenderToText()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}

Http::HandlerResponse App::Dashboard::Config(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);

    try {
        templateEngine.Load("www/html/config.html");
        templateEngine.Set("title", "CONFIG");
        templateEngine.Set("clientId", "5");

        templateEngine.Set("dashboardClass", "");
        templateEngine.Set("configClass", "active");
        templateEngine.Set("aboutClass", "");
        templateEngine.Set("logEntryClass", "");

        return {Http::OK, templateEngine.RenderToText()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}

Http::HandlerResponse App::Dashboard::LogEntry(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);

    try {
        templateEngine.Load("www/html/logEntry.html");
        templateEngine.Set("title", "LOG ENTRY");
        templateEngine.Set("clientId", "5");

        templateEngine.Set("dashboardClass", "");
        templateEngine.Set("configClass", "");
        templateEngine.Set("aboutClass", "");
        templateEngine.Set("logEntryClass", "active");

        return {Http::OK, templateEngine.RenderToText()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}

void App::Dashboard::Init() {
    this->AddRoute({"/dashboard", Http::GET}, std::bind(&Dashboard::Home, this, std::placeholders::_1));
    this->AddRoute({"/about", Http::GET}, std::bind(&Dashboard::About, this, std::placeholders::_1));
    this->AddRoute({"/config", Http::GET}, std::bind(&Dashboard::Config, this, std::placeholders::_1));
    this->AddRoute({"/log-entry", Http::GET}, std::bind(&Dashboard::LogEntry, this, std::placeholders::_1));
}


