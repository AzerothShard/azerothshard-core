#!/usr/bin/env bash

TOURNAMENT_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_EXTRA_CUSTOM_PATHS+=(
    $TOURNAMENT_MODULE_PATH"/data/sql/extra"
)