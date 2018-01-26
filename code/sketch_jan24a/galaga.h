#pragma once
#include "SoundSystem.h"
#include "scale16.h"
const Note galaga[] = {
    {G3, E4, 9}, 
    {C4, G3, 3},
    {D4, D4, 9},
    {F4, A3, 3},
    {E4, C4, 9},
    {C4, G3, 3},
    {D4, A3, 9},
    {A4, B3, 3},
    {G4, E4, 9},
    {C4, C4, 3},
    {D4, D4, 9},
    {F4, A3, 3},
    {E4, C4, 9},
    {C4, A3, 3},
    {G4, D4, 9},
    {B4, G4, 3}, // 16
    {C5, Gx4, 9},
    {Ax4, G4, 3},
    {Gx4, F4, 9},
    {G4, Dx4, 3},
    {F4, D4, 9},
    {Dx4, C4, 3}, // 22
    {D4, Ax3, 9},
    {C4, Gx3, 3},
    {Ax4, Dx4, 9},
    {C5, F4, 3},
    {G4, E4, 9},
    {F4, C4, 3},
    {A4, F4, 4},
    {F4, D4, 4},
    {D4, A3, 4},
    {G4, D4, 4},
    {E4, B3, 4},
    {D4, A3, 4}
  };
extern const uint8_t galaga_length = sizeof(galaga) / sizeof(Note);

