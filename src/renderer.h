#pragma once

#include "main.h"

#include <string>

class gme_t;

namespace vgmsplit::renderer {
#ifndef renderer_INTERNAL
#define renderer_INTERNAL private
#endif

using RenderError = int;

RenderError write_master(Arguments const& args);

RenderError write_channel(Arguments const& args, int channel);

class Renderer {
renderer_INTERNAL:
	Arguments _args;

	// Computed state
	gme_t* _emu;

public:
	Renderer(Arguments args) /*throws*/;

	~Renderer();

	/// Used for single-threaded process-all.
	void process();

	void write_master();

	void write_channel(int channel);
};

}
