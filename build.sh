#!/usr/bin/env bash
# $1 = input path
# $2 = output path

g++ $1 -Wall -Werror -Wpedantic --std=c++20 -o $2