#!/bin/bash

echo "#ifndef __CONFIG_H__" > config.h
echo "#define __CONFIG_H__" >> config.h
while read -r line; do
  if [[ `expr "$line" : "CONFIG_"` -ne 7 ]]; then
    continue;
  fi;
  name=`echo $line | cut -d"=" -f1`
  value=`echo $line | cut -d"=" -f2`
  echo "#define $name $value" >> config.h
done < $1
echo "#endif" >> config.h
