; This file is for the FamiStudio Sound Engine and was generated by FamiStudio


.if FAMISTUDIO_CFG_C_BINDINGS
.export _sounds=sounds
.endif

sounds:
	.word @ntsc
	.word @ntsc
@ntsc:
	.word @sfx_ntsc_jump

@sfx_ntsc_jump:
	.byte $85,$01,$84,$7c,$83,$bf,$89,$f0,$05,$84,$6a,$83,$7f,$01,$84,$58
	.byte $01,$84,$46,$01,$84,$34,$01,$84,$22,$01,$84,$10,$01,$85,$00,$84
	.byte $fe,$01,$84,$ec,$01,$84,$da,$01,$84,$c9,$01,$00

.export sounds