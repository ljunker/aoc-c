#!/bin/sh
set -e

SCRIPT_DIR=$(cd -- "$(dirname -- "$0")" && pwd)
NEWDAY="${SCRIPT_DIR}/newday.sh"

YEAR=$(date +%Y)
MONTH=$(date +%m)
DAY=$(date +%d)

if [ "$MONTH" != "12" ]; then
    echo "Warning: It's month $MONTH, AoC is usually in December."
fi

echo "Creating year $YEAR day $DAY..."
"$NEWDAY" "$YEAR" "$DAY"
