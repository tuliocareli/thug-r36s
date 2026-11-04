#if defined(__aarch64__)
	.text
	.align 4

#define STUB(sym) \
	.global sym; \
	.type sym, %function; \
sym: \
	mov x0, #0; \
	fmov s0, wzr; \
	ret; \
	.size sym, .-sym

STUB(_Z13MemViewTogglev)
STUB(_ZN2Nx12plat_shatterEPNS_5CGeomE)
STUB(_ZN2Nx18plat_texture_splatEPPNS_7CSectorEPPNS_11CCollStaticERN3Mth6VectorES8_ffPNS_8CTextureEPNS_26sSplatTrailInstanceDetailsE)
STUB(_ZN2Nx19plat_shatter_renderEPNS_23sShatterInstanceDetailsE)
STUB(_ZN2Nx19plat_shatter_updateEPNS_23sShatterInstanceDetailsEf)
STUB(_ZN2Nx20plat_shatter_cleanupEv)
STUB(_ZN2Nx23plat_shatter_initializeEv)
STUB(_ZN2Nx24plat_screen_flash_renderEPNS_19sScreenFlashDetailsE)
STUB(_ZN2Nx25plat_texture_splat_renderEv)
STUB(_ZN2Nx26plat_texture_splat_cleanupEv)
STUB(_ZN2Nx29plat_texture_splat_initializeEv)
STUB(_ZN2Nx29plat_texture_splat_reset_polyEPNS_21sSplatInstanceDetailsEi)
STUB(_ZN3SIO12KeyboardReadEPc)
STUB(_ZN3SIO13KeyboardClearEv)
STUB(_ZN3Tmr14GetRenderFrameEv)

	.data
	.align 4
	.global gbQuit
gbQuit:
	.word 0
	.size gbQuit, 4

	.global gun_fired
gun_fired:
	.word 0
	.size gun_fired, 4

	.global screen_angle
screen_angle:
	.word 0
	.size screen_angle, 4

#else
@ ARM 32-bit Thumb fallback
	.text
	.syntax unified
	.thumb
	.global _Z13MemViewTogglev
	.thumb_func
	.type _Z13MemViewTogglev, %function
_Z13MemViewTogglev:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _Z13MemViewTogglev, .-_Z13MemViewTogglev

	.global _ZN2Nx12plat_shatterEPNS_5CGeomE
	.thumb_func
	.type _ZN2Nx12plat_shatterEPNS_5CGeomE, %function
_ZN2Nx12plat_shatterEPNS_5CGeomE:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx12plat_shatterEPNS_5CGeomE, .-_ZN2Nx12plat_shatterEPNS_5CGeomE

	.global _ZN2Nx18plat_texture_splatEPPNS_7CSectorEPPNS_11CCollStaticERN3Mth6VectorES8_ffPNS_8CTextureEPNS_26sSplatTrailInstanceDetailsE
	.thumb_func
	.type _ZN2Nx18plat_texture_splatEPPNS_7CSectorEPPNS_11CCollStaticERN3Mth6VectorES8_ffPNS_8CTextureEPNS_26sSplatTrailInstanceDetailsE, %function
_ZN2Nx18plat_texture_splatEPPNS_7CSectorEPPNS_11CCollStaticERN3Mth6VectorES8_ffPNS_8CTextureEPNS_26sSplatTrailInstanceDetailsE:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx18plat_texture_splatEPPNS_7CSectorEPPNS_11CCollStaticERN3Mth6VectorES8_ffPNS_8CTextureEPNS_26sSplatTrailInstanceDetailsE, .-_ZN2Nx18plat_texture_splatEPPNS_7CSectorEPPNS_11CCollStaticERN3Mth6VectorES8_ffPNS_8CTextureEPNS_26sSplatTrailInstanceDetailsE

	.global _ZN2Nx19plat_shatter_renderEPNS_23sShatterInstanceDetailsE
	.thumb_func
	.type _ZN2Nx19plat_shatter_renderEPNS_23sShatterInstanceDetailsE, %function
_ZN2Nx19plat_shatter_renderEPNS_23sShatterInstanceDetailsE:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx19plat_shatter_renderEPNS_23sShatterInstanceDetailsE, .-_ZN2Nx19plat_shatter_renderEPNS_23sShatterInstanceDetailsE

	.global _ZN2Nx19plat_shatter_updateEPNS_23sShatterInstanceDetailsEf
	.thumb_func
	.type _ZN2Nx19plat_shatter_updateEPNS_23sShatterInstanceDetailsEf, %function
_ZN2Nx19plat_shatter_updateEPNS_23sShatterInstanceDetailsEf:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx19plat_shatter_updateEPNS_23sShatterInstanceDetailsEf, .-_ZN2Nx19plat_shatter_updateEPNS_23sShatterInstanceDetailsEf

	.global _ZN2Nx20plat_shatter_cleanupEv
	.thumb_func
	.type _ZN2Nx20plat_shatter_cleanupEv, %function
_ZN2Nx20plat_shatter_cleanupEv:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx20plat_shatter_cleanupEv, .-_ZN2Nx20plat_shatter_cleanupEv

	.global _ZN2Nx23plat_shatter_initializeEv
	.thumb_func
	.type _ZN2Nx23plat_shatter_initializeEv, %function
_ZN2Nx23plat_shatter_initializeEv:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx23plat_shatter_initializeEv, .-_ZN2Nx23plat_shatter_initializeEv

	.global _ZN2Nx24plat_screen_flash_renderEPNS_19sScreenFlashDetailsE
	.thumb_func
	.type _ZN2Nx24plat_screen_flash_renderEPNS_19sScreenFlashDetailsE, %function
_ZN2Nx24plat_screen_flash_renderEPNS_19sScreenFlashDetailsE:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx24plat_screen_flash_renderEPNS_19sScreenFlashDetailsE, .-_ZN2Nx24plat_screen_flash_renderEPNS_19sScreenFlashDetailsE

	.global _ZN2Nx25plat_texture_splat_renderEv
	.thumb_func
	.type _ZN2Nx25plat_texture_splat_renderEv, %function
_ZN2Nx25plat_texture_splat_renderEv:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx25plat_texture_splat_renderEv, .-_ZN2Nx25plat_texture_splat_renderEv

	.global _ZN2Nx26plat_texture_splat_cleanupEv
	.thumb_func
	.type _ZN2Nx26plat_texture_splat_cleanupEv, %function
_ZN2Nx26plat_texture_splat_cleanupEv:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx26plat_texture_splat_cleanupEv, .-_ZN2Nx26plat_texture_splat_cleanupEv

	.global _ZN2Nx29plat_texture_splat_initializeEv
	.thumb_func
	.type _ZN2Nx29plat_texture_splat_initializeEv, %function
_ZN2Nx29plat_texture_splat_initializeEv:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx29plat_texture_splat_initializeEv, .-_ZN2Nx29plat_texture_splat_initializeEv

	.global _ZN2Nx29plat_texture_splat_reset_polyEPNS_21sSplatInstanceDetailsEi
	.thumb_func
	.type _ZN2Nx29plat_texture_splat_reset_polyEPNS_21sSplatInstanceDetailsEi, %function
_ZN2Nx29plat_texture_splat_reset_polyEPNS_21sSplatInstanceDetailsEi:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN2Nx29plat_texture_splat_reset_polyEPNS_21sSplatInstanceDetailsEi, .-_ZN2Nx29plat_texture_splat_reset_polyEPNS_21sSplatInstanceDetailsEi

	.global _ZN3SIO12KeyboardReadEPc
	.thumb_func
	.type _ZN3SIO12KeyboardReadEPc, %function
_ZN3SIO12KeyboardReadEPc:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN3SIO12KeyboardReadEPc, .-_ZN3SIO12KeyboardReadEPc

	.global _ZN3SIO13KeyboardClearEv
	.thumb_func
	.type _ZN3SIO13KeyboardClearEv, %function
_ZN3SIO13KeyboardClearEv:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN3SIO13KeyboardClearEv, .-_ZN3SIO13KeyboardClearEv

	.global _ZN3Tmr14GetRenderFrameEv
	.thumb_func
	.type _ZN3Tmr14GetRenderFrameEv, %function
_ZN3Tmr14GetRenderFrameEv:
	movs	r0, #0
	vmov	s0, r0
	bx	lr
	.size _ZN3Tmr14GetRenderFrameEv, .-_ZN3Tmr14GetRenderFrameEv

	.data
	.global gbQuit
gbQuit:
	.word 0
	.size gbQuit, 4

	.global gun_fired
gun_fired:
	.word 0
	.size gun_fired, 4

	.global screen_angle
screen_angle:
	.word 0
	.size screen_angle, 4
#endif
