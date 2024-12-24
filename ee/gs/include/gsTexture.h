//  ____     ___ |    / _____ _____
// |  __    |    |___/    |     |
// |___| ___|    |    \ __|__   |     gsKit Open Source Project.
// ----------------------------------------------------------------------
// Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
// Licenced under Academic Free License version 2.0
// Review gsKit README & LICENSE files for further details.
//
// gsTexture.h - Header for gsTexture.c
//

#ifndef __GSTEXTURE_H__
#define __GSTEXTURE_H__

#include "gsKit.h"

#define GS_ST         0x02
#define GS_UV         0x03
#define GS_TEX0_1     0x06
#define GS_TEX0_2     0x07
#define GS_TEX1_1     0x14
#define GS_TEX1_2     0x15
#define GS_TEX2_1     0x16
#define GS_TEX2_2     0x17
#define GS_TEXCLUT    0x1c
#define GS_SCANMSK    0x22
#define GS_MIPTBP1_1  0x34
#define GS_MIPTBP1_2  0x35
#define GS_MIPTBP2_1  0x36
#define GS_MIPTBP2_2  0x37
#define GS_TEXA       0x3b
#define GS_TEXFLUSH   0x3f

#define GS_CLUT_NONE	0x00
#define GS_CLUT_PALLETE 0x01
#define GS_CLUT_TEXTURE 0x02

#define GS_CLUT_STOREMODE_NOLOAD 0x00
#define GS_CLUT_STOREMODE_LOAD 0x01
#define GS_CLUT_STOREMODE_LOAD_CBP0 0x02
#define GS_CLUT_STOREMODE_LOAD_CBP1 0x03
#define GS_CLUT_STOREMODE_COMPARE_CBP0 0x04
#define GS_CLUT_STOREMODE_COMPARE_CBP1 0x05

#define GS_SETREG_COLCLAMP(clamp) ((u64)(clamp))

#define GS_SETREG_MIPTBP1_1 GS_SETREG_MIPTBP1
#define GS_SETREG_MIPTBP1_2 GS_SETREG_MIPTBP1
#define GS_SETREG_MIPTBP1(tbp1, tbw1, tbp2, tbw2, tbp3, tbw3) \
  ((u64)(tbp1)        | ((u64)(tbw1) << 14) | \
  ((u64)(tbp2) << 20) | ((u64)(tbw2) << 34) | \
  ((u64)(tbp3) << 40) | ((u64)(tbw3) << 54))

#define GS_SETREG_MIPTBP2_1 GS_SETREG_MIPTBP2
#define GS_SETREG_MIPTBP2_2 GS_SETREG_MIPTBP2
#define GS_SETREG_MIPTBP2(tbp4, tbw4, tbp5, tbw5, tbp6, tbw6) \
  ((u64)(tbp4)        | ((u64)(tbw4) << 14) | \
  ((u64)(tbp5) << 20) | ((u64)(tbw5) << 34) | \
  ((u64)(tbp6) << 40) | ((u64)(tbw6) << 54))

#define GS_SETREG_TEX0_1  GS_SETREG_TEX0
#define GS_SETREG_TEX0_2  GS_SETREG_TEX0
#define GS_SETREG_TEX0(tbp, tbw, psm, tw, th, tcc, tfx,cbp, cpsm, csm, csa, cld) \
  ((u64)(tbp)         | ((u64)(tbw) << 14) | \
  ((u64)(psm) << 20)  | ((u64)(tw) << 26) | \
  ((u64)(th) << 30)   | ((u64)(tcc) << 34) | \
  ((u64)(tfx) << 35)  | ((u64)(cbp) << 37) | \
  ((u64)(cpsm) << 51) | ((u64)(csm) << 55) | \
  ((u64)(csa) << 56)  | ((u64)(cld) << 61))

#define GS_SETREG_TEX1_1  GS_SETREG_TEX1
#define GS_SETREG_TEX1_2  GS_SETREG_TEX1
#define GS_SETREG_TEX1(lcm, mxl, mmag, mmin, mtba, l, k) \
  ((u64)(lcm)        | ((u64)(mxl) << 2)  | \
  ((u64)(mmag) << 5) | ((u64)(mmin) << 6) | \
  ((u64)(mtba) << 9) | ((u64)(l) << 19) | \
  ((u64)(k) << 32))

#define GS_SETREG_TEX2_1  GS_SETREG_TEX2
#define GS_SETREG_TEX2_2  GS_SETREG_TEX2
#define GS_SETREG_TEX2(psm, cbp, cpsm, csm, csa, cld) \
  (((u64)(psm) << 20) | ((u64)(cbp) << 37) | \
  ((u64)(cpsm) << 51) | ((u64)(csm) << 55) | \
  ((u64)(csa) << 56)  | ((u64)(cld) << 61))

#define GS_SETREG_TEXA(ta0, aem, ta1) \
  ((u64)(ta0) | ((u64)(aem) << 15) | ((u64)(ta1) << 32))

#define GS_SETREG_TEXCLUT(cbw, cou, cov) \
  ((u64)(cbw) | ((u64)(cou) << 6) | ((u64)(cov) << 12))

#define GS_SETREG_TRXDIR(xdr) ((u64)(xdr))

#define GS_SETREG_TRXPOS(ssax, ssay, dsax, dsay, dir) \
  ((u64)(ssax)        | ((u64)(ssay) << 16) | \
  ((u64)(dsax) << 32) | ((u64)(dsay) << 48) | \
  ((u64)(dir) << 59))

#define GS_SETREG_TRXREG(rrw, rrh) \
  ((u64)(rrw) | ((u64)(rrh) << 32))

#define GS_SETREG_UV(u, v) ((u64)(u) | ((u64)(v) << 16))

#define GS_SETREG_STQ(s, t) ((u64)(s) | ((u64)(t) << 32))

#define GS_SETREG_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
  ((u64)(sbp)         | ((u64)(sbw) << 16) | \
  ((u64)(spsm) << 24) | ((u64)(dbp) << 32) | \
  ((u64)(dbw) << 48)  | ((u64)(dpsm) << 56))

#ifdef __cplusplus
extern "C" {
#endif
u32  gsKit_texture_size_ee(int width, int height, int psm);
u32  gsKit_texture_size(int width, int height, int psm);
void gsKit_texture_to_psm16(GSTEXTURE *Texture);
void gsKit_texture_send(u32 *mem, int width, int height, u32 tbp, u32 psm, u32 tbw, u8 clut);
void gsKit_texture_send_inline(GSGLOBAL *gsGlobal, u32 *mem, int width, int height, u32 tbp, u32 psm, u32 tbw, u8 clut);
void gsKit_texture_upload(GSGLOBAL *gsGlobal, GSTEXTURE *Texture);
void gsKit_renderToTexture(GSGLOBAL *gsGlobal, GSTEXTURE *texture);

void gsKit_prim_sprite_texture_3d(GSGLOBAL *gsGlobal, const GSTEXTURE *Texture, float x1, float y1, int iz1, float u1, float v1,
                                                                       float x2, float y2, int iz2, float u2, float v2,
                                                                       u64 color);

void gsKit_prim_sprite_striped_texture_3d(GSGLOBAL *gsGlobal, const GSTEXTURE *Texture, float x1, float y1, int iz1, float u1, float v1,
                                                                       float x2, float y2, int iz2, float u2, float v2,
                                                                       u64 color);

void gskit_prim_list_sprite_texture_uv_3d(GSGLOBAL *gsGlobal, const GSTEXTURE *Texture, int count, const GSPRIMUVPOINT *vertices);
void gskit_prim_list_sprite_texture_uv_flat(GSGLOBAL *gsGlobal, const GSTEXTURE *Texture, int count, const u128 *flatContent);
void gskit_prim_list_sprite_texture_uv_flat_color(GSGLOBAL *gsGlobal, const GSTEXTURE *Texture, gs_rgbaq color, int count, const GSPRIMUVPOINTFLAT *vertices);

void gsKit_prim_triangle_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3, u64 color);

void gsKit_prim_triangle_goraud_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3,
				u64 color1, u64 color2, u64 color3);

void gsKit_prim_list_triangle_goraud_texture_uv_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, int count, const GSPRIMUVPOINT *vertices);
void gsKit_prim_list_triangle_goraud_texture_stq_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture, int count, const GSPRIMSTQPOINT *vertices);

void gsKit_prim_triangle_strip_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriStrip, int segments, int iz, u64 color);

void gsKit_prim_triangle_strip_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriStrip, int segments, u64 color);

void gsKit_prim_triangle_fan_texture(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriFan, int verticies, int iz, u64 color);

void gsKit_prim_triangle_fan_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
					float *TriFan, int verticies, u64 color);

void gsKit_prim_quad_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3,
				float x4, float y4, int iz4, float u4, float v4, u64 color);

void gsKit_prim_quad_goraud_texture_3d(GSGLOBAL *gsGlobal, GSTEXTURE *Texture,
				float x1, float y1, int iz1, float u1, float v1,
				float x2, float y2, int iz2, float u2, float v2,
				float x3, float y3, int iz3, float u3, float v3,
				float x4, float y4, int iz4, float u4, float v4,
				u64 color1, u64 color2, u64 color3, u64 color4);


#define gsKit_prim_sprite_texture(gsGlobal, Texture,	x1, y1, u1, v1,		\
							x2, y2, u2, v2,		\
							z, color)		\
	gsKit_prim_sprite_texture_3d(gsGlobal, Texture, x1, y1, z, u1, v1,	\
					 		x2, y2, z, u2, v2, color);

#define gsKit_prim_sprite_striped_texture(gsGlobal, Texture,	x1, y1, u1, v1,		\
							x2, y2, u2, v2,		\
							z, color)		\
	gsKit_prim_sprite_striped_texture_3d(gsGlobal, Texture, x1, y1, z, u1, v1,	\
					 		x2, y2, z, u2, v2, color);

#define gsKit_prim_triangle_texture(gsGlobal, Texture,	x1, y1, u1, v1,		\
							x2, y2, u2, v2,		\
							x3, y3, u3, v3,		\
							z, color)		\
	gsKit_prim_triangle_texture_3d(gsGlobal, Texture, x1, y1, z, u1, v1,	\
							x2, y2, z, u2, v2,	\
							x3, y3, z, u3, v3, color);

#define gsKit_prim_triangle_goraud_texture(gsGlobal, Texture,	x1, y1, u1, v1,		\
							x2, y2, u2, v2,		\
							x3, y3, u3, v3,		\
							z, color1, color2, color3)		\
	gsKit_prim_triangle_goraud_texture_3d(gsGlobal, Texture, x1, y1, z, u1, v1,	\
							x2, y2, z, u2, v2,	\
							x3, y3, z, u3, v3, color1, color2, color3);

#define gsKit_prim_quad_texture(gsGlobal, Texture,	x1, y1, u1, v1,		\
							x2, y2, u2, v2,		\
							x3, y3, u3, v3,		\
							x4, y4, u4, v4,		\
							z, color)		\
	gsKit_prim_quad_texture_3d(gsGlobal, Texture, x1, y1, z, u1, v1,	\
							x2, y2, z, u2, v2,	\
							x3, y3, z, u3, v3,	\
					 		x4, y4, z, u4, v4, color);

#define gsKit_prim_quad_goraud_texture(gsGlobal, Texture,	x1, y1, u1, v1,		\
							x2, y2, u2, v2,		\
							x3, y3, u3, v3,		\
							x4, y4, u4, v4,		\
							z, color1, color2, color3, color4)		\
	gsKit_prim_quad_goraud_texture_3d(gsGlobal, Texture, x1, y1, z, u1, v1,	\
							x2, y2, z, u2, v2,	\
							x3, y3, z, u3, v3,	\
					 		x4, y4, z, u4, v4, color1, color2, color3, color4);

#ifdef __cplusplus
}
#endif

#endif /* __GSTEXTURE_H__ */
