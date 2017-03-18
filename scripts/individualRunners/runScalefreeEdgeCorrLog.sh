#!/bin/bash

## quit on error
set -e

PROJ_ROOT=.

./scripts/runScalefreeEdgeCorrLog.sh 2>&1 | tee >(sed 's/.*\r//'> ./data/logs/runScalefreeEdgeCorrLog.log)
