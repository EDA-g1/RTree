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


Cada uno de los árboles implementados tienen su propia compilación y ejecución.
1. Para Rtree:
```
make run_rtree
```
2. Para Dynamic HilbertRtree:
```
make run_hilbert
```
3. Para R\*Tree:
```
make run_srtree
```



### Ejecución

Correr el ejecutable que resulta de la compilación.

Los comandos son los siguientes:
- **Click izquierdo o scroll down del mouse:** añadir un punto (sea independiente o para formar una figura)
- **Click derecho:** para realizar la busqueda de los k-vecinos más cercanos por defecto k = 3
- **Mantener presionar la tecla `LShift` en el teclado:** remover una figura con el clik derecho
- **Presionar la tecla `q` en el teclado:** modo insercion de poligono, insertar n puntos y luego presionar `q` para insertarlo
- **Presionar un numero:** agrega el dígito del número presionado al final del knn actual. Ej: con knn 1 si se presiona 2 knn se vuelve 12 (el valor por defecto del knn es 3).
- **Presionar backspace:** elimina el ultimo número presionado para el knn 

La GUI se actualiza automáticamente dependiendo de alguno de estos inputs.
