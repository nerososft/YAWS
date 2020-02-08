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
        templateEngine.SetBlock("configs").Repeat(5);

        for (int i = 0; i < 5; i++) {
            templateEngine.SetBlock("configs")[i].Set("status", "OK");
            templateEngine.SetBlock("configs")[i].Set("role", "Follower");
            templateEngine.SetBlock("configs")[i].Set("id", "Follower");
            templateEngine.SetBlock("configs")[i].Set("host", "Follower");
            templateEngine.SetBlock("configs")[i].Set("port", "Follower");
            templateEngine.SetBlock("configs")[i].Set("term", "Follower");
            templateEngine.SetBlock("configs")[i].Set("commissionId", "Follower");
        }

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
}
