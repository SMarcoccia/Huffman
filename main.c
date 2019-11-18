
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <windows.h>

#include "ctypes.h"
#include "huffman.h"

#define DEBUG_1 0 // Pour Huffman_Check_StructHuffman().
#define DEBUG_2 0 // Idem.
#define DEBUG_3 0 // Idem.
#define DEBUG_4 0 // Pour Huffman_DisplayDico().
#define DEBUG_5 0 // Pour Huffman_DisplayDataFileCompress().


#define MASK (1 << 15) // Pour touche clavier.


int main(void)
{
  freopen("CON", "w", stdout);
  // Todo : Vérifier si fichier vide.
  // Todo faire malloc si texte plus grand.

  Huffman_InitListCara();
  Huffman_RecoveryFile();
  Huffman_DicoInit();
  Huffman_HuffmanInit(NULL);
  Huffman_CounterOcc();

  #if DEBUG_1
    Huffman_Check_StructHuffman(NULL);
  #endif
  
  Huffman_Qsort(NULL);

  #if DEBUG_2
    Huffman_Check_StructHuffman(NULL);
  #endif

  Huffman_AddOcc();
  
  #if DEBUG_3
    Huffman_Check_StructHuffman(NULL);
  #endif
  
  Huffman_CreateDico();

  #if DEBUG_4
    Huffman_DisplayDico();
  #endif

  Huffman_SetDicoFileCompressed();
  Huffman_InitDicoUsed();
  Huffman_CompressionFileTxt();
  Huffman_DicoInit();
  Huffman_GetDataFileCompress();
  
  #if DEBUG_5
    Huffman_DisplayDataFileCompress();
    printf("---------------------------------\n");
  #endif
  Huffman_UncompressTxt();

  Huffman_FreeDicoUsed();
  Huffman_FreeArrayTxtCompress();

  printf("Espace pour quitter\n");
  while(1)
  {
    if(GetKeyState(VK_SPACE) & MASK)
    {
      break;
    }
  }    

  return 0;
}