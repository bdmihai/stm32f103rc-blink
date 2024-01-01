#!/bin/sh

git submodule add git@github.com:bdmihai/stm32-doc doc/stm32
git submodule add git@github.com:bdmihai/stm32-qbs qbs/modules/stm32
git submodule add git@github.com:bdmihai/stm32-boot source/boot
git submodule add git@github.com:bdmihai/stm32-runtime source/runtime
git submodule add git@github.com:bdmihai/stm32-hal source/hal
git submodule add git@github.com:bdmihai/stm32-cmsis source/cmsis
git submodule add git@github.com:bdmihai/stm32-freertos source/freertos

