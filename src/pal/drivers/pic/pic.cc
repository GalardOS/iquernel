/*
 * This project is provided under the GNU GPL v2 license, more information can 
 * found on https://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    - Iker Galardi
 */

#include "pal/drivers/pic/pic.hh"

#include "pal/drivers/gdt/gdt.hh"

#include "pal/cpu.hh"

#define KERNEL_CODE_SEGMENT 1
#define KERNEL_DATA_SEGMENT 2
#define USER_CODE_SEGMENT   3
#define USER_DATA_SEGMENT   4

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_MASTER_IMR 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_SLAVE_IMR 0xA1

#define PIC_COMMAND_EOI 0x20

extern "C" void local_interrupt_ignore();

namespace pic {

    struct idt_structure {
        uint16 address_low_bits;
        uint16 code_segment_selector;
        uint8 reserved;
        uint8 access;
        uint16 address_high_bits;
    } __attribute__((packed));

    pic::idt_structure descriptors[256];
    
    void initialize() {
        // Setup global descriptor table
        gdt::set_entry(0, 0, 0, GDT_FLAG_NULL);
        gdt::set_entry(1, 0, 0xFFFFF, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT); 
        gdt::set_entry(2, 0, 0xFFFFF, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT);
        gdt::set_entry(3, 0, 0xFFFFF, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
        gdt::set_entry(4, 0, 0xFFFFF, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
        gdt::reload_table();
        
        // Setup all the descriptors as ignore when not present
        for(uint16 i = 0; i < 256; i++) {
            set_ignore_entry(i);
        }

        // Send initialization command to both PICs
        pal::cpu::ports::out8(PIC_MASTER_COMMAND, 0x11);
        pal::cpu::ports::out8(PIC_SLAVE_COMMAND, 0x11);

        // Map interrupts to certain IRQ numbers
        pal::cpu::ports::out8(PIC_MASTER_DATA, 0x20);
        pal::cpu::ports::out8(PIC_SLAVE_DATA, 0x20 + 8);
        
        // Specify how the master/slave pics will comunicate (IRQ2)
        // Master takes a bit mask with the relevant bit enabled and
        // slave takes the number directly (very strange)
        pal::cpu::ports::out8(PIC_MASTER_DATA, BIT(2 + 1));
        pal::cpu::ports::out8(PIC_SLAVE_DATA, 2);

        // Indicate PICs we are in 8086 (whatever that means)
        pal::cpu::ports::out8(PIC_MASTER_DATA, 0x01);
        pal::cpu::ports::out8(PIC_SLAVE_DATA, 0x01);

        pic::reload_idt();
   }

    void set_entry(pic::int_descriptor entry) {
        // Set handler address
        descriptors[entry.int_number].address_low_bits = ((uint32)entry.handler) & 0xFFFF;
        descriptors[entry.int_number].address_high_bits = ((uint32)entry.handler >> 16) & 0xFFFF;
        
        // Set the code segment selector
        descriptors[entry.int_number].code_segment_selector = gdt::get_entry_offset(KERNEL_CODE_SEGMENT);
        
        // Reserved byte NEEDS to be 0
        descriptors[entry.int_number].reserved = 0;

        // Sets the flags for entry access (0x80 flag sets the descriptor as present)
        descriptors[entry.int_number].access = 0x80 | (uint8)entry.descriptor_type | ((entry.priviledge_level & 3) << 5);
    }

    /// TODO: maybe remove the entry from IDT instead of empty handler
    void set_ignore_entry(uint8 int_number) {
            pic::int_descriptor descriptor;
            descriptor.int_number = int_number;
            descriptor.handler = &local_interrupt_ignore;
            descriptor.priviledge_level = 0;                                                // Priviledge level to kernel mode
            descriptor.descriptor_type = pic::desc_type::interrupt;

            // Add the descriptor to IDT
            pic::set_entry(descriptor);
    }

    void reload_idt() {
        struct { uint16 size; uint32 ptr; } idtpr;
        idtpr.size = sizeof(idt_structure) * 256 - 1;
        idtpr.ptr = reinterpret_cast<uint32>(descriptors);
        asm volatile("lidt %0" :: "m"(idtpr));
    }

    void enable() {
        asm("sti");
    }

    void disable() {
        asm("cli");
    }

}