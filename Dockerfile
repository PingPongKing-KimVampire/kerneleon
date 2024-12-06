FROM ubuntu:22.04

WORKDIR /workspace

RUN apt update && apt install -y grub-pc-bin xorriso

RUN mkdir -p /workspace/isodir/boot/grub


COPY kernel/myos.bin /workspace/isodir/boot/myos.bin
COPY kernel/grub.cfg /workspace/isodir/boot/grub/grub.cfg

CMD ["grub-mkrescue", "-o", "/workspace/image/myos.iso", "isodir"]
