#!/bin/sh
make -C src clean

rm -v bin/*

rm -v disk/*.bin

rm -v bochs/*.img