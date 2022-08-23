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

Esto significa que realizamos nuestra secuencia a medida que recorremos los steps. Cada uno de estos steps, representará una figura músical, sea una corchea, semicorchea, negra, todo depende de a que velocidad estamos cambiando entre cada uno de ellos. Y con ello, un apartado que no he querido mencionar. El elemento más importante de la música. El tempo. La velocidad a la que recorremos los steps, y la duración de cada step depende del tempo. Este nos dirá cuando debemos cambiar de un step a otro, en funcion a que figura ritmica representa dicho step. Ahora, como bien sabemos, el compás más común en la música es el 4/4, de forma que la mayoría de los displays de los secuenciadores, representan un compás de 4/4, donde cada step corresponde a una semicorchea o corchea (es lo más común, pero también es común que este 4/4 se pueda subdividir o icluso la duración de los steps se pueda configurar). De esta forma los dos display de secuencia más comunes son 8 pasos (corcheas) y 16 pasos (semicorcheas), de como esos se puedan configurar no me hago responsable.

Step secuencers

Ahora una parte important de diseño, es el estado de cada step, estos son High o Low, el step puede estar "encendido" o "apagado", cada uno de estos estaos puede representar un evento. Por ejemplo, tocar una nota, o mantener silecio. Eso significa que configurando, que steps estan encendidos y cuales apagados, podemos programar una secuencia rítmica. Esta es la principal tarea de un secuenciador. Pero no solo eso, podemos ir más allá, podemos decirle tambien a un intrumento, que nota debe tocar en determinado step, de forma que nuestra secuencia ritmica ahora se ha convertido en una melodía. Ahora ¿Como hacer esto? Esa es la parte realmente buena.


## Secuenciador electrónico

Primero que nada, acá explicaré como hacer funcionar un secuenciador, no como construirlo, ya que más adelante lo implementaré en software, si alguien quiere un esquemático y listas de componentes hay muchos en internet y de calidad bastante buenos, voy a mencionar ciertos componentes porque sirven para explicar muy visualmente como funcionan estos dispositivos.

Ahora, vamos a abstraernos bastante de los elementos "técnicos" del sintetizador, vamos a hablar a grandes rasgos como funciona eso y nos centraremos en el secuenciador. 

Para crear un secuenciador lo principal que necesitamos es, algo que represente los steps, algo que nos entregue el tempo, y algo que nos permita alternar el estado de los steps. Estos 3 elementos son esenciales y se resuelven fácilmente, si alguien tuvo clases de electrónica digital seguro recuerda los contadores binarios y sus hermanos mayores los contadores decodificados. Nos centraremos en el contador decodificado, ¿Que hace? Es un circuito integrado, un chip que bueno, cuenta, jaja, pero a diferencia del contador binario, este lo hace a travez de sus pines. Es decir, a medida que el contador va contando, 0, 1, 2, 3 y así, va entregando estados a travez de sus pines de salida, usualmente correspondientes al número de la cuenta, es decir, en el 0 se enciende determinado pin, en el 1, otro pin, en el 2, otro pin distinto y así mientras que claro, cada vez que cambia la cuenta el pin del numero anterior se apaga, de forma que el contador entrega la informacion de "en que paso está" a traves de "que pin esta encendido", y una vez que llega al ultimo pin, la cuenta comienza desde 0. Así que es muy fácil pensar que esta cosa va a hacer el 80% del trabajo, y sí. Es así, con esto tenemos una representación sólida de los steps y además podemos hacer que la cuenta sea realizada a una velocidad determinada (podemos setear el tempo) ¿como?. Como buen sistema digital, este contador funciona con una señal de reloj, cada vez que detecta un cambio de voltaje positivo en la señal de reloj, el contador aumenta la cuenta. Entonces podemos ajustar el tempo del secuenciador mediante la frecuencia de la señal de reloj.

Con todo esto, lo unico que necesitamos es una forma de monitorear la secuencia, y de alternar los leds entre apagado y encendido. Bueno fácil, leds e interruptores. Conectaremos un led a cada salida del contador y un interruptor, de esta forma cada vez que un pin este encendido, se encenderá el led. Y mediante el interrptor podemos hacer que dicha señal llegue o no a nuestro "dispositivo de sonido". El diagrama de esto sería así.

Diagrama secuenciador simple

Listo, con esto tenemos un secuenciador simple, que le dice nuestro "dispositivo de sonido" mediante señales electricas cuando debe hacer ruido y cuando no, lo cual es fácil e intuitivo de controlar mediante los interruptores. Ahora obviamente los secuenciadores comerciales tienen otro tipo de controles más complejos, como el gate, trigger, y blablabla. Ya lo dije, nosotros nos centraremos en dos cosas, decirle al "dispositivo de sonido" cuando hacer ruido y que "nota debe tocar". La primera con este "aproach" ya esta cubierta. ¿Como hacemos la segunda? Bueno es más complejo, pero no imposible, digamos que nuestro "dispositivo de sonido" puede leer votaje y tocar una nota determinada de acuerdo a un nivel de voltaje correspondiente (es un VCO, pero shhh), es decir, si le llega 1 volt, toca un Do, si le llegan 2 toca un Re, así. Entonces conectaremos un potenciómetro, y un interruptor "controlado" (puede ser un transistor) despues de cada interruptor de "step". El interruptor de "step" dira si debemos tocar o no la nota, y cuando este diga que debemos tocarla, activara el interruptor controlado que le hará llegar un voltaje determinado al "dispositivo de sonido", el cual lo leerá y pum! tocará la nota correspondiente. La verdad no era tan complicado. Ahora podemos decirle al "dispositivo de sonido" que nota debe tocar mediante los potenciómetros, ya que con estos controlamos el voltaje que le entregaremos en cada step. Y así podemos configurar una secuencia rítmica, con los interruptores de step, y una secuencia de notas, con los potenciómetros. Espectacular.

Figura sec electrónico completo


























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
