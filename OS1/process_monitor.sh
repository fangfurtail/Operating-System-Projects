#!/bin/bash

# Black list that I have to check
BLACKLIST=("gedit" "firefox" "telegram")

# RAM sınırı (MB)
# RAM MAX VALUE
RAM_THRESHOLD=100

# Log  name
LOG_FILE="process_alert.log"

# Starting log
echo "$(date) 🚀 Script started. Monitoring every 10 seconds..." > "$LOG_FILE"

# Main loop 
while true; do
    echo "✅ System Check: $(date)"
    echo "$(date) 🔄 Checking processes..." >> "$LOG_FILE"

    ps -eo pid=,comm=,%mem= --sort=-%mem | while read -r pid rest; do
        name=$(echo "$rest" | awk '{print $1}')
        mem=$(echo "$rest" | awk '{print $2}')
        mem_int=$(echo "$mem" | cut -d. -f1)

        for bad in "${BLACKLIST[@]}"; do
            if [[ "$name" == "$bad" ]]; then
                echo "$(date) ❌ BLACKLIST - '$name' (PID $pid)" >> "$LOG_FILE"
            fi
        done

        if [[ "$mem_int" =~ ^[0-9]+$ ]] && (( mem_int > RAM_THRESHOLD )); then
            echo "$(date) ⚠️ HIGH RAM - '$name' (PID $pid) using $mem%" >> "$LOG_FILE"
        fi
    done

    sleep 8
done
