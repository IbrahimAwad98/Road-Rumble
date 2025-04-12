#!/bin/bash
echo "Startar test + debug..."
./game --test --debug

if [ $? -eq 0 ]; then
  echo "TEST PASSED (kod 0)"
else
  echo "TEST FAILED (kod $?)"
fi

echo ""
echo "Loggfilens: "
cat testlog.txt
