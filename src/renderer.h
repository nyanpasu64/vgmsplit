#pragma once

#include "main.h"
#include "util/copy_move.h"

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

	// Allow copying. (Not currently used, but it works despite being hackish.)
	Renderer(Renderer const& other) : Renderer(other._args) {}

    // Disable copy assignment (haven't figured it out yet).
    Renderer &operator=(Renderer const&) = delete;

    // Disable move (haven't figured it out yet).
	DISABLE_MOVE(Renderer)


	/// Use the return value to determine how many times to call write_channel().
	int channel_count() const;

	/// Calls write_master() and write_channel().
	void process();

	void write_master();

	void write_channel(int channel);
};

}
