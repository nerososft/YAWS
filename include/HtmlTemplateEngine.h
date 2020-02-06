//
// Created by XingfengYang on 2020/2/6.
//

#ifndef RAFT_HTMLTEMPLATEENGINE_H
#define RAFT_HTMLTEMPLATEENGINE_H

#include <map>
#include <string>

namespace Raft {
    class HtmlTemplateEngine {

    private:
        std::string templateName;
        std::vector<std::map<std::string, std::string>> data;

        /**
         * $for ${data}
         * <p>${node}</p><p>${ip}</p>
         * $end
         */
    public:
        std::string Render();
    };
}

#endif //RAFT_HTMLTEMPLATEENGINE_H
