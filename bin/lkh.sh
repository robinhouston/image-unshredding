#!/bin/bash

# A simple wrapper around LKH that takes arguments on the command-line
# instead of in a parameter file.

LKH=LKH-2.0.7/LKH

tempfile=$(mktemp)
cleanup() { rm -f "$tempfile"; }
trap cleanup EXIT

cat >"$tempfile" <<END
PROBLEM_FILE = $1
OUTPUT_TOUR_FILE = $2
RUNS = 5
END

"$LKH" "$tempfile"
