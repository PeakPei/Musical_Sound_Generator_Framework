//
//  msgf_amplitude.cpp
//
//	Musical Sound Generator Framework
//
//  Created by 長谷部 雅彦 on 2012/10/31.
//  Copyright (c) 2012年 長谷部 雅彦. All rights reserved.
//

#include "msgf_amplitude.h"
#include "msgf_note.h"
#include "msgf_audio_buffer.h"
#include "msgf_voice_context.h"
#include "msgf_instrument.h"
#include "msgf_part.h"
using namespace msgf;

//---------------------------------------------------------
#define EG_LEVEL_MAX	100
#define EG_LEVEL_MIN	0

//---------------------------------------------------------
//		Move to next segment
//---------------------------------------------------------
void Amplitude::toAttack( void )
{
	//	time
	_state = ATTACK;
	_egStartDac = _dacCounter = 0;
	_egTargetDac = _egStartDac
		+ getTotalDacCount(_parentNote->getVoiceContext()->getParameter(VP_AEG_ATTACK_TIME));

	//	level
	_startLvl = EG_LEVEL_MIN;
	_targetLvl = EG_LEVEL_MAX;
}
//---------------------------------------------------------
void Amplitude::toDecay1( void )
{
	//	time
	_state = DECAY1;
	_egStartDac = _dacCounter;
	_egTargetDac = _egStartDac +
		+ getTotalDacCount(_parentNote->getVoiceContext()->getParameter(VP_AEG_DECAY1_TIME));
	
	//	level
	_startLvl = EG_LEVEL_MAX;
	_targetLvl = _parentNote->getVoiceContext()->getParameter(VP_AEG_DECAY1_LEVEL);
}
//---------------------------------------------------------
void Amplitude::toDecay2( void )
{
	//	time
	_state = DECAY2;
	_egStartDac = _dacCounter;
	_egTargetDac = _egStartDac +
		+ getTotalDacCount(_parentNote->getVoiceContext()->getParameter(VP_AEG_DECAY2_TIME));
	
	//	level
	_startLvl = _targetLvl;
	_targetLvl = _parentNote->getVoiceContext()->getParameter(VP_AEG_DECAY2_LEVEL);
}
//---------------------------------------------------------
void Amplitude::toDecay2Steady( void )
{
	//	time
	_state = KEY_ON_STEADY;
	_egStartDac = _dacCounter;

	//	level
	_startLvl = _targetLvl;
}
//---------------------------------------------------------
void Amplitude::toRelease( void )
{
	_state = RELEASE;
	_startLvl = static_cast<int>(getAegLevel( _dacCounter-_egStartDac, _egTargetDac-_egStartDac, _startLvl, _targetLvl ));

	//	time
	_egStartDac = _dacCounter;
	_egTargetDac = _egStartDac
		+ getTotalDacCount(_parentNote->getVoiceContext()->getParameter(VP_AEG_RELEASE_TIME));

	_targetLvl = EG_LEVEL_MIN;
}

//---------------------------------------------------------
//		Calculate AEG
//---------------------------------------------------------
double Amplitude::getAegLevel( long crntDac, long targetDac, int startLvl, int targetLvl )
{
	if ( targetDac == 0 ) return startLvl;
	return (double)((targetLvl - startLvl)*crntDac)/targetDac + startLvl;
}

//---------------------------------------------------------
//		Calculate Volume
//---------------------------------------------------------
double Amplitude::calcVolume( double amp )
{
	double	vol = _parentNote->getVoiceContext()->getParameter(VP_VOLUME);
	amp *= (vol/EG_LEVEL_MAX);

	Uint8 midiVal = _parentNote->getInstrument()->getPart()->getCc7();
	amp = (amp*midiVal)/127;
	midiVal = _parentNote->getInstrument()->getPart()->getCc11();
	amp = (amp*midiVal)/127;

	return amp;
}

//---------------------------------------------------------
//		Process Function
//---------------------------------------------------------
void Amplitude::checkEvent( void )
{
	switch (_state){
		case EG_NOT_YET:{
			if ( _parentNote->conditionKeyOn() == true ){
				//	Start key On
				toAttack();
			}
			break;
		}
		case ATTACK:
		case DECAY1:
		case DECAY2:
		case KEY_ON_STEADY:{
			if ( _parentNote->conditionKeyOn() == false ){
				//	Key Off
				toRelease();
			}
			break;
		}
		default: break;
	}
}
//---------------------------------------------------------
void Amplitude::checkSegmentEnd( void )
{
	switch (_state){
		case ATTACK:
			toDecay1();
			break;
		case DECAY1:
			toDecay2();
			break;
		case DECAY2:{
			if ( _targetLvl != EG_LEVEL_MIN ){
			}
			break;
		}
		case RELEASE:
		default: break;
	}
}
//---------------------------------------------------------
void Amplitude::process( TgAudioBuffer& buf )
{
	//	check Event
	checkEvent();
	
	//	write Buffer
	for ( int i=0; i<buf.bufferSize(); i++ ){

		//	Change AEG Segment
		if ( _dacCounter >= _egTargetDac ){
			checkSegmentEnd();
		}

		//	calc real amplitude
		double aeg = getAegLevel( _dacCounter-_egStartDac, _egTargetDac-_egStartDac, _startLvl, _targetLvl );
		aeg /= EG_LEVEL_MAX;

		//	calculate Volume
		double vol = calcVolume( aeg );
		
		buf.mulAudioBuffer( i, vol*vol );
		_dacCounter++;
	}
}
