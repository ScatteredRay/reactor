#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "vectormath.h"

struct Character;
struct PlayerInput;

void InitCharacters();
void FinishCharacters();
Character* CreateCharacter(PlayerInput* input);
void UpdateCharacter(Character* character, float DeltaTime);
void RenderCharacter(Character* caracter);
void DestroyCharacter(Character* character);

Vector3 GetCharacterLocation(Character* character);

#endif //_CHARACTER_H_
