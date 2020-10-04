#define renderer_INTERNAL public
#include "renderer.h"
#include "wave_writer.h"
#include "gme/gme.h"
#include "gme/Music_Emu.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <filesystem>

namespace vgmsplit::renderer {

namespace fs = std::filesystem;

using gme_t = Music_Emu;

static constexpr auto WAV_EXT = ".wav";
static constexpr int BUF_SIZE = 1024;
static constexpr int MS_SECOND = 1000;

Renderer::Renderer(Arguments args)
	: _args(std::move(args))
	, _emu(nullptr)
{
	// Load file.
	const char* err1 = gme_open_file(_args.filename.c_str(), &_emu, _args.sample_rate);
	if (err1) {
		std::cerr << err1 << "\n";
		throw std::runtime_error(err1);
	}

	// Load length.
	if (_args.tracklen_ms < 0) {
		gme_info_t* info;
		gme_track_info(_emu, &info, _args.tracknum);

		_args.tracklen_ms = info->play_length;  // Guaranteed to be either valid, or 2.5 minutes.
		delete info;
	}

	//Ignoring silence allows us to record tracks that start in or have
	//long periods of silence. Unfortunately, this also means that
	//if a track is of finite length, we still need to have its length separately.
	gme_ignore_silence(_emu, true);
	const char* err2 = gme_start_track(_emu, _args.tracknum);
	if (err2) {
		std::cerr << err2 << "\n";
		throw std::runtime_error(err2);
	}

	//Run the emulator for a second while muted to eliminate opening sound glitch
	for (int len = 0; len < MS_SECOND; len = gme_tell(_emu)) {
		// Mute all but channel 0.
		int m = ~0 ^ 1;
		gme_mute_voices(_emu, m);
		short buf[BUF_SIZE];
		gme_play(_emu, BUF_SIZE, buf);
	}
}

Renderer::~Renderer() {
	gme_delete(_emu);
}

static void renderer_write(Renderer & self, char const* wav_name) {
	auto const& _args = self._args;
	auto & _emu = self._emu;

	//Create a buffer to hand the data from GME to wave_write
	short buffer[BUF_SIZE];     // I'd use int16_t, but gme_play() and wave_write() use short[].

	//Sets up the header of the WAV file so it is, in fact, a WAV
	auto wave = Wave_Writer(_args.sample_rate, wav_name);
	wave.enable_stereo(); //GME always outputs in stereo

	// Set play time and record until fadeout is complete.
	bool should_fade = _args.fade_ms > 0;

	if (should_fade) {
		_emu->set_fade(_args.tracklen_ms, _args.fade_ms);
	}
	while (should_fade ? (!gme_track_ended(_emu)) : (gme_tell(_emu) < _args.tracklen_ms)) {
		//If an error occurs during play, we still need to close out the file
		if (gme_play(_emu, BUF_SIZE, buffer)) break;
		wave.write(buffer, BUF_SIZE);
	}

	// Technically unnecessary because Wave_Writer calls close() upon destruction.
	wave.close();
}

int Renderer::channel_count() const {
	return gme_voice_count(_emu);
}

void Renderer::process() {
	// Render channels.
	write_master();
	for (int channel = 0; channel < channel_count(); channel++) {
		write_channel(channel);
	}
}

void Renderer::write_master() {
	std::cout << "Rendering all channels..." << std::endl;

	// Initialize GEP.
	const char* err = gme_start_track(_emu, _args.tracknum);
	if (err) {
		std::cerr << err << "\n";
		exit(1);
	}

	// Unmute all channels.
	int mute = 0;
	gme_mute_voices(_emu, mute);

	// Render to file.
	auto wav_name = fs::path(_args.filename);
	wav_name.replace_extension(WAV_EXT);
	renderer_write(*this, wav_name.filename().c_str());
}

static std::string num_to_str(int x) {
	std::stringstream result;
	result << x;
	return result.str();
}

void Renderer::write_channel(int channel) {
	//The filename will be a number, followed by a space and its track title.
	//This ensures both unique and (in most cases) descriptive file names.
	std::string channel_name = num_to_str(channel+1);
	channel_name += "-";
	channel_name += (std::string)gme_voice_name(_emu, channel);
	std::cout << "Rendering channel " << channel_name << "..." << std::endl;

	const char* err = gme_start_track(_emu, _args.tracknum);
	if (err) {
		std::cerr << err << "\n";
		exit(1);
	}

	//Create a muting mask to isolate the channel
	int mute = ~0;
	mute ^= (1 << channel);
	gme_mute_voices(_emu, mute);

	std::string wav_name = channel_name + WAV_EXT;
	renderer_write(*this, wav_name.c_str());
}

}
