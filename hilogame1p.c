//======================================================================================================================================
// hilogame1p.c: Higher Or Lower Text Based Playing Card Betting Game.
//======================================================================================================================================
// This program plays the hi-lo card game for a single player. This hi-lo card game is described as follows.
//To play, both the player and the house receive a random card from a single shuffled deck. The player's card is face down, but the house's card is face up showing the player the house's card value. The player must decide what amount to bet before knowing the house's card, then once the house's card is known, bet that their card is higher or lower than the house's card. If they are correct, or the card values are equal, the player wins their bet back plus 25 percent. If the cards are not the same and the player's guess was wrong, the player loses all of their bet. In this card game Aces are high. Find a strategy to win! Bets must be some multiple of 4 units and not more than their balance. The player starts with 1000 units and the target is to get to 1 million units. If the player's money falls below 4 units, they lose!
// Author: Simon Goater Sep 2026.
// Usage:- ./hilogame1p.bin [seed]
//
// COPYRIGHT NOTICE: Copying and distributing without modification, but with conspicuous attribution for any legal purpose is permitted.
//======================================================================================================================================
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <openssl/evp.h>
  
// gcc hilogame1p.c -o hilogame1p.bin -O3 -Wall -mssse3 -lssl -lcrypto 
// (sudo apt-get install libssl-dev ...for evp.h)

//===============================
//#define HASH_NAME "md5"
//#define HASH_LEN 16
//===============================
//#define HASH_NAME "md4"
//#define HASH_LEN 16
//===============================
//#define HASH_NAME "sha1"
//#define HASH_LEN 20
//===============================
//#define HASH_NAME "sha224"
//#define HASH_LEN 28
//===============================
#define HASH_NAME "sha256"
#define HASH_LEN 32
//===============================
//#define HASH_NAME "sha384"
//#define HASH_LEN 48
//===============================
//#define HASH_NAME "sha512"
//#define HASH_LEN 64
//===============================
//#define HASH_NAME "ripemd160"
//#define HASH_LEN 20
//===============================
//#define HASH_NAME "sha3-224"
//#define HASH_LEN 28
//===============================
//#define HASH_NAME "sha3-256"
//#define HASH_LEN 32
//===============================
//#define HASH_NAME "sha3-384"
//#define HASH_LEN 48
//===============================
//#define HASH_NAME "sha3-512"
//#define HASH_LEN 64
//===============================

#define HILO_PLAYER_FUNDS 1000ULL
#define HILO_INPUT_BUFFER_SIZE 101
_Bool getinput(char *buffer, size_t buffersize, char *msg) {
  _Bool validinput = false;
  while (!validinput) {
    printf("%s", msg);
    fflush(stdout);
    if (fgets(buffer, HILO_INPUT_BUFFER_SIZE-1, stdin)) {
      buffer[HILO_INPUT_BUFFER_SIZE-1] = 0;
      size_t count = strlen(buffer);
      if (buffer[count-1] == '\n') {
        buffer[count-1] = 0;
        count--;
      }
      if (count) validinput = true;
    }
  }
  return true;
}

uint64_t getbetamount(char *buffer, size_t buffersize, uint64_t playerfunds) {
  uint64_t betamount = 0;
  while (betamount == 0) {
    printf("You have %lu units.\n", playerfunds);
    assert(getinput(buffer, buffersize, "Enter amount you would like to bet. (Multiple of 4).\n"));
    sscanf(buffer, "%lu", &betamount);
    if (betamount > playerfunds) {
      printf("You have insufficient funds for that!\n");
      betamount = 0;
    }
    if (betamount % 4 != 0) betamount = 0;
  }
  return betamount;
}

_Bool getbet(char *buffer, size_t buffersize) {
  buffer[0] = 0;
  while ((buffer[0] != 'H') && (buffer[0] != 'L')) {
    assert(getinput(buffer, buffersize, "Enter H (Higher) or L (Lower).\n"));
  }
  return buffer[0] == 'H';
}

char *playingcards[] = {"H2", "D2", "C2", "S2", "H3", "D3", "C3", "S3", "H4", "D4", "C4", "S4", "H5", "D5", "C5", "S5", "H6", "D6", "C6", "S6", "H7", "D7", "C7", "S7", "H8", "D8", "C8", "S8", "H9", "D9", "C9", "S9", "H10", "D10", "C10", "S10", "HJ", "DJ", "CJ", "SJ", "HQ", "DQ", "CQ", "SQ", "HK", "DK", "CK", "SK", "HA", "DA", "CA", "SA"};

_Bool newdeck(uint8_t *deck) {
  for (uint8_t i=0; i < 52; i++) deck[i] = i;
  for (uint8_t i=0; i < 100; i++) {
    for (uint8_t j=0; j < 52; j++) {
      uint8_t pos = rand() % 52;
      uint8_t temp = deck[j];
      deck[j] = deck[pos];
      deck[pos] = temp;
    }
  }
  unsigned char md_bin[HASH_LEN];
  EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
  const EVP_MD *usedigest = EVP_get_digestbyname(HASH_NAME);
  EVP_DigestInit_ex(mdctx, usedigest, NULL);
  EVP_DigestUpdate(mdctx, deck, 52*sizeof(uint8_t));
  EVP_DigestFinal_ex(mdctx, md_bin, NULL);
  EVP_MD_CTX_destroy(mdctx);
  EVP_cleanup();
  printf("Deck hash is ");
  for (uint16_t i=0; i<HASH_LEN; i++) printf("%02x", md_bin[i]);
  printf(".\n");
  fflush(stdout);
  return true;
}

int main(int argc, char* argv[]) {
  printf("This program plays the hi-lo card game for a single player. This hi-lo card game is described as follows.\nTo play, both the player and the house receive a random card from a single shuffled deck. The player's card is face down, but the house's card is face up showing the player the house's card value. The player must decide what amount to bet before knowing the house's card, then once the house's card is known, bet that their card is higher or lower than the house's card. If they are correct, or the card values are equal, the player wins their bet back plus 25 percent. If the cards are not the same and the player's guess was wrong, the player loses all of their bet. In this card game Aces are high. Find a strategy to win! Bets must be some multiple of 4 units and not more than their balance. The player starts with %llu units and the target is to get to 1 million units. If the player's money falls below 4 units, they lose!\nUsage: %s [seed]\nAuthor: Simon Goater\n", HILO_PLAYER_FUNDS, argv[0]);
  char buffer[HILO_INPUT_BUFFER_SIZE];
  if (argc >= 2) {
    srand(atol(argv[1]));
  } else {
    srand(time(0));
  }
  uint64_t playerfunds = HILO_PLAYER_FUNDS;
  uint8_t deck[52];
  uint8_t deckpos = 52;
  while ((playerfunds > 3) && (playerfunds < 1000000)) {
    if (deckpos >= 52) {
      printf("New shuffled deck.\n");
      assert(newdeck(deck));
      deckpos = 0;
    }
    uint64_t betamount = getbetamount(buffer, HILO_INPUT_BUFFER_SIZE, playerfunds);
    playerfunds -= betamount;
    uint8_t housecard = deck[deckpos];
    printf("House's card is %s...\n", playingcards[housecard]);
    deckpos++;
    _Bool betishigher = getbet(buffer, HILO_INPUT_BUFFER_SIZE);
    if (betishigher) {
      printf("You bet higher.\n");
    } else {
      printf("You bet lower.\n");
    }
    fflush(stdout);
    sleep(5);
    uint8_t playercard = deck[deckpos];
    printf("Your card is %s...\n", playingcards[playercard]);
    _Bool playerwins = false;
    if (betishigher) {
      if ((playercard >> 2) >= (housecard >> 2)) playerwins = true;
    } else {
      if ((playercard >> 2) <= (housecard >> 2)) playerwins = true;
    }
    if (playerwins) {
      printf("You won!\n");
      playerfunds += (5*betamount)/4;
    } else {
      printf("You lost!\n");
    }
    deckpos++;
  }
  if (playerfunds < 4) {
    printf("You're out of money!\n");
  } else {
    printf("Congratulations!! You're a millionaire!!!\n");
  }
  if (deckpos < 52) {
    printf("Remaining cards in the deck are");
    for (; deckpos < 52; deckpos++) printf(" %s", playingcards[deck[deckpos]]);
    printf(".\n");
  } else {
    printf("There are no cards remaining in this deck.\n");
  }
  exit(0);
}
