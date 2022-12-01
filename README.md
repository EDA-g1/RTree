# R-Tree

## Cómo correrlo

### Prerequisitos

> Recordar de que antes de instalar cualquier librería siempre es recomendable correr `sudo apt update` y `sudo apt upgrade` primero.
Para ejecutar el R-Tree es necesario contar con la librería **SFML**, puesto que esta es la que se empleó para crear la interfaz gráfica.

Si se está empleando Linux, basta con utilizar los comandos mostrado a continuación para descargarla.
```
sudo apt-get install libsfml-dev
```

Cabe mencionar también que se requiere de **g++** para la compilación. Si es que no se tiene este instalado, usar
```
sudo apt install g++
```
para hacerlo.

Una vez cubiertos estos prerequisitos, podemos ejecutarlo.

### Compilación

Nos dirigimos a la carpeta para la que queremos crear el ejecutable:
- `rtree` contiene el R-Tree vanilla
- `hilbert_rtree` el Dynamic Hilbert R-Tree.
- `star_rtree` el R*-Tree.

Para poder hacer la compilación, ofrecemos dos formas:

1. Usando el Makefile adjunto:
```
make
```

2. Usando la línea de comando directamente:
```
g++ ./main_star.cpp -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
```

### Ejecución

Correr el ejecutable que resulta de la compilación.

Los comandos son sencillos:
- **Click izquierdo:** añadir un punto (sea independiente o para formar una figura)
- **Click derecho:** para realizar la busqueda de los k-vecinos más cercanos por defecto k = 3
- **Mantener presionar la tecla `LShift` en el teclado:** remover una figura con el clik derecho
- **Presionar la tecla `q` en el teclado:** agregar una figura
- **Scroll del mouse:** añadir multiples puntos

El R-Tree se actualiza automáticamente tras recibir estos inputs.
