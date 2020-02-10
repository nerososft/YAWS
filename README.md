# YaRaft
Yet Another Raft Implement

[What is Raft?](https://raft.github.io/)

![C/C++ CI](https://github.com/nerososft/YaRaft/workflows/C/C++%20CI/badge.svg)

Not just Raft.

## dashboard
get RaftServer status form broswer
```
http://localhost:8899/dashboard
```


# WebApp
## define app
let define a HelloWorldApp:
```c++
namespace App {
    class HelloWorldApp : public WebApplication {
    public:
        void Init();
    public:
        Http::HandlerResponse sayHelloWorld(Http::HttpRequest request);
    };
}
```
## initial
add app in AppConfig::AttachApps:
```c++
void App::AppConfig::AttachApps(std::shared_ptr<Http::HttpServer> httpServer) {
    App::HelloWorldApp helloWorldApp;
    helloWorldApp.SetServer(httpServer);
    helloWorldApp.Init();
}
```
## router
add router in App's Init method:
```c++
void App::HelloWorldApp::Init() {
    this->AddRoute({"/helloworld", Http::GET}, std::bind(&HelloWorldApp::sayHelloWorld, this, std::placeholders::_1));
}
```
## controller
implement sayHelloWorld function:
```c++
Http::HandlerResponse App::HelloWorldApp::sayHelloWorld(Http::HttpRequest request) {
    return {Http::OK,"hello, world!"};
}
```
## template engine
can render html template in controller:

template define: add a html file in /www/html directory,like:

helloworld.html
```html
<html lang="html">
<head>
    <title>HelloWorldApp</title>
</head>
<body>
    <p>{{ helloworld }}</p>
</body>
</html>
```
and use it in controller:
```c++
Http::HandlerResponse App::HelloWorldApp::sayHelloWorld(Http::HttpRequest request) {
    Loader::FileLoader fileLoader;
    Template::TemplateEngine templateEngine(fileLoader);
    try {
        templateEngine.Load("www/html/helloworld.html");
        templateEngine.Set("helloworld", "hello, world!");
     
        return {Http::OK, templateEngine.RenderToText()};
    } catch (std::logic_error error) {
        return {Http::OK, error.what()};
    }
}
```
more complex template pls see the code.
