#!/bin/bash

ARENA_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_WORLD_CUSTOM_PATHS+=(
    $ARENA_MODULE_PATH"/data/sql/*"
)