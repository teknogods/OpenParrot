#pragma once
void reset_addressed();
unsigned long process_stream(unsigned char *stream, unsigned long srcsize, unsigned char *dst, unsigned long dstsize);
unsigned long process_rfid(unsigned char *stream, unsigned long srcsize, unsigned char *dst, unsigned long dstsize);
void init_RfidEmu(X2Type type);