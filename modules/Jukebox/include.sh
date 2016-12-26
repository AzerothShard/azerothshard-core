#!/bin/bash

JUKEBOX_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_WORLD_CUSTOM_PATHS+=(
    $JUKEBOX_MODULE_PATH"/data/sql/*"
)