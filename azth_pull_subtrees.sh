#!/usr/bin/env bash

git subtree pull --prefix=modules/mod-transmog mod-transmog master

git subtree pull --prefix=modules/mod-duelreset mod-duelreset master

git subtree pull --prefix=modules/mod-anticheat mod-anticheat master

git subtree pull --prefix=modules/mod-vas-autobalance mod-vas-autobalance master

read -p "done"
