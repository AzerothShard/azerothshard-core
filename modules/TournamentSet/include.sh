#!/usr/bin/env bash

TOURNAMENTSET_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_EXTRA_CUSTOM_PATHS+=(
    $TOURNAMENTSET_MODULE_PATH"/data/sql/extra/"
)

DB_CHARACTERS_CUSTOM_PATHS+=(
    $TOURNAMENTSET_MODULE_PATH"/data/sql/characters/"
)
