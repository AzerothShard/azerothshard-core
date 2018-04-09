#!/usr/bin/env bash

git push https://github.com/azerothcore/mod-transmog `git subtree split --prefix modules/mod-transm`:master

git push https://github.com/azerothcore/mod-duelreset `git subtree split --prefix modules/mod-duelreset`:master

git push https://github.com/azerothcore/mod-anticheat `git subtree split --prefix modules/mod-anticheat`:master

git push https://github.com/azerothcore/mod-vas-autobalance `git subtree split --prefix modules/mod-vas-autobalance`:master

read -p "done"
