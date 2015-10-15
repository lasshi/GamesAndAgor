#!/bin/bash
# add info about this file
echo -e "/**\n * This file is automatically generated by make.\n * All changes will be overrided next time you compile\n**/" > scenes.h
echo -e "#pragma once" >> scenes.h;
#includes
echo -e "#include <iostream>\n#include <vector>\n#include <string>\n" >> scenes.h
# create a include list
for file in ./inc/scenes/*
do
    echo "#include <.$file>"
done >> scenes.h
echo -e "\nusing namespace std;" >> scenes.h
#vector of scenes
echo -e "\nstd::vector<pair<string, Scene*>> scenes;\n" >> scenes.h
# create vector for a objects
echo "extern void import_scenes() {" >> scenes.h

for file in ./inc/scenes/*
do
    IFS='/' read -a arr <<< "$file"
    echo -e "\tscenes.emplace_back(\"${arr[3]::-4}\", new ${arr[3]::-4}());"
done >> scenes.h
echo "}" >> scenes.h

mv -f scenes.h inc/scenes.h
