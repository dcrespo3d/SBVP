void wait_for_vretrace(void)
{
	asm{
		MOV DX,0x3DA
	}
	ET1:
	asm{
		IN AL,DX
		AND AL,8
		JNZ ET1
	}
	ET2:
	asm{
		IN AL,DX
		AND AL,8
		JZ ET2
	}
}
