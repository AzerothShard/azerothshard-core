#!/usr/bin/env bash

CROSSFACTION_MODULE_PATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

DB_EXTRA_CUSTOM_PATHS+=(
    $CROSSFACTION_MODULE_PATH"/data/sql/db-extra/"
)