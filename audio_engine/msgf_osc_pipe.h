//
//  msgf_osc_pipe.h
//  ToneGenerator
//
//  Created by 長谷部 雅彦 on 2013/12/01.
//  Copyright (c) 2013年 長谷部 雅彦. All rights reserved.
//

#ifndef __ToneGenerator__msgf_osc_pipe__
#define __ToneGenerator__msgf_osc_pipe__

#include <iostream>
#include <math.h>
#include "msgf_type.h"
#include "msgf_signal_process_core.h"
#include "msgf_voice_context.h"
#include "msgf_note.h"
#include "msgf_eg2seg.h"
#include "msgf_lfo.h"

namespace msgf {
	
	//	to be defined every Audio Engine that wants an original parameter set
	const int VP_OSC_PIPE_ID = 320;
	//---------------------------------------------------------
	typedef enum {
		
		//	Pitch
		VP_TUNING,				//	-100 - 100[cent]
		VP_TRANSPOSE,			//	-24 - 24(seminote)
		//	Waveform
		VP_CARRIER_FREQ,		//	0 - 20?
		VP_CARRIER_LEVEL,		//	0 - 100[%]
		//	Portamento
		VP_PORTAMENTO_MODE,		//	0:rate constant, 1:time constant
		VP_PORTAMENTO_CURVE,	//	0:cent linear, 1:freqency linear
		VP_PORTAMENTO,			//	0 - ??? (rate: *10 =[ms]/100[cent], time: *10 =[ms])
		//	LFO
		VP_LFO_FREQUENCY,		//	0 - ??? (*0.1 =[Hz])
		VP_LFO_DELAY_TIME,		//	0 - ??? (*10 =[ms])
		VP_LFO_FADEIN_TIME,		//	0 - ??? (*10 =[ms])
		VP_LFO_PMD,				//	0 - 100 %
		//	Note Change EG
		VP_WAITING_DCNT,		//	0 - inf (*22.67 =[us])
		VP_FAST_MOVE_DCNT,		//	0 - inf	(*22.67 =[us])
		VP_PORTAMENTO_DIFF,		//	-24 - 24(semitone)

		VP_OSCILLATOR_MAX
		
	} VoiceParameter_OscMono;
	//---------------------------------------------------------
	typedef enum {
		
		SINE,
		TRIANGLE,
		WAVEFORM_MAX
		
	} WAVEFORM;
	
	//---------------------------------------------------------
	typedef enum {
		
		NO_MOVE,
		WAITING_PRTM,	//	Time of from event to begin to move
		FAST_MOVE,		//	fast move time
		SLOW_MOVE,		//	slow move time
		PRTM_STATE_MAX
		
	} PRTM_STATE;
	
	//---------------------------------------------------------
	class TgAudioBuffer;
	class PegCallBack;
	//---------------------------------------------------------
	class OscPipe : public SignalProcessCore {
		
		friend class PegCallBack;
		
	public:
		OscPipe( Note& parent );
		~OscPipe( void );
		
		void	init( void ){ init(true);}
		void	init( bool phaseReset );
		void	changeNote( void );
		void	release( void ){}
		
		//	process thread
		void	process( TgAudioBuffer& buf );
		
		static const int PEG_MAX = 60;
		static const int PEG_DEPTH_MAX = 2; // /2 Octave
		
	private:
		
		double	calcPitch( const Uint8 note );
		double	getPegPitch( int depth );
		void	reflectMidiController( void );
		double	calcDeltaLFO( double lfoDpt, double diff );
		void	stateOfWaitingPortamento( void );
		void	stateOfFastMove( void );
		void	stateOfSlowMove( void );
		void	managePortamentoState( void );
		void	setPortamentoCounter( double centDiff );
		double	generateWave( double phase );
		
		int		getVoicePrm( int id ){ return _parentNote.getVoiceContext()->getParameter( VP_OSC_PIPE_ID, id ); }
		
		static const double tPitchOfA[11];

		//	Basic Reference
		Note&		_parentNote;
		
		//	generate waveform
		Uint8		_note;
		double		_pitch;
		double		_pitchAdj;
		double		_crntPhase;
		int			_carrierFreq;
		double		_carrierLevel;

		//	portamento
		int			_portamentoCounter;
		int			_maxPortamentoCounter;
		double		_sourcePitch;
		double		_targetPitch;
		double		_targetCent;
		PRTM_STATE	_prtmState;
		
		//	LFO
		Lfo*	_pm;
		double	_pmd;
	};
}
#endif /* defined(__ToneGenerator__msgf_osc_pipe__) */
