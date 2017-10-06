#!/bin/bash

function check
{
  var="$*"
  DIFF=$(diff <( ./myfind ${var} 2>/dev/null) <( find ${var} 2>/dev/null))
  if [ "$DIFF" != "" ]; then
    printf "\e[31m[ FAIL ]\e[39m  ";
    printf "%b\n" "$var"
    printf "%s\n" $DIFF;
  else
    printf "\e[32m[ OK ]\e[39m\t  "
    printf "%b\n" "$var"
  fi
}
check
check ../
check src include/
check src
check ~/Desktop
check src/ -type d  \( -name ls.? -o -name libstring \)
check src \! -name src
check src/ -name src
check src -name src
check -H ~/Desktop
check -L .
check -q
check . -name ls.?
check ../ -type b
check ../ -type c
check ../ -type d
check ../ -type f
check ../ -type l
check ../ -type p
check ../ -type s
check -print -o -print
check -print -a -print
check src -exec "pwd" ";"
check src -exec "echo" "{}" ";"
check src -execdir "echo" "{}" ";"
