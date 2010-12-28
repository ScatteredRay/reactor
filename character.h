#ifndef _CHARACTER_H_
#define _CHARACTER_H_

struct Character;

void InitCharacters();
void FinishCharacters();
Character* CreateCharacter();
void RenderCharacter(Character* caracter);
void DestroyCharacter(Character* character);

#endif //_CHARACTER_H_
