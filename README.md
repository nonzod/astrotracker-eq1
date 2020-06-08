# Arduino EQ1 Motorized

Automatico
* On = Insegui
* Off = Manuale

Quando è attiva la modalità "Inseguimento" la si avvia con il pulsante di rotazione > e si ferma con il pulsante di rotazione <

Quanto è attiva la modalità "Manuale" ci si sposta nelle due direzione con < e >

## AccellStepper



* `runSpeed()` si usa per farlo girare indefinitivamente, quindi usato per i pulsanti
* `run()` segue invece le destinazioni

## Velocità

```c
//may deviate depending on your individual arduino quartz. delay between steps for tracking in
//1/100.000 seconds, lower number = faster movement
//(rest of the program adds +20/1.000.000 seconds)
//calculate like this:
//steps per full turn = 16(microstepping)*200(stepper resolution)*100(EQ-1 worm gear)*41/20(printed reduction gear)
//steps per full turn = 656.000
//seconds per siderian day (=full day turn) = 23*60*60+56*60+4,1 = 86164,1
//seconds per step = seconds per siderian day / steps per full turn = 0,1313477
//0,131348 = 1/1.000.000 * 131348 / in microseconds
//131348/10 (map() has an integer limit, circumvent it by /10 *10)
//13135 that's the number we take as our base
//it may need small adjustments because of arduino quartz deviations etc.
```

### Stelle

23 ore, 56 minuti e 4.1 secondi circa.

Considerando i due ingranaggi per fare un giro completo occorrono 1312000 steps

giro completo con ingranaggi: 1312000

82800+3360+4.1

86164.1

86164100

### Luna

`Comunque abbiamo trovato gli stessi dati, ovvero 0.61°/h in longitudine e -0.05°/h in latitudine. Quindi il tuo calcolo per la variazione al minuto era giusto cioè 0.61/60=0,01°/m. Mentre per la latitudine non ha senso fare i calcoli perché basta che vada bene per quello più grande. Ora però credo che l'ordine di grandezza giusto siano i secondi quindi 0.01/60=0.0002°/s.... Non sono un esperto di fotografia, ma anche con un esposizione di 2 secondi avresti un errore di 0,0004°, che penso sia dentro i limiiti.`

## Riferimenti

* [Astro Tracker for EQ1 mount](https://www.thingiverse.com/thing:1033336)
* [Tutorial DRV8825](https://www.mauroalfieri.it/elettronica/drv8825-stepper-driver-arduino.html)
