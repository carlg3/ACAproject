#!/bin/bash

for FILE in "$1"/*; do
  if [ -f "$FILE" ]; then
	  OUTPUT=$(basename "$FILE")
	  echo $OUTPUT
  fi
done
