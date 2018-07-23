//towave - a program to extract the channels from a chiptune file
//Copyright 2011 Bryan Mitchell
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

#include "wave_writer.h"
#include "gme/gme.h"
#include <iostream>
#include <string>
#include <sstream>


std::string num2str(int x) {
	std::stringstream result;
	
	result << x;
	
	return result.str();
}

void writeTheWave(gme_t* emu, int tracknum, int tracklen, int i, int sample_rate) {
	//Ignoring silence allows us to record tracks that start in or have
	//long periods of silence. Unfortunately, this also means that
	//if a track is of finite length, we still need to have its length separately.
	gme_ignore_silence(emu, true);
	
	//Create a muting mask to isolate the channel
	int mute = -1;
	mute ^= (1 << i);
	gme_mute_voices(emu, mute);
	
	//Create a buffer to hand the data from GME to wave_write
	int buf_size = 1024;
	short buffer[1024];
	
	//The filename will be a number, followed by a space and its track title.
	//This ensures both unique and (in most cases) descriptive file names.
	std::string wav_name = num2str(i+1);
	wav_name += "-";
	wav_name += (std::string)gme_voice_name(emu, i);
	wav_name += ".wav";
	
	//Sets up the header of the WAV file so it is, in fact, a WAV
	wave_open(sample_rate, wav_name.c_str());
	wave_enable_stereo(); //GME always outputs in stereo
	
	//Perform the magic.
	while (gme_tell(emu) < tracklen*1000) {
		//If an error occurs during play, we still need to close out the file
		if (gme_play(emu, buf_size, buffer)) break;
		wave_write(buffer, buf_size);
	}
	
	//Properly finishes the header and closes the internal file object
	wave_close();
}

int main ( int argc, char** argv ) {
	std::string filename;
	
	if (argc > 1) filename = argv[argc - 1];
	else {
		std::cout << "Please call towave from the command prompt." << std::endl;
		std::cout << "Proper syntax is" << std::endl;
		std::cout << "\ttowave filename" << std::endl;
		std::cout << "Where filename is of any type accepted by GME. (See readme)";
		return 0;
	}
	
	gme_t* emu;
	int sample_rate = 44100;
	const char* err1 = gme_open_file(filename.c_str(), &emu, sample_rate);
	
	if (err1) {
		std::cout << err1;
		return 1;
	}
	
	int tracknum, tracklen;
	
	std::cout << "Track number (first track is 1): ";
	std::cin >> tracknum;
	tracknum--; //first track for GME is 0
	std::cout << std::endl;
	
	std::cout << "How long to record, in seconds: ";
	std::cin >> tracklen;
	
	const char* err2 = gme_start_track(emu, tracknum);
	if (err2) {
		std::cout << err2;
		return 1;
	}
	//Run the emulator for a second while muted to eliminate opening sound glitch
	for (int len = 0; len < 1000; len = gme_tell(emu)) {
		int m = -1;
		m ^= 1;
		gme_mute_voices(emu, m);
		short buf[1024];
		gme_play(emu, 1024, buf);
	}
	
	for (int i = 0; i < gme_voice_count(emu); i++) {
		const char* err = gme_start_track(emu, tracknum);
		if (err) {
			std::cout << err;
			return 1;
		}
		writeTheWave(emu, tracknum, tracklen, i, sample_rate);
	}
	
	gme_delete(emu);
	
	return 0;
}