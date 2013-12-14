//
//  msgf_amp_pipe.h
//  ToneGenerator
//
//  Created by 長谷部 雅彦 on 2013/12/08.
//  Copyright (c) 2013年 長谷部 雅彦. All rights reserved.
//

#ifndef __msgf_amp_pipe__
#define __msgf_amp_pipe__

#include <iostream>
#include "msgf_type.h"
#include "msgf_signal_process_core.h"
#include "msgf_note.h"
#include "msgf_lfo.h"
#include "msgf_voice_context.h"
#include "msgf_eg2seg.h"

namespace msgf {
	
	//	to be defined every Audio Engine that wants an original parameter set
	const int VP_AMP_PIPE_ID = 220;
	//---------------------------------------------------------
	typedef enum {
		
		VP_VOLUME,				//	0 - 100
		
		VP_AEG_ATTACK_TIME,		//	0 - 1000 (*10ms)
		VP_AEG_RELEASE_TIME,	//	0 - 1000 (*10ms)
		
		VP_LFO_AMD,				//	0 - 100 %
		
		VP_AMPLITUDE_MAX
		
	} VoiceParameter_AmpPipe;
	//---------------------------------------------------------
	class TgAudioBuffer;
	class AegPCallBack;
	//---------------------------------------------------------
	class AmpPipe : public SignalProcessCore {
		
		friend class AegPCallBack;
		
	public:
		AmpPipe( Note& parent );
		~AmpPipe( void );
		
		void	init( void );
		void	release( void ){ _eg->moveToRelease(); }
		
		//	process thread
		void	process( TgAudioBuffer& buf );
		
		static const int AMP_PRM_MAX = 100;
		
	private:
		//	original
		double	calcMidiVolume( double amp );
		
		//	Get Segment Parameter
		int		getVoicePrm( int id ){ return _parentNote.getVoiceContext()->getParameter( VP_AMP_PIPE_ID, id ); }
		int		getAttackDacCount( void ){ return getTotalDacCount(getVoicePrm(VP_AEG_ATTACK_TIME)); }
		int		getReleaseDacCount( void ){ return getTotalDacCount(getVoicePrm(VP_AEG_RELEASE_TIME)); }
		
		//	Basic Reference
		Note&	_parentNote;
		
		//	EG Level
		AegPCallBack*	_cbInst;
		Eg2segment*		_eg;
		double			_realVol;
		double			_targetVol;
		
		//	LFO
		Lfo*	_am;
		double	_amd;
	};
	//---------------------------------------------------------
	class AegPCallBack : public CallBack {
		
	public:
		AegPCallBack( AmpPipe* amp ):
		_amp(amp){}
		virtual double	getEgLvl( EG_STATE prm )
		{
			switch (prm){
				case EG_NOT_YET: return 0;
				case EG_ATTACK:	 return 0;
				case EG_RELEASE: return 0;
				default: return 0;
			}
		}
		virtual int		getEgTime( EG_STATE prm )
		{
			switch (prm){
				case EG_ATTACK:	 return _amp->getAttackDacCount();
				case EG_RELEASE: return _amp->getReleaseDacCount();
				default: return 0;
			}
		}
	private:
		AmpPipe*	_amp;
	};
}
#endif /* defined(__ToneGenerator__msgf_amp_pipe__) */