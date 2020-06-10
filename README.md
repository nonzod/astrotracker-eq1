# Arduino EQ1 Motorized

Automatico
* On = Insegui
* Off = Manuale

Quando è attiva la modalità "Inseguimento" la si avvia con il pulsante di rotazione > e si ferma con il pulsante di rotazione <

Quanto è attiva la modalità "Manuale" ci si sposta nelle due direzione con < e >

## Motore Stepper

Il driver è impostato per lavorare con `32 microstep`

Calcolo degli step necessari per effettuare un giro completo:

* `(microstepping)*(stepper resolution)*(EQ-1 worm gear)*(printed reduction gear)`
* `32*200*100*(41/20) = 1312000`

Giro completo in **1312000** steps

## Velocità e Tempo Siderale

[un secondo solare dura 1,00278 secondi siderali](https://it.wikipedia.org/wiki/Tempo_siderale)

[La durata di un giorno siderale è 86164,1 secondi](https://it.wikipedia.org/wiki/Giorno_siderale) (`23*60*60+56*60+4,1 = 86164,1`)

Calcolo dei secondi per step: `secondi giorno siderale / steps per giro completo`

`86164,1/1312000 = 0,06567385670731707317` (non ci serve ma giusto per averlo)

Calcolo degli step per secondo: `steps per giro completo / secondi giorno siderale`

`1312000/86164,1 = 15,22675917232350828245` arrotondato a **15,23 steps/sec**

### Stelle

Per le stelle si imposta la velocità steps/sec per il Giorno Siderale.

**15,23 steps/sec**

### Luna

La luna

## AccellStepper

* `runSpeed()` si usa per farlo girare indefinitivamente, quindi usato per i pulsanti
* `run()` segue invece le destinazioni

## Riferimenti

* [Astro Tracker for EQ1 mount](https://www.thingiverse.com/thing:1033336)
* [Specifiche DRV8825](https://www.pololu.com/product/2133)
* [Tutorial DRV8825](https://www.mauroalfieri.it/elettronica/drv8825-stepper-driver-arduino.html)
* [AccellStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html)
* [Icone per OLED](https://www.iconfinder.com/iconsets/space-exploration-set)
