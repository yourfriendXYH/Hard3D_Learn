#include "ScriptCommander.h"
#include <fstream>
#include <sstream>
#include "../json.hpp"
#include "TexturePreprocessor.h"

// 根据json配置来执行处理图片的命令
ScriptCommander::ScriptCommander(const std::vector<std::string>& args)
{
	// 扭转法线贴图的方向
	if (args.size() >= 2 && args[0] == "--commands")
	{
		// 
		const auto scriptPath = args[1];
		std::ifstream script(scriptPath);
		if (!script.is_open())
		{
			//throw SCRIPT_ERROR();
		}

		nlohmann::json top;
		script >> top;

		if (!top.is_array())
		{
		}

		if (top.at("enabled"))
		{
			bool abort = false;
			for (const auto& j : top)
			{
				const auto commandName = j.at("command").get<std::string>();
				const auto params = j.at("params");
				if (commandName == "flip-y") // 反转单个法线贴图
				{
					const auto source = params.at("source");
					TexturePreprocessor::FlipYNormalMap(source, params.value("dest", source));
					abort = true;
				}
				else if (commandName == "flip-y-obj") // 批量反转法线贴图
				{
					TexturePreprocessor::FlipYAllNormalMapsInObj(params["source"]);
					abort = true;
				}
				else if (commandName == "validate-nmap") // 验证法线贴图有效性
				{
					TexturePreprocessor::ValidateNormalMap(params["source"], params["min"], params["max"]);
					abort = true;
				}
				else if (commandName == "make-stripes")
				{

					abort = true;
				}
				else
				{

				}
			}
		}
	}
}
