#pragma once

#include <string>

namespace vgmsplit {

struct Arguments {
	std::string filename;
	int tracknum;
	int tracklen_ms;
	int fade_ms;
	int sample_rate;
};

}
