#!/bin/bash

## quit on error
set -e

PROJ_ROOT=.

./scripts/runRandSpatialMissLog.sh 2>&1 | tee >(sed 's/.*\r//'> ./data/logs/runRandSpatialMissLog.log)