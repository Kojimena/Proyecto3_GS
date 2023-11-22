# Raytracing

Este proyecto es una aplicación gráfica que utiliza Raycasting y la biblioteca SDL2 para renderizar una escena 3D. La escena incluye diversos objetos como esferas, cubos y un cielo (skybox), junto con técnicas de iluminación y sombreado.

## Demostración

Se puede ver el juego en acción en este [video](https://drive.google.com/file/d/1Xwa5cS0IBLwZA52bafeuIdHKrM6HzVCF/view?usp=sharing).

## Puntos cumplidos

- [30 puntos] Criterio subjetivo. Por qué tan compleja sea su escena
- [20 puntos] Criterio subjetivo. Por qué tan visualmente atractiva sea su escena
- [20 puntos] Por implementar rotación en su diorama y dejar que la camara se acerque y aleje
    - [Se implementó rotación y se puede acercar y alejar la cámara]
- [5 puntos] por cada material diferente que implementen, para un máximo de 5 (piensen en los diferentes tipos de bloques en minecraft)
  - [Se realizaron 10 materiales diferentes]
- [10 puntos] por implementar refracción en al menos uno de sus materiales (debe tener sentido contextual en su escena)
    - [Se aplicó refracción a la espada y esfera de cristal de la sirena]
- [5 puntos] por implementar reflexión en al menos uno de sus materiales
    - [Se aplicó reflexión a las burbujas del pez y a la espada de la sirena]
- [20 puntos] por implementar un skybox para su material
    - [Se implementó un skybox para el fondo del mar]

## Características

- **Resolución de pantalla**: 700x600 con una relación de aspecto definida.
- **Técnicas de Raycasting**: Implementa raycasting para renderizar la escena 3D.
- **Iluminación y Sombreado**: Calcula la iluminación y sombras para cada objeto en la escena.
- **Materiales y Texturas**: Cada objeto tiene su propio material y textura para un aspecto realista.
- **Control de Cámara**: Permite mover la cámara y rotar la vista.
- **Skybox**: Incluye un cielo panorámico para una mayor inmersión.
