# Distribucion de Paquetes en Camiones con Capacidad Limitada

## Descripción
Proyecto de la materia Computación de Alta Performance de la Facultad de Ingeniería, UdelaR.

Se aborda el problema de ruteo de una flota de camiones con restricciones de volumen, se utiliza el algoritmo Branch and Bound y se propone una estrategia paralela que busca minimizar la distancia recorrida.

Autores: 
- Carolina Acosta (carolina.acosta@fing.edu.uy)
- Mateo Gargano (mateo.gargano@fing.edu.uy)

## Instrucciones de ejecución
1- Descoprimir el archivo almrrc2021-data.zip, esto debe generar 2 carpetas llamadas almrrc2021-data-evaluation y almrrc2021-data-training que contienen datos obtenidos del Amazon Last Mile Routing Research Challenge.

2- Compilar el archivo que se desea ejecutar y ejecutarlo: 
 - El mainSerial.cpp contiene una solución serial del problema.

 En nuestro caso lo compilamos usnado `gcc -x c++ -o mainSerial mainSerial.cpp -lstdc++ -lm`
 y ejecutamos con `./mainSerial`
 - El mainMPI.cpp contiene la solución paralela al problema usando MPI.

 En nuestro caso lo compilamos usnado `mpicc mainMPI.cpp -o mainMPI -lstdc++ -lm`
 y ejecutamos con `mpirun -np <N> -hosts <N hosts> ./mainMPI`.

 Para especificar los <N hosts> se puede hacer con los nombres de las maquinas separados por coma (ejemplo: `mpirun -np 2 -hosts pcunix40,pcunix42 ./mainMPI`) o se puede definir los hosts dentro de un archivo (ejemplo: `mpirun -np 2 -hostfile mmis_hosts ./mainMPI`)