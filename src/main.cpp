#include <SDL2/SDL.h>
#include <glm/geometric.hpp>
#include <string>
#include <vector>
#include <print.h>
#include <SDL_image.h>
#include "color.h"
#include "intersect.h"
#include "object.h"
#include "sphere.h"
#include "light.h"
#include "camera.h"
#include "cube.h"
#include "skybox.h"


const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 600;
const float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
const int MAX_RECURSION = 3;
const float BIAS = 0.0001f;
glm::vec3 lightOffset(0.0f, 2.0f, -1.0f);  // Ejemplo de desplazamiento
Skybox skybox("../assets/ocean.png");

SDL_Renderer* renderer;
std::vector<Object*> objects;
Light light(glm::vec3(-1.0, 0, 10), 1.0f, Color(255, 255, 255));
Camera camera(glm::vec3(0.0, 0.0, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);


void point(glm::vec2 position, Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(renderer, position.x, position.y);
}

float castShadow(const glm::vec3& shadowOrigin, const glm::vec3& lightDir, Object* hitObject) {
    for (auto& obj : objects) {
        if (obj != hitObject) {
            Intersect shadowIntersect = obj->rayIntersect(shadowOrigin, lightDir);
            if (shadowIntersect.isIntersecting && shadowIntersect.dist > 0) {
                float shadowRatio = shadowIntersect.dist / glm::length(light.position - shadowOrigin);
                shadowRatio = glm::min(1.0f, shadowRatio);
                return 1.0f - shadowRatio;
            }
        }
    }
    return 1.0f;
}

Color getColorFromSurface(SDL_Surface* surface, float u, float v) {
    if (surface == nullptr) return Color(0, 0, 0); // Retornar color negro en caso de no haber textura

    u = fmod(u, 1.0f);
    v = fmod(v, 1.0f);
    if (u < 0) u += 1.0f;
    if (v < 0) v += 1.0f;

    int x = static_cast<int>(u * surface->w);
    int y = static_cast<int>(v * surface->h);

    Uint32 pixel = static_cast<Uint32*>(surface->pixels)[y * surface->w + x];
    SDL_Color color;
    SDL_GetRGB(pixel, surface->format, &color.r, &color.g, &color.b);
    return Color(color.r, color.g, color.b);
}


Color castRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const short recursion = 0) {
    float zBuffer = 99999;
    Object* hitObject = nullptr;
    Intersect intersect;

    for (const auto& object : objects) {
        Intersect i = object->rayIntersect(rayOrigin, rayDirection);
        if (i.isIntersecting && i.dist < zBuffer) {
            zBuffer = i.dist;
            hitObject = object;
            intersect = i;
        }
    }

    if (!intersect.isIntersecting || recursion >= MAX_RECURSION) {
        return skybox.getColor(rayDirection);  // Sky color
    }



    glm::vec3 lightDir = glm::normalize(light.position - intersect.point);
    glm::vec3 viewDir = glm::normalize(rayOrigin - intersect.point);
    glm::vec3 reflectDir = glm::reflect(-lightDir, intersect.normal); 

    float shadowIntensity = castShadow(intersect.point, lightDir, hitObject);

    float diffuseLightIntensity = std::max(0.0f, glm::dot(intersect.normal, lightDir));
    float specReflection = glm::dot(viewDir, reflectDir);
    
    Material mat = hitObject->material;

    float specLightIntensity = std::pow(std::max(0.0f, glm::dot(viewDir, reflectDir)), mat.specularCoefficient);


    Color reflectedColor(0.0f, 0.0f, 0.0f);
    if (mat.reflectivity > 0) {
        glm::vec3 origin = intersect.point + intersect.normal * BIAS;
        reflectedColor = castRay(origin, reflectDir, recursion + 1); 
    }

    Color refractedColor(0.0f, 0.0f, 0.0f);
    if (mat.transparency > 0) {
        glm::vec3 origin = intersect.point - intersect.normal * BIAS;
        glm::vec3 refractDir = glm::refract(rayDirection, intersect.normal, mat.refractionIndex);
        refractedColor = castRay(origin, refractDir, recursion + 1); 
    }

    Color diffusecolor ;
    if (mat.texture != nullptr) {
        diffusecolor = getColorFromSurface(mat.texture, intersect.tx, intersect.ty);
    } else {
        diffusecolor = mat.diffuse;
    }

    Color diffuseLight = diffusecolor * light.intensity * diffuseLightIntensity * mat.albedo * shadowIntensity;
    Color specularLight = light.color * light.intensity * specLightIntensity * mat.specularAlbedo * shadowIntensity;
    Color color = (diffuseLight + specularLight) * (1.0f - mat.reflectivity - mat.transparency) + reflectedColor * mat.reflectivity + refractedColor * mat.transparency;
    return color;
}

SDL_Surface* loadTexture(const std::string& file) {
    SDL_Surface* surface = IMG_Load(file.c_str());
    if (surface == nullptr) {
        std::cerr << "Unable to load image: " << IMG_GetError() << std::endl;
    }
    return surface;
}


void setUp() {

    SDL_Surface* textureSurface = loadTexture("../assets/face.png");
    SDL_Surface* skinFace = loadTexture("../assets/skin.png");
    SDL_Surface* chestFace = loadTexture("../assets/collar.png");
    SDL_Surface* dress = loadTexture("../assets/dress.png");
    SDL_Surface * tail = loadTexture("../assets/tail.png");
    SDL_Surface * hair = loadTexture("../assets/hair.png");

    SDL_Surface * faceFish = loadTexture("../assets/facefish.png");
    SDL_Surface * bodyFish = loadTexture("../assets/bodyfish.png");

    SDL_Surface * trident = loadTexture("../assets/trident.png");

    Material faceMaterial = {
        Color(0, 0, 0),
        0.9,
        0.3,
        10.0f,
        0.0f,
        0.0f,
        0.0f,
        textureSurface
    };

    Material facefishMaterial = {
            Color(0, 0, 0),
            1.0,
            0.3,
            10.0f,
            0.2f,
            0.0f,
            0.0f,
            faceFish
    };

    Material bodyfishMaterial = {
            Color(0, 0, 0),
            0.9,
            0.3,
            10.0f,
            0.0f,
            0.0f,
            0.0f,
            bodyFish
    };

    Material bodyMaterial = {
            Color(0, 0, 0),
            0.9,
            0.3,
            10.0f,
            0.0f,
            0.0f,
            0.0f,
            skinFace
    };

    Material chestMaterial = {
            Color(0, 0, 0),
            0.9,
            0.3,
            10.0f,
            0.0f,
            0.0f,
            0.0f,
            chestFace
    };

    Material dressMaterial = {
        Color(155, 0, 0),
        1.0,
        0.3,
        10.0f,
        0.2f,
        0.0f,
        0.0f,
        dress

    };

    Material tailMaterial = {
        Color(0, 0, 0),
        1.0,
        0.3,
        10.0f,
        0.2f,
        0.0f,
        1.0f,
        tail
    };

    Material hairMaterial = {
            Color(0, 0, 0),
            1.2,
            0.9,
            10.0f,
            0.3f,
            0.4f,
            10.0f,
            hair
    };

    Material greeneMaterial = {
        Color(20, 255, 230, 10),   // diffuse
        0.9,
        0.1,
        10.0f,
        0.7f,
        0.0f,
        10.0f,
    };

    Material mirror = {
        Color(255, 255, 255),
        0.0f,
        10.0f,
        1425.0f,
        0.9f,
        0.0f
    };

    Material glass = {
        Color(255, 0, 225),
        0.0f,
        10.0f,
        1525.0f,
        0.2f,
        1.0f,
        1525.0f
    };

    Material tridentMaterial = {
            Color(0, 0, 0),
            1.3,
            0.3,
            5.0f,
            0.4f,
            0.0f,
            0.0f,
            trident
    };


    objects.push_back(new Cube(glm::vec3(0.0f, 0.0f, -1.0f), 1.0f, faceMaterial));
    //hair
    objects.push_back(new Sphere(glm::vec3(0.0f, 0.6f, -1.0f), 0.5f, hairMaterial));
    objects.push_back(new Sphere(glm::vec3(0.6f, 0.6f, -1.0f), 0.5f, tridentMaterial));
    objects.push_back(new Sphere(glm::vec3(-0.6f, 0.6f, -1.0f), 0.5f, tridentMaterial));
    objects.push_back(new Sphere(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, tridentMaterial));
    objects.push_back(new Sphere(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, tridentMaterial));
    //shoulder
    objects.push_back(new Cube(glm::vec3(-0.5f, -1.0f, -1.0f), 0.8f, bodyMaterial));
    objects.push_back(new Cube(glm::vec3(0.5f, -1.0f, -1.0f), 0.8f, bodyMaterial));

    //cuerpo
    objects.push_back(new Cube(glm::vec3(0.0f, -1.0f, -1.0f), 1.0f, chestMaterial));
    objects.push_back(new Cube(glm::vec3(0.0f, -2.0f, -1.0f), 1.0f, dressMaterial));
    objects.push_back(new Cube(glm::vec3(0.0f, -3.0f, -1.0f), 1.0f, tailMaterial));


    //pez cara
    objects.push_back(new Cube(glm::vec3(3.0f, -2.0f, 0.0f), 1.2f, facefishMaterial));

    //burbujas
    objects.push_back(new Sphere(glm::vec3(3.0f, -2.0f, 1.0f), 0.2f, mirror));
    objects.push_back(new Sphere(glm::vec3(3.0f, -2.0f, 2.0f), 0.2f, mirror));
    objects.push_back(new Sphere(glm::vec3(3.0f, -1.5f, 1.5f), 0.1f, mirror));

    //pez cuerpo
    objects.push_back(new Cube(glm::vec3(3.0f, -1.0f, 0.0f), 0.2f, bodyfishMaterial));
    objects.push_back(new Cube(glm::vec3(3.0f, -1.5f, 0.0f), 0.7f, bodyfishMaterial));
    objects.push_back(new Cube(glm::vec3(2.6f, -2.0f, 0.0f), 0.9f, bodyfishMaterial));
    objects.push_back(new Cube(glm::vec3(3.4f, -2.0f, 0.0f), 0.9f, bodyfishMaterial));

    //trident
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, -0.6f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, -0.4f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, -0.2f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 0.0f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 0.2f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 0.4f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 0.6f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 0.8f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 1.0f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 1.2f), 0.2f, greeneMaterial));

    objects.push_back(new Cube(glm::vec3(1.0f, -1.2f, 1.2f), 0.2f, greeneMaterial));
    objects.push_back(new Cube(glm::vec3(0.6f, -1.2f, 1.2f), 0.2f, greeneMaterial));

    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 1.4f), 0.2f, tridentMaterial));
    objects.push_back(new Cube(glm::vec3(0.8f, -1.2f, 1.6f), 0.2f, tridentMaterial));
    objects.push_back(new Cube(glm::vec3(0.6f, -1.2f, 1.4f), 0.2f, tridentMaterial));
    objects.push_back(new Cube(glm::vec3(1.0f, -1.2f, 1.4f), 0.2f, tridentMaterial));
    objects.push_back(new Cube(glm::vec3(0.4f, -1.2f, 1.4f), 0.2f, tridentMaterial));
    objects.push_back(new Cube(glm::vec3(1.2f, -1.2f, 1.4f), 0.2f, tridentMaterial));
    objects.push_back(new Cube(glm::vec3(0.4f, -1.2f, 1.6f), 0.2f, tridentMaterial));
    objects.push_back(new Cube(glm::vec3(1.2f, -1.2f, 1.6f), 0.2f, tridentMaterial));

}

void render() {
    float fov = 3.1415/3;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {

            float screenX = (2.0f * (x + 0.5f)) / SCREEN_WIDTH - 1.0f;
            float screenY = -(2.0f * (y + 0.5f)) / SCREEN_HEIGHT + 1.0f;
            screenX *= ASPECT_RATIO;
            screenX *= tan(fov/2.0f);
            screenY *= tan(fov/2.0f);


            glm::vec3 cameraDir = glm::normalize(camera.target - camera.position);

            glm::vec3 cameraX = glm::normalize(glm::cross(cameraDir, camera.up));
            glm::vec3 cameraY = glm::normalize(glm::cross(cameraX, cameraDir));
            glm::vec3 rayDirection = glm::normalize(
                cameraDir + cameraX * screenX + cameraY * screenY
            );
           
            Color pixelColor = castRay(camera.position, rayDirection);

            point(glm::vec2(x, y), pixelColor);
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Kosirena - FPS: 0",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    int frameCount = 0;
    Uint32 startTime = SDL_GetTicks();
    Uint32 currentTime = startTime;


    
    setUp();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        camera.move(-1.0f);
                        break;
                    case SDLK_DOWN:
                        camera.move(1.0f);
                        break;
                    case SDLK_LEFT:
                        camera.rotate(-1.0f, 0.0f);
                        break;
                    case SDLK_RIGHT:
                        camera.rotate(1.0f, 0.0f);
                        break;

                 }
                light.position = camera.position + lightOffset;
            }


        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render();

        // Present the renderer
        SDL_RenderPresent(renderer);

        frameCount++;

        // Calculate and display FPS
        if (SDL_GetTicks() - currentTime >= 1000) {
            currentTime = SDL_GetTicks();
            std::string title = "Kosirena - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

