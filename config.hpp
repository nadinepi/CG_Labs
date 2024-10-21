#pragma once

#include "core/various.hpp"

#include <fstream>
#include <string>

namespace config
{
	constexpr unsigned int msaa_rate = 1;
	constexpr unsigned int resolution_x = 1600;
	constexpr unsigned int resolution_y = 900;

	inline std::string shaders_path(std::string const& path)
	{
		std::string const tmp_path = std::string("shaders/") + path;
		std::string const root = std::ifstream(utils::widen(tmp_path)) ? "." : "/home/leviwfrey/LUND/EDAF80/CG_Labs";
		return root + std::string("/") + tmp_path;
	}
	inline std::string resources_path(std::string const& path)
	{
		std::string const tmp_path = std::string("res/") + path;
		std::string const root = std::ifstream(utils::widen(tmp_path)) ? "." : "/home/leviwfrey/LUND/EDAF80/CG_Labs";
		return root + std::string("/") + tmp_path;
	}
}
