#ifndef APO_R19_RESTORE_H
#define APO_R19_RESTORE_H
/* R21 uses the R2 core with only the final pitch interpolation changed. */
#undef APO_SCRATCH_DSP_H
#define ApoScratchStretch r19_ApoScratchStretch
#define ApoSpectralFreeze r19_ApoSpectralFreeze
#define scratch_abs_d r19_scratch_abs_d
#define scratch_clamp_d r19_scratch_clamp_d
#define scratch_wrap_angle r19_scratch_wrap_angle
#define scratch_sincos r19_scratch_sincos
#define scratch_sqrt r19_scratch_sqrt
#define scratch_atan2 r19_scratch_atan2
#define scratch_fft r19_scratch_fft
#define scratch_ifft r19_scratch_ifft
#define scratch_read_source r19_scratch_read_source
#define scratch_stretch_init r19_scratch_stretch_init
#define scratch_analyze r19_scratch_analyze
#define scratch_synthesis_frame r19_scratch_synthesis_frame
#define scratch_ws_synthesis_frame r19_scratch_ws_synthesis_frame
#define scratch_stretch_render r19_scratch_stretch_render
#include "r21_r2_pitch.h"
#undef ApoScratchStretch
#undef ApoSpectralFreeze
#undef scratch_abs_d
#undef scratch_clamp_d
#undef scratch_wrap_angle
#undef scratch_sincos
#undef scratch_sqrt
#undef scratch_atan2
#undef scratch_fft
#undef scratch_ifft
#undef scratch_read_source
#undef scratch_stretch_init
#undef scratch_analyze
#undef scratch_synthesis_frame
#undef scratch_ws_synthesis_frame
#undef scratch_stretch_render
#endif
