#include "NativeServer.h"
#include "ApiEvent.h"
bool EventHandler::handlePost(CivetServer* server, struct mg_connection* conn)
{
    std::string ret;
    try
    {
        auto data = server->getPostData(conn);
        api_event(data.c_str());
        nlohmann::json j2 = nlohmann::json::object();
        j2["code"] = 0;
        j2["msg"] = "ok";
        ret = j2.dump();
    }
    catch (const std::exception& e)
    {
        nlohmann::json j = nlohmann::json::object();
        j["code"] = -1;
        j["msg"] = e.what();
        ret = j.dump();
    }
    mg_send_http_ok(conn, "application/json", ret.length());
    mg_write(conn, ret.c_str(), ret.length());
    return true;
}
bool EventHandler::handleGet(CivetServer* server, struct mg_connection* conn) {
    std::string html = "I'm listening.";
    mg_send_http_ok(conn, "text/html", html.length());
    mg_write(conn, html.c_str(), html.length());
    return true;
}
