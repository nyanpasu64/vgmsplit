#include <utility>

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


#include "CLI11/CLI11.hpp"

#include "wave_writer.h"
#include "gme/gme.h"

#include <iostream>
#include <string>
#include <sstream>

#ifdef EXPERIMENTAL_FS
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif


using gme_t = Music_Emu;
using std::string;

static const char *const WAV_EXT = ".wav";
static const int BUF_SIZE = 1024;
static const int MS_SECOND = 1000;


std::string num2str(int x) {
	std::stringstream result;

	result << x;

	return result.str();
}


class VgmSplit {
public:
	VgmSplit(string filename, int tracknum, int tracklen_ms, int sample_rate) :
			filename(std::move(filename)),
			tracknum(tracknum),
			tracklen_ms(tracklen_ms),
			emu(nullptr),
			sample_rate(sample_rate) {}

	int process() {
		// Load file.
		const char* err1 = gme_open_file(filename.c_str(), &emu, sample_rate);
		if (err1) {
			std::cerr << err1;
			return 1;
		}

		// Load length.
		if (tracklen_ms < 0) {
			gme_info_t* info;
			gme_track_info(emu, &info, tracknum);

			tracklen_ms = info->play_length;  // Guaranteed to be either valid, or 2.5 minutes.
			delete info;
		}

		//Ignoring silence allows us to record tracks that start in or have
		//long periods of silence. Unfortunately, this also means that
		//if a track is of finite length, we still need to have its length separately.
		gme_ignore_silence(emu, true);
		const char* err2 = gme_start_track(emu, tracknum);
		if (err2) {
			std::cerr << err2;
			return 1;
		}

		//Run the emulator for a second while muted to eliminate opening sound glitch
		for (int len = 0; len < MS_SECOND; len = gme_tell(emu)) {
			int m = -1;
			m ^= 1;
			gme_mute_voices(emu, m);
			short buf[BUF_SIZE];
			gme_play(emu, BUF_SIZE, buf);
		}

		// Render channels.
		writeMaster();
		for (int channel = 0; channel < gme_voice_count(emu); channel++) {
			writeChannel(channel);
		}

		gme_delete(emu);

		return 0;
	}

private:
	void writeMaster() {
		std::cout << "Rendering all channels..." << std::endl;

		// Initialize GEP.
		const char* err = gme_start_track(emu, tracknum);
		if (err) {
			std::cerr << err;
			exit(1);
		}

		// Unmute all channels.
		int mute = 0;
		gme_mute_voices(emu, mute);

		// Render to file.
		fs::path wav_name = this->filename;
		wav_name.replace_extension(WAV_EXT);
		write(wav_name.filename().string());
	}

	void writeChannel(int channel) {
		//The filename will be a number, followed by a space and its track title.
		//This ensures both unique and (in most cases) descriptive file names.
		std::string channel_name = num2str(channel+1);
		channel_name += "-";
		channel_name += (std::string)gme_voice_name(emu, channel);
		std::cout << "Rendering channel " << channel_name << "..." << std::endl;

		const char* err = gme_start_track(emu, tracknum);
		if (err) {
			std::cerr << err;
			exit(1);
		}

		//Create a muting mask to isolate the channel
		int mute = -1;
		mute ^= (1 << channel);
		gme_mute_voices(emu, mute);

		auto wav_name = channel_name + WAV_EXT;
		write(wav_name);
	}

	/**
	 * Dump all channels currently unmuted in this->emu.
	 * @param wav_name Output path
	 */
	void write(const string &wav_name) {
		//Create a buffer to hand the data from GME to wave_write
		short buffer[BUF_SIZE];     // I'd use int16_t, but gme_play() and wave_write() use short[].

		//Sets up the header of the WAV file so it is, in fact, a WAV
		wave_open(sample_rate, wav_name.c_str());
		wave_enable_stereo(); //GME always outputs in stereo

		// Set play time and record until fadeout is complete.
		gme_set_fade(emu, tracklen_ms);
		while (!gme_track_ended(emu)) {
			//If an error occurs during play, we still need to close out the file
			if (gme_play(emu, BUF_SIZE, buffer)) break;
			wave_write(buffer, BUF_SIZE);
		}

		//Properly finishes the header and closes the internal file object
		wave_close();
	}

	// Input parameters
	string filename;
	int tracknum;
	int tracklen_ms;

	// Computed state
	gme_t* emu;
	int sample_rate;
};


int main ( int argc, char** argv ) {
	CLI::App app{"vgmsplit: Program to record channels from chiptune files"};

	std::string filename;
	int tracknum = 1;
	int tracklen_ms;
	int sample_rate = 44100;
	{
		double _tracklen_s = -1;

		app.add_option("filename", filename, "Any music file accepted by GME")->required();
		app.add_option("tracknum", tracknum, "Track number (first track is 1)");
		app.add_option("tracklen", _tracklen_s, "How long to record, in seconds");
		app.add_option("-r,--rate", sample_rate, "Sampling rate (defaults to 44100 Hz)");
		app.failure_message(CLI::FailureMessage::help);
		CLI11_PARSE(app, argc, argv);

		tracknum -= 1;
		tracklen_ms = static_cast<int>(_tracklen_s * 1000);
	}

	VgmSplit vgmsplit{filename, tracknum, tracklen_ms, sample_rate};
	return vgmsplit.process();
}
