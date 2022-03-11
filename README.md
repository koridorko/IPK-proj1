# IPK - Projekt 1
Jedná sa o jednoduchý socket server komunikujúci pomocou protokolu HTTP implementovaný v jayzku C/C++.
Server načúva na zadanom porte a vracia odpovede na požiadavky týkajúce sa informácií o systéme.


## Stiahnutie
```shell
git clone https://github.com/koridorko/IPK-proj1.git
```

## Použitie

```shell
make
./hinfosvc [port]
```
## Testované hodnoty

```shell
./hinfosvc 12345&
```

```shell
curl http://localhost:12345/hostname
```
merlin.fit.vutbr.cz

```shell
curl http://localhost:12345/cpu-name
```
Intel(R) Xeon(R) CPU E5-2640 0 @ 2.50GHz

```shell
curl http://localhost:12345/load
```
7.434134%


## Autor
Štefan Gajdošík\
xgajdo30