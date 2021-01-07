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

#include "gdt.hh"

#define MAX_GDT_ENTRIES 64

namespace gdt {
    static segment gdt_entries[MAX_GDT_ENTRIES];

    void reload_table() {
        // Load the global descriptor table on the cpu register
        uint32 gdt_register[2];
        gdt_register[0] = reinterpret_cast<uint32>(gdt_entries);
        gdt_register[1] = sizeof(segment) * MAX_GDT_ENTRIES;
        asm volatile("lgdt (%0)" :: "p" (reinterpret_cast<uint8*>(gdt_register) + 2));
    }

    void set_entry(uint16 i, uint32 base, uint32 limit, gdt::flags flags) {
        uint16 flag_int = static_cast<uint16>(flags);

        gdt_entries[i] = limit & 0xFFFLL;
        gdt_entries[i] |= (base & 0xFFFFFFLL) << 16;
        gdt_entries[i] |= (flag_int & 0xFFLL) << 40;
        gdt_entries[i] |= ((limit >> 16) & 0xFLL) << 48;
        gdt_entries[i] |= ((flag_int >> 8) & 0xFFLL) << 52;
        gdt_entries[i] |= ((base >> 24) & 0xFFLL) << 56;
    }

    segment get_entry(uint16 i) {
        return gdt_entries[i];
    }

    uint16 get_entry_offset(uint16 i) {
        return reinterpret_cast<uint8*>(gdt_entries[i]) - reinterpret_cast<uint8*>(gdt_entries);
    }
}