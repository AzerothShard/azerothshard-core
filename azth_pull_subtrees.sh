#!/usr/bin/env bash

git subtree pull --prefix=modules/mod-transm https://github.com/azerothcore/mod-transmog master

git subtree pull --prefix=modules/mod-duelreset https://github.com/azerothcore/mod-duelreset master

git subtree pull --prefix=modules/mod-anticheat https://github.com/azerothcore/mod-anticheat master

git subtree pull --prefix=modules/mod-vas-autobalance https://github.com/azerothcore/mod-vas-autobalance master

read -p "done"
