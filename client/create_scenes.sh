#!/bin/bash
# add info about this file
echo -e "/**\n * This file is automatically generated by make.\n * All changes will be overrided next time you compile\n**/" > scenes.hpp
echo -e "#ifndef _CREATESCENES_HPP\n#define _CREATESCENES_HPP\n" >> scenes.hpp;
#includes
echo -e "#include <iostream>\n#include <vector>\n#include <string>\n#include \"core/Scene.hpp\"\n" >> scenes.hpp
# create a include list
for file in ./inc/scenes/*
do
    echo "#include <.$file>"
done >> scenes.hpp
echo -e "\nvoid import_scenes();\n" >> scenes.hpp
echo -e "\nusing namespace std;" >> scenes.hpp
#vector of scenes
echo -e "\nstd::vector<pair<string, Scene*>> scenes;\n" >> scenes.hpp
# create vector for a objects
echo "void import_scenes() {" >> scenes.hpp

for file in ./inc/scenes/*
do
    IFS='/' read -a arr <<< "$file"
    echo -e "\tscenes.emplace_back(\"${arr[3]::-4}\", new ${arr[3]::-4}());"
done >> scenes.hpp
echo "}" >> scenes.hpp
echo -e "#endif" >> scenes.hpp

mv -f scenes.hpp inc/scenes.hpp
