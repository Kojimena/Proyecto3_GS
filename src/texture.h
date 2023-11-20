#pragma once


#include "color.h"

class Texture {
public:
    SDL_Texture* sdlTexture;
    Color getColor(float u, float v) {
        // Implementación que devuelva un color basado en las coordenadas UV
        // Esta parte es compleja porque SDL_Texture no permite acceso directo a píxeles

    }
};