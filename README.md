# dobot-kinematics
C-based CLI simulation of the Dobot Magician robotic arm with forward and inverse kinematics

Účelem tohoto projektu je vytvořit roznumou cmd aplikaci, která bude kontrolovat a převádět koordináty robota Dobot Magician.
Projekt byl vytvořen v rámci semestrální práce z programovacího jazyka C.

Program umí následující věci:
- převod úhlů jednotlivých kloubů na pozici koncového TCP - přímá kinematika
- převod pozic TCP na jednotlivé úhly kloubů - inverzní kinematika
- ASCII vizualizace pro přibližnou kontrolu
- Práce se soubory čtení a zápis koordinátů z a do souboru
- Jednoduché CLI pro automatizaci bez nutnosti interakce - hodí se pro validaci dat v souboru

Struktura projektu:
- /src/       - zdrojové kódy aplikace
- /include/   - hlavičkové soubory
- /build/     - výstup kompiace (výsledná aplikace pro spuštění)

Program pracuje se soubory:
- joints.dat: Soubor s úhly kloubů J1 J2 J3
- positions.dat: Soubor s pozicemi TCP x y z
- results_joints.dat: Výstupní soubor s převody z TCP na J1 J2 J3
- results_positions.dat: Výstupní soubor s převody J1 J2 J3 na TCP x y z

Konfigurace parametrů robota a defaultních názvů souborů je uložena v hlavičkovém souboru config.h

Kompilace MacOS, Linux:
    - potřeba mít nainstalován gcc a make
    - $ make

Kompilace Windows (komplikace):
- doporučuji: stáhnout balíček pro distribuci a building aplikací: https://www.msys2.org
- $ pacman -S mingw-w64-x86_64-gcc
- $ pacman -S make

- Pro správné zobrazení diakritiky v cmd zadej: chcp 65001

Spouštění:
    $ ./build/dobot_kin

Parametry CMD:
- $ ./build/dobot_kin -h                           Nápověda k aplikaci
- $ ./build/dobot_kin -f joints.dat -o out.dat     Převod úhlů J1, J2, J3 ze souboru joints.dat na pozice x, y, z do souboru out.dat
- $ ./build/dobot_kin -i positions.dat -o out.dat  Převod pozic TCP ze souboru positions.dat na úhly J1, J2, J3 do souboru out.dat
- $ ./build/dobot_kin -f joints.dat -a             Zobrazení vizualizace ASCII robota s krokováním dat ze souboru joints.dat

