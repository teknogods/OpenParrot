#pragma once
enum class NesicaKey
{
	None = -1,
	MagicalBeat = 0,
	BlazBlueCentralFriction = 1,
	Persona4Arena = 2,
	BlazBlueChronoPhantasma = 3,
	KOFXIIIClimax = 4,
	Persona4Ultimix = 5,
	USF4 = 6,
	NoNet = 7,
};

void init_CryptoPipe(NesicaKey id);