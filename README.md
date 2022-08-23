# Secuenciador musical utilizando ESP32

En este proyecto la idea es ir más alla con los conceptos del DDS y el NCO implementados en el proyecto anterior (si alguien lo quiere ver, acá esta el link: https://github.com/BrunoEdPerezS/ddsESP32) implementando un secuenciador en el mismo micro.

De todas formar la idea tras el diseño del secuenciador es que pueda ser "modular" de forma que con aplicar uno que otro cambio sea posible implementar esto con otro "motor de sintesis", así que en el repo hay dos sketchs de arduino, uno con el dds ya implementado y otro con el secuenciador funcionando por si sólo (funciona el software perfectamente, pero a la salida del micro solo entrega High o Low de acuerdo a los steps).
En este caso el codigo no será explicado tan detalladamente, ya que es bastante. PERO, me aeguraré de explicar bien el "como haremos que..." de forma que sea fácil para el lector leer el codigo y entender que pasa en cada sección (o escribir el suyo propio).

Bueno, unas consideraciones importantes del diseño. La idea del proyecto es realizar dos cosas, poder configurar una secuencia, y reproducirla. El apartado de "configurar la secuencia" es el más engorroso porque a parte de configurar un patron rítimico, implementaremos una forma de configurar una secuencia de notas musicales para dotar de melodía al ritmo y así producir resultados con mayor musicalidad. Ahora sin más exceso de texto, vamos a proceder a explicar un poco que son los secuenciadores.

## Secuenciadores en la música

Iremos al grano, en el business de la música **llamamos secuenciador a cualquier cosa, sea máquina, software, o circuito, que nos permita realizar una secuencia de eventos músicales preestablecidos, a lo largo de un intervalo de tiempo.**

Con esta definición haré un parentesis importante, si bien el secuenciador explícitamente es la **herramienta** que nos permite "reproducir" dichos **eventos**, realmente no los reproduce sino que "ordena" su reproducción a los demás elementos, el secuenciador no es un sampler, o un instrumento, y menos un DAW (una vez lei por ahi que FL Studio era un secuenciador), el secuenciador es **quien le dice a estos 3 cuando deben hacer "algo"**, sea ese algo, cambiar de nota, tocar o no tocar una nota, activar o desactivar un efecto, o lanzar un sample, etc. Es una **HERRAMIENTA** que se encarga de ordenar la ejecución de estos eventos.
Haciendo más gráfico esto podriamos decir que el "secuenciador" dentro de FL Studio sería el piano roll, o incluso el "playlist", donde podemos cargar los eventos músicales y configurar su funcionamiento. Lo mismo para los demás DAWS, pero aún más allá dentro del mismo DAW podemos utilizar secuenciadores VST, que reproducen eventos MIDI para controlar los demás instrumentos. En un caso más analógico, para los que no tienen experiencia con DAWS, piensen en los drum machines clásicos, el TR-808, dentro de este el secuenciador corresponde al sistema que permite almacenar los datos de "cuando" y "cual" sonido se debe reproducir, pero quien reproduce esos sonidos es el sistema eléctrónico incorporado en el instrumento. El secuenciador sería el grupo de botones de colores en la parte inferior de la drum machine y su circuitería asociada, pero no toda la máquina, y menos la sección destinada a reproducir los sonidos.

## Funcionamiento del secuenciador

Ahora con esto explicado, voy a explicar el funcionamiento de un secuenciador. El diseño más conocido de secuenciador es el denominado "step sequencer" y es bastante simple. Tomamos cierta cantidad de "pasos" (steps) y los recorremos a una velocidad determinada. Listo.

Steps recorridos de pana

Esto significa que realizaremos nuestra secuencia a medida que recorremos los steps. Cada uno de estos steps, representará una figura músical, sea una corchea, semicorchea, negra, todo depende de a que velocidad estamos cambiando entre cada uno de ellos. Esto dependerá del tempo, debemos aregurarnos que cambiamos de un step al otro de acuerdo a un tempo determinado, usualmente medido en bpm. De forma que la velocidad a la que recorremos los steps, y la duración de cada step dependerá del tempo. Este nos dirá cuando debemos pasar de un step a otro, en funcion a que figura ritmica representa dicho step. 

Ahora, ¿como se representan estos steps?, bien sabemos, el compás más común en la música es el 4/4, de forma que la mayoría de los displays de los secuenciadores, representan un compás de 4/4, donde cada step corresponde a una semicorchea o corchea (es lo más común, pero también es común que este 4/4 se pueda subdividir o icluso la duración de los steps se pueda configurar). De esta forma los dos display de secuencia más comunes son 8 pasos (corcheas) y 16 pasos (semicorcheas), de como esos se puedan configurar no me hago responsable.

Step secuencers

Ahora una parte importante del secuenciado es que hacemos en cada step, estos steps representan las divisiones de tiempo. Que hacer en cada step, eso depende de su "estado", usualmente los steps se consideran como switches lógicos, es decir, pueden estar encendidos o apagados, high o low, 1 o 0, y pueden alternar entre estos estados. Este estado determinará que debemos hacer en su step correspondiente. Si este en high, debemos tocar una nota, y si este es low, debemos "hacer silencio", es decir, no tocar una nota. Ahora implementando este sistema podemos establecer patrones rítmicos, sería como escribir en lenguaje "binario" una partitura músical alternando entre figuras y silencios.

partitura step secuencer

Con lo dicho anteriormente es facil escribir patrones rítmicos, solo debemos encender los steps donde queremos que "haya sonido" y apagar en los que queremos "silencio".

## Secuenciador por software 

En este apartado, vamos a entrar de lleno al diseño del secuenciador del ESP32, así que sin ánimos de malgastar más texto, vamos a establecer las bases del proyecto.

1. El secuenciador será implementado en un ESP32 y mostrará los 16 steps de la secuencia mediante una pantalla OLED.

2. Contaremos con 5 pulsadores para controlar el secuenciador.

3. El secuenciador contará con 2 modalidades principales, reproducción de secuencias y editor de secuencias.

4. El editor de secuencia permitirá crear 2 patrones de secuencia, tanto de forma rítmica como melódica, es decir podremos configurar que notas reproducir y cuando, estos patrones serán reproducidos a travéz del modo reproducción.

5. Se debe considerar un control para los BPM de la secuencia.

6. La sintesis de sonido será realizada por el NCO diseñado en el proyecto anterior, es decir, de cierta forma vamos a controlar el NCO con el secuenciador.

Ahora considerando estos puntos, vamos a comenzar por el primer apartado, como es que vamos almacenar la información correspondiente a cada secuencia. Y como vamos a reproducirla.

## Bancos de secuencia y banco de arpegio

Almacenaremos dos patrones de secuencia, estos patrones los denominaremos por su palabra en inglés **Pattern** y cada uno contará con dos variables correspondientes, un banco de secuencia y un banco de arpegio.

El banco de secuencia será un vector que contiene la información del estado de cada step dentro del pattern, "1" para encendido y "0" para apagado.

El banco de arpegio contendrá la información de que nota debe reproducirse en cada step, esta informacio se divide en 2 categorías, la nota en sí, representada por los números desde el 0 al 11 (serán 12 notas en total considerando los sostenidos), por ejemplo Do será el 0, Do# será el 1 y así hasta el Si. La segunda cantegoría de información contendrá la octava correspondiente a la nota.

REPRESENTACION DE LOS BANCOS COMO TABLAS

Bien, con esto explicado cabe decir que de ahora en adelante trabajaremos las notas musicales en clave americana. Y pasaremos a explicar en como vamos a reproducir el sonido.

### Recuerdos del DDS

Haciendo memoria del proyecto anterior sabemos que en un NCO podemos variar la frecuencia de la onda que se reproduce mediante la "tuning word" representada por la letra "**M**", ahora, considerando esto y la ecuación básica del NCO tenemos que.

$$ M =\frac{F_{salida} 2^{N}}{F_{CLK}}$$

Donde $F_{salida}$ es literal la frecuencia que queremos a la salida. En nuestro caso actual, este valor corresponderá a la frecuencia correspondiente a cada una de las notas musicales que queremos reproducir con el secuenciador. De forma que para acceder de forma óptima a esos valores vamos a crear una tabla con todas las notas desde las octavas 1 a la 7, y sus resprectivas frecuencias. Luego crearemos otra tabla, la cual contendrá los valores respectivos de **M** para cada una de estas frecuencias, y a esa tabla la llamaremos "**Matriz de notas**".

Grafico matriz de notas

Así le diremos al NCO que nota debe tocar, accediendo a los valores de **M** almacenados en esta tabla.























<!---
Imagenes
![Circulo y onda](docs/assets/images/Circulo_Onda.svg) 

Indice
1. [Entendiendo la oscilación](#id1)
2. [Osciladores digitales y wavetables](#id2)


<div id='id1' />
## Entendiendo la oscilación

<div id='id2' />
## Osciladores digitales y wavetables
-->
