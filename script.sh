#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Mod de utilizare: bash $0 <cale_fisier> <caracter>"
    exit 1
fi

cale_fisier=$1
caracter=$2

if [ ! -f "$cale_fisier" ]; then
    echo "Fișierul nu există sau nu poate fi găsit."
    exit 1
fi

while read -r line;
do
    if echo "$line" | egrep "^[A-Z].*[A-Za-z0-9\ \,\.\!\?]+$" | grep -v ",[ ]*și[]" | egrep "[\.\?\!]$" | egrep "$caracter"; then
        ((count++))
    fi
done < "$cale_fisier"


echo "Au fost identificate in total $count propozitii corecte care contin caracterul $caracter."
