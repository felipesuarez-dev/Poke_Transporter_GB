#include <tonc.h>

#define pkmn_size(gen == 1 ? 44 : 48)
#define name_size            11

byte gen_one_pkmn_index[255];
byte gen_two_pkmn_index[255];


class Pokemon{
public:

    void Pokemon(byte gen, int index, byte party_data[441]){
        num_of_pkmn = party_data[0x11];

        int party_offset = 20 + (index * 48);
        int ot_offset    = 20 + (6 * pkmn_size) + (index * name_size);
        int name_offset  = 20 + (6 * pkmn_size) + (6 * name_size) + (index * name_size);
            
        switch(gen){
            case 1:
          

            break;
            case 2:
                species_index = gen_one_pkmn_index[party_data[party_offset + 0x00];
                std::copy(party_data[party_offset + 0x02], party_data[party_offset + 0x05], moves[0]);
                std::copy(party_data[party_offset + 0x06], party_data[party_offset + 0x07], trainer_id[0];
                exp[3]; //??
                std::copy(party_data[name_offset + 0x00], party_data[name_offset + 0x0A], nickname[0]);
                std::copy(party_data[ot_offset + 0x00], party_data[name_offset + 0x0A], trainer_name[11]);
                pokerus = party_data[party_offset + 0x1C];
                std::copy(party_data[party_offset + 0x1D], party_data[party_offset + 0x1C], caught_data[0]);
                level; //??
            break;
        
    }

    void convert_to_gen_three(){
        
        {
        
private:
    byte gen;
    byte species_index;
    byte moves[4];
    byte trainer_id[2];
    byte exp[3];
    byte nickname[11];
    byte trainer_name[11];
    byte pokerus;
    byte caught_data[2];
    byte level;
    byte gen_3_pkmn[80];

    }
};