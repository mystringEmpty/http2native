#include "NativeServer.h"
#include "ApiEvent.h"

Api_PostEvent api_event{ nullptr };
std::unique_ptr<CivetServer> server;
EventHandler handler;

__pragma(comment(linker, "/EXPORT:StartUp=_StartUp@8"))
extern "C" __declspec(dllexport)
int __stdcall StartUp(int port, Api_PostEvent api) {
	if (!api)return 0;
	api_event = api;
	// 初始化服务器
	mg_init_library(0);

	try {
		const std::string port_option = std::to_string(port);

		std::vector<std::string> mg_options = { "listening_ports", port_option.c_str() };

		server = std::make_unique<CivetServer>(mg_options);

		server->addHandler("/", handler);
		auto ports = server->getListeningPorts();

		if (ports.empty()){
			//启动失败！端口已被使用？
			return 0;
		}
		return ports[0];
	}
	catch (const CivetException& e){
		//启动失败！端口已被使用？
		return 0;
	}
	return 0;
}

__pragma(comment(linker, "/EXPORT:ShutDown=_ShutDown@0"))
extern "C" __declspec(dllexport)
int __stdcall ShutDown() {
	server.reset();
	mg_exit_library();
	return 0;
}