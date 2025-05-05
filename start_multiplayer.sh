#!/bin/bash

# Startar servern i nytt Terminal-fönster med etikett "SERVER LOG"
osascript -e 'tell app "Terminal"
    do script "echo SERVER LOG; ./server"
end tell'

# Väntar 2 sekunder så att servern hinner starta
sleep 2

# Startar första klienten i nytt Terminal-fönster med etikett "CLIENT 0"
osascript -e 'tell app "Terminal"
    do script "echo CLIENT 0; ./Game --ip 127.0.0.1 --id 0"
end tell'

# Väntar 1 sekund till
sleep 1

# Startar andra klienten i nytt Terminal-fönster med etikett "CLIENT 1"
osascript -e 'tell app "Terminal"
    do script "echo CLIENT 1; ./Game --ip 127.0.0.1 --id 1"
end tell'

echo "All programs started. You can now play!"
