# Distribucion de Paquetes en Camiones con Capacidad Limitada

## Descripción
Proyecto de la materia Computación de Alta Performance de la Facultad de Ingeniería, UdelaR.

Se aborda el problema de ruteo de una flota de camiones con restricciones de volumen, se utiliza el algoritmo Branch and Bound y se propone una estrategia paralela que busca minimizar la distancia recorrida.

Autores: 
- Carolina Acosta (carolina.acosta@fing.edu.uy)
- Mateo Gargano (mateo.gargano@fing.edu.uy)

## Instrucciones de ejecución
1- Compilar el archivo que se desea ejecutar
 - El mainSerial.cpp contiene una solución serial del problema. En nuestro caso lo compilamos usando `gcc -x c++ -o mainSerial mainSerial.cpp -lstdc++ -lm`.

 - El mainMPI.cpp contiene la solución paralela al problema usando MPI. En nuestro caso lo compilamos usando `mpicc mainMPI.cpp -o mainMPI -lstdc++ -lm`.
 
2 - Correr el ejecutable generado

El código serial lo ejecutamos con `./mainSerial`.

En nuestro caso el codigo paralelo loejecutamos con `mpirun -np <N> -hosts <N hosts> ./mainMPI`.

Para especificar los hosts se puede hacer con los nombres de las maquinas separados por coma (ejemplo: `mpirun -np 2 -hosts pcunix40,pcunix42 ./mainMPI`) o se puede definir los hosts dentro de un archivo (ejemplo: `mpirun -np 2 -hostfile mmis_hosts ./mainMPI`)