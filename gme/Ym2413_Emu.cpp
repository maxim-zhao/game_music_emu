// Game_Music_Emu $vers. http://www.slack.net/~ant/

#include "Ym2413_Emu.h"

#include <algorithm>

extern "C" {
#include "../vgmplay/VGMPlay/chips/emu2413.h"
}


static Ym2413_Emu::sample_t saturate(int value)
{
    if (value > std::numeric_limits<Ym2413_Emu::sample_t>::max()) 
    {
        return std::numeric_limits<Ym2413_Emu::sample_t>::max();
    }
    if (value < std::numeric_limits<Ym2413_Emu::sample_t>::min()) 
	{
        return std::numeric_limits<Ym2413_Emu::sample_t>::min();
    }
    return static_cast<Ym2413_Emu::sample_t>(value);
}

Ym2413_Emu::Ym2413_Emu() { opll = 0; }

Ym2413_Emu::~Ym2413_Emu()
{
	if ( opll ) OPLL_delete( (OPLL *) opll );
}

int Ym2413_Emu::set_rate( int sample_rate, int clock_rate )
{
	if ( opll )
	{
        OPLL_delete( (OPLL *) opll );
		opll = 0;
	}
	
	opll = OPLL_new( clock_rate, sample_rate );
	if ( !opll )
		return 1;

    OPLL_setChipType( (OPLL *) opll, 0 );
	
	reset();
	return 0;
}

void Ym2413_Emu::reset()
{
	OPLL_reset( (OPLL *) opll );
	OPLL_setMask( (OPLL *) opll, 0 );
}

void Ym2413_Emu::write( int addr, int data )
{
	OPLL_writeIO( (OPLL *) opll, 0, addr );
	OPLL_writeIO( (OPLL *) opll, 1, data );
}

void Ym2413_Emu::mute_voices( int mask )
{
	OPLL_setMask( (OPLL *) opll, mask );
}

void Ym2413_Emu::run( int pair_count, sample_t* out )
{
	for (int i = 0; i < pair_count; i++)
	{
		// Get one sample
		int output = OPLL_calc(static_cast<OPLL*>(opll));

		// Scale up (arbitrarily)
		output *= 3;

		// Add and saturate to 16 bits
		out[0] = saturate(out[0] + output);
		out[1] = saturate(out[1] + output);
		out += 2;
	}
}
