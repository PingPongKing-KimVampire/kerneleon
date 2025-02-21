#include <stdint.h>
#include "print42.h"
#include "print_stack.h"
#include "gdt.h"

// 세그먼트 디스크립터의 필드를 설정하기 위한 매크로들
#define SEG_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)
#define SEG_PRES(x)      ((x) << 0x07) // Present
#define SEG_SAVL(x)      ((x) << 0x0C) // Available for system use
#define SEG_LONG(x)      ((x) << 0x0D) // Long mode
#define SEG_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define SEG_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define SEG_PRIV(x)     (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
 
// 세그먼트 타입 별 권한을 정의하는 상수들
#define SEG_DATA_RD        0x00 // Read-Only
#define SEG_DATA_RDA       0x01 // Read-Only, accessed
#define SEG_DATA_RDWR      0x02 // Read/Write
#define SEG_DATA_RDWRA     0x03 // Read/Write, accessed
#define SEG_DATA_RDEXPD    0x04 // Read-Only, expand-down
#define SEG_DATA_RDEXPDA   0x05 // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD  0x06 // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX        0x08 // Execute-Only
#define SEG_CODE_EXA       0x09 // Execute-Only, accessed
#define SEG_CODE_EXRD      0x0A // Execute/Read
#define SEG_CODE_EXRDA     0x0B // Execute/Read, accessed
#define SEG_CODE_EXC       0x0C // Execute-Only, conforming
#define SEG_CODE_EXCA      0x0D // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC     0x0E // Execute/Read, conforming
#define SEG_CODE_EXRDCA    0x0F // Execute/Read, conforming, accessed
 
// 각 세그먼트에 대해 엔트리의 플래그나 속성을 설정하는 매크로
#define GDT_CODE_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(0)     | SEG_DATA_RDWR
 
#define GDT_STACK_PL0 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                        SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(0)     | SEG_DATA_RDWR

#define GDT_CODE_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_CODE_EXRD
 
#define GDT_DATA_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
                     SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
                     SEG_PRIV(3)     | SEG_DATA_RDWR

#define GDT_STACK_PL3 SEG_DESCTYPE(1) | SEG_PRES(1) | SEG_SAVL(0) | \
					 SEG_LONG(0)     | SEG_SIZE(1) | SEG_GRAN(1) | \
					 SEG_PRIV(3)     | SEG_DATA_RDWR


// 세그먼트 디스크립터 생성 함수
// 세그먼트의 기본 정보 base, limit, flag 를 인자로 받음
uint64_t create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
    uint64_t descriptor; // 세그먼트 디스크립터를 저장할 변수
 
    // 상위 32비트 생성
    descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
    descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
    descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
    descriptor |=  base        & 0xFF000000;         // set base bits 31:24
 
    // 상위 32비트 완료 후, 하위 32비트로 이동
    descriptor <<= 32;
 
    // 하위 32비트 생성
    descriptor |= base  << 16;                       // set base bits 15:0
    descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0

    return descriptor;
}

void kernel_main(void) 
{
    uint64_t *gdt = (uint64_t *)0x800;

    gdt[0] = create_descriptor(0, 0, 0); // 널 세그먼트 생성
    gdt[1] = create_descriptor(0x00000000, 0x00FFFFFF, (GDT_CODE_PL0)); 
    gdt[2] = create_descriptor(0x00000000, 0x00FFFFFF, (GDT_DATA_PL0)); 
	gdt[3] = create_descriptor(0x00000000, 0x00FFFFFF, (GDT_STACK_PL0));
    gdt[4] = create_descriptor(0x00000000, 0x00FFFFFF, (GDT_CODE_PL3)); 
    gdt[5] = create_descriptor(0x00000000, 0x00FFFFFF, (GDT_DATA_PL3)); 
    gdt[6] = create_descriptor(0x00000000, 0x00FFFFFF, (GDT_STACK_PL3));
 
    uint16_t limit  = 56;
    uint32_t base   = (uint32_t)gdt;
    uint32_t offset = 0;

    setGdt(limit, base, offset);
	reloadSegments();
	print_stack();
}
