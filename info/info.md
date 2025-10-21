**longitud**: dos bytes big endian
**códigos de acción** se enviarán en un byte con el valor literal.

---

**Protocolo cliente:**

- **Nombre de usuario**

- 0x00 <LENGTH> <USERNAME>
    - LENGTH **longitud** del nombre de usuario.
    - USERNAME string con el nombre de usuario.

- **Conexion** (le avisa al server si crear o unirse a una sala)

- 0x01 <CREATE-ROOM|JOIN-ROOM> <CHOSEN-ROOM>
    - CREATE-ROOM es 0x02 
    - JOIN-ROOM es 0x03, ambos de un byte. Si se elige este, se debe indicar CHOSEN-ROOM.
    - CHOSEN-ROOM es un byte que indica la sala elegida.    

- **Iniciar el juego** (elige el mapa, n carreras) (creador del lobby)

- 0x04 <QUANTITY-RACES> [<RACE-INFO>] 
    - QUANTITY-RACES es un byte.
    - RACE-INFO es <LENGTH> <MAP> <ROUTE>
        - LENGTH **longitud** del nombre del MAP.
        - MAP un string con el nombre del mapa 
        - ROUTE un byte que indica el recorrido elegido.

- 0x05 <MOVE>
    - MOVE es un byte que indica la dirección:
        - 0x01: Arriba
        - 0x02: Abajo
        - 0x03: Izquierda
        - 0x04: Derecha

- **Elegir Auto**

- 0x06 <CAR-ID>
    - CAR-ID un byte indicando el auto elegido.

- **Mejoras de auto**

- 0x07 <IMPROVEMENT>
    - IMPROVEMENT es un byte, ejemplo 0x01 mejora velocidad.

- **Cheats al server**

- 0x08 <CHEAT>
    - CHEAT un byte.

---

**Protocolo server:**

- **Todas las salas disponibles**

- 0x21 <NUMBER-ROOMS> [<ROOM-INFO>]
    - NUMBER-ROOMS es un byte con la cantidad de salas disponibles.
    - ROOM-INFO es <ROOM-ID> <CURRENT-PLAYERS> <MAX-PLAYERS>
        - ROOM-ID es un byte que identifica la sala.
        - CURRENT-PLAYERS es un byte con la cantidad de jugadores actuales en la sala.
        - MAX-PLAYERS es un byte con la cantidad máxima de jugadores en la sala.

- **Todos los autos disponibles para usar**

- 0x22 <NUMBER-CARS> [<CAR-INFO>]
    - NUMBER-CARS es un byte con la cantidad de autos disponibles.
    - CAR-INFO es <CAR-ID> <SPEED> <ACCELERATION> <HEALTH> <MASS> <CONTROLLABILITY>
        - CAR-ID byte que identifica el auto.
        - SPEED byte con la velocidad del auto.
        - ACCELERATION byte con la aceleración del auto.
        - HEALTH byte con la salud del auto.
        - MASS byte con la masa del auto.
        - CONTROLLABILITY byte con la controlabilidad del auto.

- **Arranque de carrera** le manda info de la carrera actual (mapa, recorrido)

- 0x23 <LENGTH> <MAP> <AMOUNT-CHECKPOINTS> [<CHECKPOINT-POSITIONS>]
    - LENGTH **longitud** del nombre del mapa.
    - MAP string con el nombre del mapa.
    - AMOUNT-CHECKPOINTS byte con la cantidad de checkpoints.
    - CHECKPOINT-POSITIONS ??? a definir.

- **Mandar info del resultado de la carrera** (ya sea al finalizar una carrera o la partida completa)

- 0x24 <NUMBER-PLAYERS> [<PLAYER-RESULTS-CURRENT>] [<PLAYER-RESULTS-TOTAL>]
    - NUMBER-PLAYERS byte con la cantidad de jugadores.
    - PLAYER-RESULTS-CURRENT es <LENGTH> <USERNAME> <TIME>
        - LENGTH **longitud** del nombre de usuario.
        - USERNAME string con el nombre del usuario.
        - TIME dos bytes big endian con el tiempo actual en segundos.
    - PLAYER-RESULTS-TOTAL es <LENGTH> <USERNAME> <TOTAL-TIME>
        - LENGTH **longitud** del nombre de usuario.
        - USERNAME string con el nombre del usuario.
        - TOTAL-TIME cuatro bytes big endian con el tiempo total en segundos.

- **Info del mapa** NPC, eventos(choques) (broadcast cada 0.25)

- 0x25 <NUMBER-PLAYERS> [<PLAYER-INFO>] <NUMBER-NPC> [<NPC-INFO>] <NUMBER-EVENTS> [<EVENTS>]
    - NUMBER-PLAYERS byte con la cantidad de jugadores.
    - PLAYER-INFO es <LENGTH> <USERNAME> <CAR-ID> <POSITION-X> <POSITION-Y>
        - LENGTH **longitud** del nombre de usuario.
        - USERNAME string con el nombre del usuario.
        - CAR-ID byte con el id del auto.
        - POSITION-X cuatro bytes big endian con la posición X.
        - POSITION-Y cuatro bytes big endian con la posición Y.
    - NUMBER-NPC byte con la cantidad de NPCs.
    - NPC-INFO es <NPC-ID> <POSITION-X> <POSITION-Y>
        - NPC-ID byte con el id del NPC. Por ahora es equivalente a CAR-ID.
        - POSITION-X cuatro bytes big endian con la posición X.
        - POSITION-Y cuatro bytes big endian con la posición Y.
    - NUMBER-EVENTS byte con la cantidad de eventos.
    - EVENTS es <EVENT-TYPE> <LENGTH> <USERNAME>
        - EVENT-TYPE byte con el tipo de evento.
        - LENGTH **longitud** del nombre de usuario.
        - USERNAME string con el nombre del usuario.

- **Fin de partida**

- 0x26