#!/bin/bash
# Skript som kompilerar, kör och verifierar testkörning av spelet med --test och --debug

# Visar att teststarten har påbörjats
echo "Startar test + debug..."

# Kör spelet med flaggorna --test och --debug
./game --test --debug

# Kontrollerar om föregående kommando (./game) lyckades (returvärde 0 betyder OK)
if [ $? -eq 0 ]; then
  echo "TEST PASSED (kod 0)"  # Lyckad körning
else
  echo "TEST FAILED (kod $?)" # Misslyckad körning, visar returkod
fi

# Ny rad för tydlighet
echo ""

# Visar innehållet i loggfilen (t.ex. testresultat, debug-info)
echo "Loggfilens innehåll:"
cat testlog.txt
