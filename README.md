# Comandi utili
## Come eseguire

Una volta scaricata la cartella eseguire il comando:

```shell
make
```

Che in automatico compilerà tutto il necessario.

&nbsp;

## Come testare

Dopo aver compilato, potete testare aprendo "umps3" dal terminale dalla stessa cartella di dove avete usato il comando presente sopra, usando il comando:

```shell
umps3
```

Una volta che lo avete aperto, vi basterà creare una nuova configurazione di macchina all'interno della stessa cartella e poi premere il tasto verde che si trova in alto a sinistra.

Potete vedere il risultato dell'esecuzione premendo ```Alt+0``` oppure andando su ```Windows > Terminal 0``` nella barra in alto.

&nbsp;

## Come vedere la documentazione

La documentazione del nostro codice può essere vista direttamente dai vari ```.h```, con commenti specifici della funzione di una funzione presenti nel ```.c```.

Se invece si desiderasse vedere unicamente la descrizione dello scopo della varie funzioni, si può generare una pagina web che mostra la documentazione. Per fare questo, eseguire i seguenti comandi:

```shell
sudo apt install doxygen
```

```shell
make open_doc
```