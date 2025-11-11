#!/bin/bash
# Script for doxygen generation. Assuming WSL or linux.
# 
# :Author: jupiterbjy@gmail.com

cd ..

echo "[SCRIPT] Fetching version"
python3 doxy_scripts/update_doxygen_version.py || exit 1

echo "[SCRIPT] Removing previous output dir"
rm -rf doxy_output

echo "[SCRIPT] Generating"
doxygen || exit 1

#echo "[SCRIPT] Moving Generated to output dir"
#mkdir doxy_output
#mv html latex doxy_output/ || exit 1

# echo "[SCRIPT] Adding korean package to refman.tex"
# python3 doxy_scripts/add_korean.py || exit 1

# echo "[SCRIPT] Building PDF"
# cd doxy_output/latex
# make || exit 1
# cp refman.pdf ../.. || exit 1
# cd ../../doxy_scripts

echo "[SCRIPT] All done!"
