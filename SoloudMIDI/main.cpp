#include "soloud.h"
#include "soloud_midi.h"
#include <iostream>

int main(int argc, char* argv[])
{
	SoLoud::Soloud* mSoloud = new SoLoud::Soloud();
	SoLoud::SoundFont* MidiFont = new SoLoud::SoundFont();
	SoLoud::Midi* MidigWave = new SoLoud::Midi();

	mSoloud->init(SoLoud::Soloud::LEFT_HANDED_3D);

	/*
	播放有问题可以去跳一下 int MidiInstance::tick(float* stream, int SampleCount) 里面的：
	for (SampleBlock = 1024; SampleCount; SampleCount -= SampleBlock, stream += SampleBlock)
	SampleBlock 的 参数；但是要是2倍数的调整
	*/

	MidiFont->load("TimGM6mb.sf2");
	MidigWave->load("夜に駆ける.mid", *MidiFont); //读取音频
	mSoloud->play3d(*MidigWave,0,0,0);

	while (mSoloud->getActiveVoiceCount() > 0)
	{

	}

	delete MidigWave;
	delete MidiFont;
	delete mSoloud;
	return 0;
}
