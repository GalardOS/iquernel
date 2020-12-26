/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

namespace io {

inline uint8 in8(uint16 port)
{
    uint8 value;
    asm volatile("inb %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

inline uint16 in16(uint16 port)
{
    uint16 value;
    asm volatile("inw %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

inline uint32 in32(uint16 port)
{
    uint32 value;
    asm volatile("inl %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

inline void repeated_in16(uint16 port, uint16* buffer, int buffer_size)
{
    asm volatile("rep insw"
                 : "+D"(buffer), "+c"(buffer_size)
                 : "d"(port)
                 : "memory");
}

inline void out8(uint16 port, uint8 value)
{
    asm volatile("outb %0, %1" ::"a"(value), "Nd"(port));
}

inline void out16(uint16 port, uint16 value)
{
    asm volatile("outw %0, %1" ::"a"(value), "Nd"(port));
}

inline void out32(uint16 port, uint32 value)
{
    asm volatile("outl %0, %1" ::"a"(value), "Nd"(port));
}

inline void repeated_out16(uint16 port, const uint16* data, int data_size)
{
    asm volatile("rep outsw"
                 : "+S"(data), "+c"(data_size)
                 : "d"(port));
}

inline void delay(uint32 microseconds)
{
    for (uint32 i = 0; i < microseconds; ++i)
        io::in8(0x80);
}

}