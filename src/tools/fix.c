/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#pragma pack(push, 1)
typedef struct {
   u32 entry_point;
   u8 logo[156];
   u8 game_title[12];
   u32 game_code;
   u16 maker_code;
   u8 fixed_value;
   u8 unit_code;
   u8 device_type;
   u8 reserved0[7];
   u8 software_version;
   u8 complement;
   u16 reserved1;
} gba_header;
#pragma pack(pop)

static u8 logo[] =
{
   0x24,0xFF,0xAE,0x51,0x69,0x9A,0xA2,0x21,0x3D,0x84,0x82,0x0A,0x84,0xE4,0x09,0xAD,
   0x11,0x24,0x8B,0x98,0xC0,0x81,0x7F,0x21,0xA3,0x52,0xBE,0x19,0x93,0x09,0xCE,0x20,
   0x10,0x46,0x4A,0x4A,0xF8,0x27,0x31,0xEC,0x58,0xC7,0xE8,0x33,0x82,0xE3,0xCE,0xBF,
   0x85,0xF4,0xDF,0x94,0xCE,0x4B,0x09,0xC1,0x94,0x56,0x8A,0xC0,0x13,0x72,0xA7,0xFC,
   0x9F,0x84,0x4D,0x73,0xA3,0xCA,0x9A,0x61,0x58,0x97,0xA3,0x27,0xFC,0x03,0x98,0x76,
   0x23,0x1D,0xC7,0x61,0x03,0x04,0xAE,0x56,0xBF,0x38,0x84,0x00,0x40,0xA7,0x0E,0xFD,
   0xFF,0x52,0xFE,0x03,0x6F,0x95,0x30,0xF1,0x97,0xFB,0xC0,0x85,0x60,0xD6,0x80,0x25,
   0xA9,0x63,0xBE,0x03,0x01,0x4E,0x38,0xE2,0xF9,0xA2,0x34,0xFF,0xBB,0x3E,0x03,0x44,
   0x78,0x00,0x90,0xCB,0x88,0x11,0x3A,0x94,0x65,0xC0,0x7C,0x63,0x87,0xF0,0x3C,0xAF,
   0xD6,0x25,0xE4,0x8B,0x38,0x0A,0xAC,0x72,0x21,0xD4,0xF8,0x07,
};

static char compute_header_complement(gba_header *header)
{
   char c = 0;
   char *p = (char *)header + 0xA0;

   for(int index = 0; index < 0xBD-0xA0; index++)
   {
      c += *p++;
   }

   return -(0x19 + c);
}

int main(int argument_count, char **arguments)
{
   if(argument_count <= 1)
   {
      fprintf(stderr,
              "usage: fix [options] <rom.gba>\n"
              "\n"
              "options:\n"
              "   -t<title>       Update the title (12 chars).\n"
              "   -c<game_code>   Update the game code (4 chars)\n"
              "   -m<maker_code>  Update the maker code (2 chars)\n"
              "   -r<version>     Update the game version (U8)\n");
      exit(1);
   }

   char *filepath = 0;
   for(int argument_index = 1; argument_index < argument_count; argument_index++)
   {
      if(arguments[argument_index][0] != '-')
      {
         filepath = arguments[argument_index];
         break;
      }
   }

   if(!filepath)
   {
      fprintf(stderr, "ERROR: No input ROM was provided.\n");
      exit(1);
   }

   FILE *file = fopen(filepath, "r+b");
   if(!file)
   {
      fprintf(stderr, "ERROR: Failed to open file %s.\n", filepath);
      exit(1);
   }

   gba_header header = {0};
   if(fread(&header, sizeof(header), 1, file) != 1)
   {
      fprintf(stderr, "ERROR: Failed to read file %s.\n", filepath);
      exit(1);
   }

   memcpy(header.logo, logo, sizeof(header.logo));
   header.fixed_value = 0x96;

   // NOTE: Process argument flags.
   for(int argument_index = 1; argument_index < argument_count; argument_index++)
   {
      char *argument = arguments[argument_index];

      if((argument[0] == '-'))
      {
         char *value = argument + 2; // Skip flag prefix
         switch(argument[1])
         {
            case 't': { // Update the title.
               if(value[0])
               {
                  // TODO: Does header.title need to be null-terminated?
                  char title[sizeof(header.game_title)] = {0};
                  strncpy(title, value, sizeof(header.game_title));

                  memcpy(header.game_title, title, sizeof(header.game_title));
               }
               else
               {
                  fprintf(stderr, "WARNING: No value was provided to %s, so it was ignored.\n", argument);
               }
            } break;

            case 'c': {// Update the game code.
               // TODO: Handle values that are not 4 chars long.
               header.game_code = value[0] | value[1]<<8 | value[2]<<16 | value[3]<<24;
            } break;

            case 'm': { // Update the maker code.
               // TODO: Handle values that are not 2 chars long.
               header.maker_code = value[0] | value[1]<<8;
            } break;

            case 'v': { // Update the version number.
               if(value[0])
               {
                  header.software_version = (u8)strtoul(value, 0, 0);
               }
               else
               {
                  fprintf(stderr, "WARNING: No value was provided to %s, so it was ignored.\n", argument);
               }
            } break;

            case 'd': {
               // Enable/disable debugging.
               if(value[0])
               {
                  header.logo[0x9C-0x04] = 0xA5; // Enable debug.
                  header.device_type = (u8)((strtoul(value, 0, 0) & 1) << 7); // Debug handler entry point.
               }
               else
               {
                  fprintf(stderr, "WARNING: No value was provided to %s, so it was ignored.\n", argument);
               }
            } break;

            default: {
               fprintf(stderr, "WARNING: Invalid option: %s\n", argument);
            } break;
         }
      }
   }

   // NOTE: Compute complement.
   header.complement = compute_header_complement(&header);

   // NOTE: Write the updated header back to the ROM.
   fseek(file, 0, SEEK_SET);
   if(fwrite(&header, sizeof(header), 1, file) != 1)
   {
      fprintf(stderr, "ERROR: Failed to update the header of %s.\n", filepath);
      fclose(file);
      exit(1);
   }

   fclose(file);

   return(0);
}
