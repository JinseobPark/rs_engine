#!/bin/bash
# Script for doxygen default header generation.
# 
# :Author: jupiterbjy@gmail.com

cd ..
doxygen -w latex header.tex footer.tex doxygen.sty
mv header.tex footer.tex doxygen.sty doxy_scripts/
cd doxy_scripts/

echo "Generated default headers as 'header.tex / footer.tex / doxygen.sty'."
