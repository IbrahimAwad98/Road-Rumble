#!/bin/bash

# Startar servern i nytt Terminal-fönster med etikett "SERVER LOG"
osascript -e 'tell app "Terminal"
    do script "echo SERVER LOG; ./server"
end tell'

# Väntar 2 sekunder så att servern hinner starta
sleep 2

# Startar klienterna 1–4 i var sitt Terminal-fönster med etiketter "CLIENT 1" till "CLIENT 4"
for id in {1..4}; do
    osascript -e "tell app \"Terminal\"
        do script \"echo CLIENT $id; ./Game --ip 127.0.0.1 --id $id\"
    end tell"
    # Väntar 1 sekund så att varje klient hinner koppla upp sig
    sleep 1
done

echo "All programs started. You can now play!"
