#include <tonc.h>
#include "mirror.h"
#include "gba_flash.h"

vu32 newest_save_offset = MEM_CRAM + SAVE_A_OFFSET;

vu32 memory_section_array[14] = {};
u8 memory_buffer[0x1000];
char mem_name = 'A';

vu8 pkmn_length = 80;

// Fills the variables with the current offset information
void initalize_memory_locations(){
    u8 save_A_index[4];
    u8 save_B_index[4];
    flash_read(MEM_CRAM + SAVE_A_OFFSET + SAVE_INDEX_OFFSET, &save_A_index[0], 0x04);
    flash_read(MEM_CRAM + SAVE_B_OFFSET + SAVE_INDEX_OFFSET, &save_B_index[0], 0x04);
    reverse_endian(&save_A_index[0], 0x04);
    reverse_endian(&save_B_index[0], 0x04);
    
    // Determines if save A or B is more recent
    if(*(vu32*)save_B_index > *(vu32*)save_A_index){
        newest_save_offset = MEM_CRAM + SAVE_B_OFFSET;
        mem_name = 'B';
    }

    // Populates the memory_section_array with the correct pointer locations
    vu8 mem_id = *(vu8*)(newest_save_offset + SECTION_ID_OFFSET);
    for (int i = 0; i < 14; i++){
        memory_section_array[mem_id] = newest_save_offset + (i * 0x1000);
        mem_id = (mem_id + 1) % 14;
    }


    //for (vu32 i = 0; i < 14; i++){
    //    memory_section_array[*(vu8*)(newest_save_offset + (i * 0x1000) + SECTION_ID_OFFSET)] = newest_save_offset + (i * 0x1000);
    //}
}

// Reverses the endian of the given array
void reverse_endian(u8 *data, size_t size){
    u8 temp;
    for (int i = 0; i < (size/2); i++){
        temp = data[i];
        data[i] = data[(size - 1) - i];
        data[(size - 1) - i] = temp;
    }
}

// Inserts "total_num" Pokemon from the party array
bool insert_pokemon(vu8 party_array[], int total_num){
    int pkmn_num = 0;
    int mem_section = 5;
    int byte_location = 4;
    int pkmn_byte = 0;
    bool write_pkmn = false;

    flash_read(memory_section_array[mem_section], &memory_buffer[0], 0x1000);

    while(pkmn_num < total_num){
        // Checks if we are currently beyond PC box information bounds. If so, return false
        if ((mem_section == 13) && (byte_location >= 1860)){
            update_memory_buffer_checksum();
            flash_write(memory_section_array[mem_section], &memory_buffer[0], 0x1000);
            return false;
        }

        // Determines if there is space to write a Pokemon, or if a Pokemon is currently being written
        write_pkmn = (write_pkmn | (memory_buffer[byte_location] == 0x00));
        
        // Writes a byte of the current Pokemon to the current spot
        if (write_pkmn){
            memory_buffer[byte_location] = party_array[(pkmn_length * pkmn_num) + pkmn_byte];
            byte_location++;
            pkmn_byte++;
            // Determines if the whole Pokemon has been written
            if (pkmn_byte == pkmn_length){
                pkmn_byte = 0x00;
                write_pkmn = false;
                pkmn_num++;
            }
        } else {
            byte_location += pkmn_length;
        }
        // If at any point we are beyond the bounds of the current memory section,
        // we move to the next memory buffer and save the current one
        if (byte_location >= 3968){
            update_memory_buffer_checksum();
            flash_write(memory_section_array[mem_section], &memory_buffer[0], 0x1000);
            mem_section++;
            flash_read(memory_section_array[mem_section], &memory_buffer[0], 0x1000);
            byte_location = byte_location % 3968;
        }
    }
    update_memory_buffer_checksum();
    flash_write(memory_section_array[mem_section], &memory_buffer[0], 0x1000);
    return true;
}

// Updates the checksum in the current memory buffer to match what is expected
void update_memory_buffer_checksum(){
    vu32 checksum = 0x00;

    vu32 num_of_bytes = 3968;
    if (memory_buffer[0x0FF4] == 13){
        num_of_bytes = 2000;
    }

    for (int i = 0; i < num_of_bytes / 4; i++){
        checksum += (memory_buffer[(4*i) + 3] << 24) | (memory_buffer[(4*i) + 2] << 16) | (memory_buffer[(4*i) + 1] << 8) | (memory_buffer[(4*i) + 0] << 0);
    }

    vu16 small_checksum = ((checksum & 0xFFFF0000) >> 16) + (checksum & 0x0000FFFF);
    memory_buffer[0x0FF6] = small_checksum & 0x00FF;
    memory_buffer[0x0FF7] = (small_checksum & 0xFF00) >> 8;
}