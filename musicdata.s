; This file for the FamiStudio Sound Engine and was generated by FamiStudio

.if FAMISTUDIO_CFG_C_BINDINGS
.export _music_data_fullstack=music_data_fullstack
.endif

music_data_fullstack:
	.byte 1
	.word @instruments
	.word @samples-4
	.word @song0ch0,@song0ch1,@song0ch2,@song0ch3,@song0ch4 ; 00 : Main Theme
	.byte .lobyte(@tempo_env_1_mid), .hibyte(@tempo_env_1_mid), 0, 0

.export music_data_fullstack
.global FAMISTUDIO_DPCM_PTR

@instruments:
	.word @env12,@env6,@env8,@env0 ; 00 : Closed Hat
	.word @env13,@env6,@env15,@env0 ; 01 : Lead
	.word @env7,@env10,@env8,@env0 ; 02 : Noise Kick
	.word @env14,@env6,@env8,@env0 ; 03 : Noise Snare
	.word @env11,@env6,@env8,@env0 ; 04 : Open Hat
	.word @env2,@env6,@env9,@env4 ; 05 : Square Bass
	.word @env5,@env1,@env8,@env0 ; 06 : Tri Kick
	.word @env5,@env3,@env8,@env0 ; 07 : Tri Snare

@samples:

@env0:
	.byte $00,$c0,$7f,$00,$02
@env1:
	.byte $c0,$be,$bc,$ba,$b8,$00,$04
@env2:
	.byte $00,$c6,$7f,$00,$02
@env3:
	.byte $c0,$bf,$be,$bd,$bc,$00,$04
@env4:
	.byte $00,$c5,$7f,$00,$02
@env5:
	.byte $00,$cf,$7f,$00,$02
@env6:
	.byte $c0,$7f,$00,$01
@env7:
	.byte $00,$cf,$cd,$cb,$c9,$c7,$00,$05
@env8:
	.byte $7f,$00,$00
@env9:
	.byte $c1,$7f,$00,$00
@env10:
	.byte $ba,$c0,$00,$01
@env11:
	.byte $00,$ca,$c9,$c8,$c7,$c7,$c6,$c6,$c5,$c5,$c3,$c2,$c1,$c0,$00,$0d
@env12:
	.byte $00,$ca,$c9,$c4,$c3,$c2,$c1,$c1,$c0,$00,$08
@env13:
	.byte $00,$cc,$cb,$ca,$c9,$00,$04
@env14:
	.byte $00,$cd,$ca,$c7,$c4,$00,$04
@env15:
	.byte $c2,$c2,$c1,$00,$02

@tempo_env_1_mid:
	.byte $03,$05,$80

@song0ch0:
@song0ch0loop:
	.byte $46, .lobyte(@tempo_env_1_mid), .hibyte(@tempo_env_1_mid), $ff, $9f, $47, $ff, $9f, $47, $ff, $9f, $47, $ff, $9f, $47
	.byte $82
@song0ref18:
	.byte $20, $91, $00, $a5, $1e, $87, $20, $87, $27, $cd
@song0ref28:
	.byte $47, $bb, $20, $87, $23, $87, $25, $91, $23, $91, $22, $91, $20, $91, $47, $23, $91, $00, $91, $2a, $91, $27, $e1
	.byte $41, $14
	.word @song0ref28
@song0ref54:
	.byte $cd, $20, $87, $23, $87, $47, $25, $b9, $25, $87, $27, $87, $25, $91, $23, $91, $22, $91, $20, $87, $1e, $87, $47, $20
	.byte $ff, $89, $43, $4f, $3a, $2c, $20, $91, $47, $00, $ff, $9d, $47
	.byte $41, $1f
	.word @song0ref18
	.byte $41, $14
	.word @song0ref28
	.byte $41, $1d
	.word @song0ref54
	.byte $42
	.word @song0ch0loop
@song0ch1:
@song0ch1loop:
@song0ref104:
	.byte $8a
@song0ref105:
	.byte $08, $8f, $00, $08, $8f, $00, $93
@song0ref112:
	.byte $08, $8f, $00, $93, $08, $8f, $00, $06, $8f, $00, $07, $8f, $00
	.byte $41, $14
	.word @song0ref105
	.byte $41, $14
	.word @song0ref105
	.byte $41, $14
	.word @song0ref105
	.byte $41, $14
	.word @song0ref105
	.byte $41, $11
	.word @song0ref105
@song0ref140:
	.byte $0b, $8f, $00, $0f, $8f, $00, $0f, $8f, $00, $93
@song0ref150:
	.byte $0f, $8f, $00, $93, $0f, $8f, $00, $0d, $8f, $00
	.byte $41, $14
	.word @song0ref140
@song0ref163:
	.byte $0b
@song0ref164:
	.byte $8f, $00, $0d, $8f, $00, $0d, $8f, $00, $93, $0d, $8f, $00, $93, $0d, $8f, $00, $0d, $8f, $00, $0f
	.byte $41, $10
	.word @song0ref164
	.byte $0b, $8f, $00, $0a, $8f, $00
	.byte $41, $14
	.word @song0ref105
	.byte $41, $14
	.word @song0ref105
	.byte $41, $0a
	.word @song0ref105
	.byte $82, $23, $cd, $bb, $8a
	.byte $41, $0a
	.word @song0ref112
	.byte $41, $0a
	.word @song0ref140
	.byte $82, $23, $e1, $bb, $8a
	.byte $41, $0a
	.word @song0ref150
	.byte $41, $0a
	.word @song0ref163
	.byte $82, $23, $cd, $8a, $0f
	.byte $41, $10
	.word @song0ref164
	.byte $0b, $8f, $00, $0a, $8f, $00, $82, $1b, $ff, $89, $43, $4f, $4d, $27, $1b, $91
	.byte $41, $14
	.word @song0ref104
	.byte $42
	.word @song0ch1loop
@song0ch2:
@song0ch2loop:
@song0ref255:
	.byte $8c, $1d, $87, $82, $14, $85, $00, $14, $8f, $00, $8e, $22, $87, $00, $87, $82, $14, $87, $8e, $22, $87, $00, $91, $8c
	.byte $1d, $87, $82, $14, $85, $00, $8e, $22, $87, $00, $81, $82, $12, $81, $00, $13, $8f, $00
	.byte $41, $1e
	.word @song0ref255
	.byte $8c, $1d, $87, $82, $13, $85, $00
	.byte $41, $21
	.word @song0ref255
	.byte $41, $1e
	.word @song0ref255
	.byte $8c, $1d, $87, $82, $13, $85, $00
	.byte $41, $21
	.word @song0ref255
	.byte $41, $1e
	.word @song0ref255
@song0ref326:
	.byte $8c, $1d, $87, $82, $17, $85
@song0ref332:
	.byte $00, $8c, $1d, $87, $82, $1b, $85, $00, $1b, $8f, $00, $8e, $22, $87, $00, $87, $82, $1b, $87, $8e, $22, $87, $00, $91
	.byte $8c, $1d, $87, $82, $1b
@song0ref361:
	.byte $85, $00, $8e, $22, $87, $00, $81, $82, $19, $81, $00, $17, $8f
	.byte $41, $1f
	.word @song0ref332
@song0ref377:
	.byte $8c, $1d, $87, $82, $17, $85
@song0ref383:
	.byte $00, $8c, $1d, $87, $82, $19, $85, $00, $19, $8f, $00, $8e, $22, $87, $00, $87, $82, $19, $87, $8e, $22, $87, $00, $91
	.byte $8c, $1d, $87, $82, $19
	.byte $41, $09
	.word @song0ref361
	.byte $1b, $8f
	.byte $41, $16
	.word @song0ref383
@song0ref420:
	.byte $85, $00, $8e, $22, $87, $00, $81, $82, $17, $81, $00, $8c, $1d, $87, $82, $16, $85, $00
	.byte $41, $21
	.word @song0ref255
	.byte $41, $1e
	.word @song0ref255
	.byte $8c, $1d, $87, $82, $13, $85, $00
	.byte $41, $21
	.word @song0ref255
	.byte $41, $1e
	.word @song0ref255
	.byte $41, $25
	.word @song0ref326
	.byte $41, $1f
	.word @song0ref332
	.byte $41, $1a
	.word @song0ref377
	.byte $41, $09
	.word @song0ref361
	.byte $1b, $8f
	.byte $41, $16
	.word @song0ref383
	.byte $41, $0e
	.word @song0ref420
	.byte $41, $21
	.word @song0ref255
	.byte $41, $1e
	.word @song0ref255
	.byte $8c, $1d, $87, $82, $13, $85, $00, $42
	.word @song0ch2loop
@song0ch3:
@song0ch3loop:
@song0ref494:
	.byte $84
@song0ref495:
	.byte $22, $87, $00, $87, $80, $20, $8b, $00, $83, $86, $1b, $8b, $00, $83, $80, $20, $87, $86, $1b, $8b, $00, $83, $80, $20
	.byte $87, $84, $22, $87, $00, $87, $86, $1b, $8b, $00, $83, $88, $22, $91
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87
	.byte $41, $1e
	.word @song0ref495
	.byte $41, $1c
	.word @song0ref494
	.byte $84, $22, $87, $00, $87, $42
	.word @song0ch3loop
@song0ch4:
@song0ch4loop:
@song0ref644:
	.byte $ff, $9f, $ff, $9f, $ff, $9f, $ff, $9f, $ff, $9f
	.byte $41, $0a
	.word @song0ref644
	.byte $41, $0a
	.word @song0ref644
	.byte $41, $0a
	.word @song0ref644
	.byte $42
	.word @song0ch4loop
