#pragma once
#include <string>
#include "json.hpp"
#include "CivetServer.h"

class EventHandler : public CivetHandler {
public:
    bool handlePost(CivetServer* server, struct mg_connection* conn);
    bool handleGet(CivetServer* server, struct mg_connection* conn);
};