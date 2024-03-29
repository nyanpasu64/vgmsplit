//vgmsplit - a program to extract the channels from a chiptune file
//Copyright 2011 Bryan Mitchell

// vgmsplit fork by nyanpasu64, 2018-

/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include "renderer.h"

#include "CLI11/CLI11.hpp"

#include <string>
#include <utility>  // std::move

using namespace vgmsplit;

#ifndef VGMSPLIT_VERSION
#define VGMSPLIT_VERSION "unknown version"
#endif

using std::move;
using renderer::Renderer;

int main(int argc, char** argv) {
	CLI::App app{"vgmsplit " VGMSPLIT_VERSION ": Program to record channels from chiptune files"};

	Arguments args;
	bool parallel;
	{
		std::string filename;
		int tracknum = 1;
		int sample_rate = 44100;
		double tracklen_s = -1;
		double fade_s = 5;
		bool no_parallel = false;

		app.add_option("filename", filename, "Any music file accepted by GME")->required();
		app.add_option("tracknum", tracknum, "Track number (first track is 1)");
		app.add_option("track_len", tracklen_s, "How long to record, in seconds (defaults to file metadata)");
		app.add_option("fade_len", fade_s, "How long to fade out, in seconds (defaults to 5.0 seconds, NOT .spc metadata)");
		app.add_option("-r,--rate", sample_rate, "Sampling rate (defaults to 44100 Hz)");
		app.add_flag("--no-parallel", no_parallel, "Disable parallel rendering of channels");
		app.failure_message(CLI::FailureMessage::help);
		CLI11_PARSE(app, argc, argv);

		// User enters a 1-indexed number, we pass 0-indexed to libgme.
		tracknum -= 1;

		args = Arguments {
			.filename = move(filename),
			.tracknum = tracknum,
			.tracklen_ms = static_cast<int>(tracklen_s * 1000),
			.fade_ms = static_cast<int>(fade_s * 1000),
			.sample_rate = sample_rate,
		};
		parallel = !no_parallel;
	}

	// WAV renderer is no longer single-threaded.
	// TODO distribute channels across a thread pool.

	// Alternatively, if the user passes in more than 1 channel,
	// call multiple child processes and pass each process 1 channel.
	// This allows the user to selectively terminate processes.

	if (parallel)
	#pragma omp parallel
	{
		auto channel_count = renderer::Renderer(args).channel_count();

		#pragma omp for
		for (int channel = -1; channel < channel_count; ++channel) {
			auto renderer = renderer::Renderer(args);
			if (channel == -1) {
				renderer.write_master();
			} else {
				renderer.write_channel(channel);
			}
		}
	}
	else {
		renderer::Renderer single_threaded{move(args)};
		single_threaded.process();
	}

	return 0;
}
