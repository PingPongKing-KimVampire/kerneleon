.section .text
.global reloadSegments

reloadSegments:
	ljmp $0x08, $.reload_CS

.reload_CS:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	ret
