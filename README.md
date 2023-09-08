# Comandi utili
## Come eseguire

Una volta scaricata la cartella eseguire il comando:

```shell
make all
```

Che in automatico compilerà tutto il necessario.

&nbsp;

## Come testare

Dopo aver compilato, potete testare eseguendo il comando:

```shell
make run
```

&nbsp;

## Come vedere la documentazione

La documentazione del nostro codice può essere vista direttamente dai vari ```.h```, con commenti specifici relativi ad una funzione, oltre ad altre informazioni presenti nel ```.c```.

Se invece si desiderasse vedere unicamente la descrizione dello scopo della varie funzioni, si può generare una pagina web che mostra la documentazione. Per fare questo, eseguire i seguenti comandi:

```shell
sudo apt install doxygen
```

```shell
make open_doc
```