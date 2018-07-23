/* Simple wave sound file writer for use in demo programs. */

/* Game_Music_Emu 0.6-pre */
#ifndef WAVE_WRITER_H
#define WAVE_WRITER_H

#ifdef __cplusplus
	extern "C" {
#endif

/* ERRORS: If error occurs (out of memory, disk full, etc.), functions print
cause then exit program. */

void wave_open( int sample_rate, const char filename [] );
/* EFFECTS: Creates and opens sound file of given sample rate and filename. */

void wave_enable_stereo( void );
/* EFFECTS: Enables stereo output. */

void wave_write( const short in [], int count );
/* EFFECTS: Appends count samples to file. */

int wave_sample_count( void );
/* RETURNS: Number of samples written so far. */

void wave_close( void );
/* EFFECTS: Finishes writing sound file and closes it. */

#ifdef __cplusplus
	}
#endif

#ifdef __cplusplus

/* C++ interface */
class Wave_Writer {
public:
	typedef short sample_t;
	Wave_Writer( int rate, const char file [] = "out.wav" ) { wave_open( rate, file ); }
	void enable_stereo()                                    { wave_enable_stereo(); }
	void write( const sample_t in [], int n )               { wave_write( in, n ); }
	int sample_count() const                                { return wave_sample_count(); }
	void close()                                            { wave_close(); }
	~Wave_Writer()                                          { wave_close(); }
};
#endif

#endif
