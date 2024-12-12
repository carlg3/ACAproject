#!/bin/bash

# File contenente la lista di host (ad esempio, "hostfile")
HOSTFILE="../hostfile"

# Controlla che il file hostfile esista
if [[ ! -f "$HOSTFILE" ]]; then
    echo "Errore: Il file $HOSTFILE non esiste."
    exit 1
fi

# Estrai gli indirizzi IP dalla lista ignorando i parametri aggiuntivi (es. slots=8)
hosts=$(awk '{print $1}' "$HOSTFILE")

# Itera su ogni host
for host in $hosts; do
    echo "Testando connessione SSH con $host..."

    # Test della connessione SSH
    ssh -o BatchMode=yes -o ConnectTimeout=5 -o StrictHostKeyChecking=no "$host" exit &>/dev/null

    # Verifica l'esito del comando
    if [[ $? -eq 0 ]]; then
        echo "Connessione SSH riuscita con $host."
    else
        echo "Connessione SSH fallita con $host."
    fi
done

echo "Test completati."