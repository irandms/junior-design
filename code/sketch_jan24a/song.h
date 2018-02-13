#pragma once

#include "SoundSystem.h"
#include "scale16.h"

const Note song[] = {
    {G3, E4, 18}, 
    {C4, G3, 6},
    {D4, D4, 18},
    {F4, A3, 6},
    {E4, C4, 18},
    {C4, G3, 6},
    {D4, A3, 18},
    {A4, B3, 6},
    {G4, E4, 18},
    {C4, C4, 6},
    {D4, D4, 18},
    {F4, A3, 6},
    {E4, C4, 18},
    {C4, A3, 6},
    {G4, D4, 18},
    {B4, G4, 6}, // 16
    {C5, Gx4, 18},
    {Ax4, G4, 6},
    {Gx4, F4, 18},
    {G4, Dx4, 6},
    {F4, D4, 18},
    {Dx4, C4, 6}, // 22
    {D4, Ax3, 18},
    {C4, Gx3, 6},
    {Ax4, Dx4, 18},
    {C5, F4, 6},
    {G4, E4, 18},
    {F4, C4, 6},
    {A4, F4, 8},
    {F4, D4, 8},
    {D4, A3, 8},
    {G4, D4, 8},
    {E4, B3, 8},
    {D4, A3, 8}
};

const uint8_t song_length = sizeof(song) / sizeof(Note);

const Note upscale1[] = {
    {C4, C4, 9}, 
    {D4, D4, 9},
    {E4, E4, 9},
    {F4, F4, 9},
    {G4, G4, 9},
    {A4, A4, 9},
    {B4, B4, 9},
    {C5, C5, 9}
};

const uint8_t upscale1_length = sizeof(upscale1) / sizeof(Note);

const Note takeonme[] = {
    {Fx4, Fx4, 12},
    {0,0,1},
    {Fx4, Fx4, 12},
    {0,0,1},
    {D4, D4, 12},
    {0,0,1},
    {C4, C4, 12},
    {0,0,4},
    {C4, C4, 12},
    {0,0,4},
    {E4, E4, 12},
    {0,0,4},
    
    {E4, E4, 12},
    {0,0,4},
    {E4, E4, 12},
    {0,0,1},
    {Gx4, Gx4, 12},
    {0,0,1},
    {Gx4, Gx4, 12},
    {0,0,1},
    {A4, A4, 12},
    {0,0,1},
    {B4, B4, 12},
    {0,0,1},

    {A4, A4, 12},
    {0,0,1},
    {A4, A4, 12},
    {0,0,1},
    {A4, A4, 12},
    {0,0,1},
    {E4, E4, 12},
    {0,0,4},
    {D4, D4, 12},
    {0,0,4},
    {Fx4, Fx4, 12},
    {0,0,4},

    {Fx4, Fx4, 12},
    {0,0,4},
    {Fx4, Fx4, 12},
    {0,0,1},
    {E4, E4, 12},
    {0,0,1},
    {E4, E4, 12},
    {0,0,1},
    {Fx4, Fx4, 12},
    {0,0,1},
    {E4, E4, 12},
    {0,0,1},
};

const uint8_t takeonme_length = sizeof(takeonme) / sizeof(Note);

