# Report: punti di dereferenziazione senza controllo `nullptr`

Data: 2025-11-14

Scopo: elencare i punti nella codebase dove viene dereferenziato un puntatore senza un controllo esplicito di `nullptr` nel contesto della funzione.

Nota: alcuni puntatori sono inizializzati/garantiti non-null a monte (es. tramite inizializzazione del `Editor` o tramite flow dell'app). Qui segnalo i luoghi dove *manca* un controllo esplicito nella funzione che effettua la dereferenziazione.

----

1) `src/shell/VolcanoShell.cpp`

- Occorrenze principali: uso di `this->f->...` in `VolcanoShell::Update` (es. accesso a `particleCount`, `minSpeed`, `minLifetime`, `startColor`, `endColor`).
- Esempio: `while (this->n_fires < this->f->particleCount && ...)` e `star.Velocity = dir * this->f->minSpeed;`
- Diagnosi: nella funzione `Update` non c'è alcun controllo `if (!f)` prima di usare `this->f`. Se `f` fosse `nullptr` si verificherebbe UB.
- Raccomandazione: aggiungere controllo all'inizio di `Update` (es. `if (!f) return;`) oppure assicurare via contract che `f` non può essere null prima di creare/avviare questa Shell.

2) `src/shell/Shell.cpp`

- `Shell::Launch(const FireworkEvent &event)` dereferenzia `event.fire->startShellPosition`, `event.fire->startShellVelocity`, `event.fire->fuseTime` senza controlli.
- `Shell::createShell(const Firework* f)` usa `switch (f->family)` senza verificare `f`.
- Diagnosi: se viene passato un `FireworkEvent` con `fire == nullptr` o se `createShell` viene chiamata con `f == nullptr`, si avrà dereferenziazione di puntatore nullo.
- Note: il `main.cpp` attualmente chiama `Shell::createShell(eventData->fire, ...)` e `eventData` proviene da `Timeline::Update`. In pratica, `mayAddEvent` imposta `newEvent.fire = selectedFirework;` e `selectedFirework` è inizializzato al primo elemento della libreria se `nullptr`. Quindi nel flusso corrente probabilmente non arrivano `nullptr`, ma la funzione stessa non è difensiva.
- Raccomandazione: aggiungere controlli difensivi (es. `if (!f) { /* log+return or throw */ }`) o cambiare la firma a prendere `const Firework &` se non è previsto `nullptr`.

3) `src/Timeline.cpp`

- `Timeline::DrawUI` usa `events[i].fire->name.c_str()` senza controllo locale su `events[i].fire`.
- Diagnosi: gli eventi sono popolati in `mayAddEvent` usando `selectedFirework` che viene inizializzato ad un elemento valido della libreria, quindi nel flusso normale non dovrebbe essere `nullptr`. Tuttavia non c'è un controllo esplicito prima della stampa.
- Raccomandazione: aggiungere un controllo difensivo `if (events[i].fire) ImGui::Text(...); else ImGui::Text("<unknown>");`.

4) `src/main.cpp`

- Uso: `shellPtr = Shell::createShell(eventData->fire, particleSystem);` senza controllo esplicito su `eventData->fire`.
- Diagnosi: `eventData` proviene da `timeline.Update()` e contiene puntatori ad eventi interni; dato il flusso attuale, `eventData->fire` è probabilmente non-null, ma il punto di chiamata non verifica la condizione. Rimane un punto dove una modifica futura potrebbe introdurre `nullptr`.
- Raccomandazione: aggiungere controllo prima di chiamare `createShell`, o rendere `createShell` robusta contro `nullptr`.

5) `src/Timeline.cpp` (altre occorrenze collegate a `selectedFirework`)

- In `mayAddEvent` c'è codice che assume `selectedFirework` non-null in alcuni punti (es. `selectedFirework->id` quando costruisco il combobox). Tuttavia, `selectedFirework` viene inizializzato se è `nullptr` a `&lib[0]` — questo è corretto solo se `lib` non è vuoto. Se `lib` fosse vuoto, qui ci sarebbe UB.
- Raccomandazione: aggiungere guardie in `mayAddEvent` come `if (lib.empty()) return;` e assicurarsi che `selectedFirework` sia valido.

6) Altre shell: `PeonyShell`, `WillowShell`, `ChrysanthemumShell`

- Queste implementazioni effettuano un controllo `if (!f) return;` all'inizio di `explode()` e quindi appaiono difensive e sicure nelle funzioni di esplosione.

----

Raccomandazioni generali

- Dove possibile, evitare passare puntatori potenzialmente null: usare riferimento (`const Firework &`) quando il valore è obbligatorio.
- Se il puntatore può essere null per motivi di design, rendere le funzioni che lo dereferenziano difensive con `if (!ptr) { /* log */ return; }` oppure usare `assert(ptr)` se si vuole che il programma fallisca in debug.
- Per l'interfaccia pubblica (es. `Shell::createShell`), preferire contratti chiari: o accettare `const Firework&` oppure documentare e controllare il comportamento per `nullptr`.

Esempi di patch veloci

- Aggiungere controllo in `VolcanoShell::Update`:

```cpp
void VolcanoShell::Update(float dt)
{
    if (!this->f)
        return;
    // ... resto del codice ...
}
```

- Rendere `Shell::createShell` difensiva:

```cpp
std::unique_ptr<Shell> Shell::createShell(const Firework* f, ParticleSystem &ps)
{
    if (!f) {
        std::cerr << "Shell::createShell called with nullptr f" << std::endl;
        return nullptr; // oppure assert(false) in debug
    }
    switch (f->family) { ... }
}
```

----

Se vuoi, applico io le patch per i punti più critici (ad es. `VolcanoShell::Update`, `Shell::Launch`/`createShell`, e i guard check in `Timeline::DrawUI`), eseguendo una build per verificare che non introduca regressioni. Vuoi che proceda con le patch automatiche ora?
